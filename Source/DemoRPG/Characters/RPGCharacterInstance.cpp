// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGCharacterInstance.h"

#include "AbilitySystemComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "RPGCharacter.h"
#include "RPGCharacterAppearance.h"
#include "RPGCharacterPreview.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DemoRPG/RPGAssetManager.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/Components/RPGInventoryMappingContainer.h"
#include "DemoRPG/Items/RPGItem.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Components/RPGEquippableItem.h"
#include "DemoRPG/Items/Components/RPGInventory.h"
#include "DemoRPG/Items/Features/RPGEquippableFeature.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/UObjectGlobals.h"

void FRPGEquippedItemsArrayEntry::PreReplicatedRemove(const FRPGEquippedItemsArray& InArraySerializer)
{
	bIsAboutToBeRemoved = true;
	if (IsValid(ItemInstance) && IsValid(InArraySerializer.EquippingCharacter))
	{
		InArraySerializer.EquippingCharacter->OnEquippedItemRemoved_Internal(ItemSlot, ItemInstance);
	}
}

void FRPGEquippedItemsArrayEntry::PostReplicatedAdd(const FRPGEquippedItemsArray& InArraySerializer)
{
	if (IsValid(ItemInstance) && IsValid(InArraySerializer.EquippingCharacter))
	{
		InArraySerializer.EquippingCharacter->OnEquippedItemAdded_Internal(ItemSlot, ItemInstance);
	}
}

void FRPGEquippedItemsArrayEntry::PostReplicatedChange(const FRPGEquippedItemsArray& InArraySerializer)
{
	if (IsValid(ItemInstance) && IsValid(InArraySerializer.EquippingCharacter))
	{
		if (bWasPostReplicatedAddCalled)
		{
			InArraySerializer.EquippingCharacter->OnEquippedItemAdded_Internal(ItemSlot, ItemInstance);
		}
		else
		{
			InArraySerializer.EquippingCharacter->OnEquippedItemChanged_Internal(ItemSlot, ItemInstance);
		}
	}
}

bool FRPGEquippedItemsArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FastArrayDeltaSerialize<FRPGEquippedItemsArrayEntry, FRPGEquippedItemsArray>(ItemEntries, DeltaParams, *this);
}

ARPGCharacterInstance::ARPGCharacterInstance()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Ability related
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Inventory
	InventoryComponent = CreateDefaultSubobject<URPGInventory>(TEXT("InventoryComponent"));
	InventoryMappingContainer = CreateDefaultSubobject<URPGInventoryMappingContainer>(TEXT("InventoryMappingContainer"));
}

void ARPGCharacterInstance::SetHighlighted(const bool bHighlighted)
{
	bIsHighlighted = bHighlighted;
	TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
	MeshComponents.Add(GetMesh());
	for (USkeletalMeshComponent* MeshComponent : MeshComponents)
	{
		MeshComponent->SetRenderCustomDepth(bIsHighlighted);
		MeshComponent->SetCustomDepthStencilValue(STENCIL_CHARACTER_OUTLINE);
	}
}

void ARPGCharacterInstance::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ARPGCharacterInstance::BeginPlay()
{
	Super::BeginPlay();
	
	// Create a preview actor
	if (CharacterPreviewClass)
	{
		FVector const SpawnLocation = CharacterPreviewClass->GetDefaultObject<ARPGCharacterPreview>()->SpawnLocation;
		
		FActorSpawnParameters Parameters;
		Parameters.bNoFail = true;
		CharacterPreview = GetWorld()->SpawnActor<ARPGCharacterPreview>(CharacterPreviewClass, SpawnLocation, {}, Parameters);
		if (IsValid(Character))
		{
			CharacterPreview->LoadBasePreview(Character->CharacterAppearance, URPGFunctionLibrary::GetGameplayTagShortName(CharacterSex));
		}
	}
	
	// Populate character's inventory with items from the default inventory map
	if (HasAuthority())
	{
		for (const TPair<URPGItem*, int>& ItemPair : DefaultInventory)
		{
			if (IsValid(ItemPair.Key))
			{
				const FAddRemoveItemResult Result = InventoryComponent->AddItem(ItemPair.Key, ItemPair.Value);
			}
		}
	}
}

void ARPGCharacterInstance::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (HasAuthority())
	{
		InventoryComponent->OnInventoryItemAddedNative.AddUObject(InventoryMappingContainer, &URPGInventoryMappingContainer::OnItemAdded);
		InventoryComponent->OnInventoryItemRemovedNative.AddUObject(InventoryMappingContainer, &URPGInventoryMappingContainer::OnItemRemoved);
		InventoryComponent->OnInventoryItemRemovedNative.AddUObject(this, &ARPGCharacterInstance::OnItemRemoved);
	}
}

void ARPGCharacterInstance::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (IsValid(Character))
	{
		LoadBaseAppearance(Character);
	}
}

void ARPGCharacterInstance::PostInitProperties()
{
	Super::PostInitProperties();
	EquippedItems.EquippingCharacter = this;
}

void ARPGCharacterInstance::ApplyModel(URPGCharacterModel* Model)
{
	TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
	for (USkeletalMeshComponent* Component : MeshComponents)
	{
		Component->DestroyComponent();
	}
	GetMesh()->SetSkeletalMesh(Model->SkeletalMesh);
}

void ARPGCharacterInstance::ApplyModel(URPGModularCharacterModel* Model)
{
	TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
	for (const FRPGModularModelPart& ModelPart : Model->ModelParts)
	{
		USkeletalMeshComponent* Component = nullptr;
		if (ModelPart.ModelPartTag == URPGCharacterAppearanceTags::Body)
		{
			Component = GetMesh();
		}
		else
		{
			Component = URPGFunctionLibrary::GetComponentByTag<USkeletalMeshComponent>(MeshComponents, ModelPart.ModelPartTag);
			if (!IsValid(Component))
			{
				Component = NewObject<USkeletalMeshComponent>(this, ModelPart.ModelPartTag);
				Component->ComponentTags.Append({ URPGCharacterAppearanceTags::Appearance, ModelPart.ModelPartTag });
				Component->RegisterComponent();
				AddInstanceComponent(Component);
				Component->SetRenderCustomDepth(bIsHighlighted);
				Component->SetCustomDepthStencilValue(STENCIL_CHARACTER_OUTLINE);
			}
			Component->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			Component->SetMasterPoseComponent(GetMesh());
		}
		Component->SetSkeletalMesh(ModelPart.SkeletalMesh);
		MeshComponents.Remove(Component);
	}
	for (USkeletalMeshComponent* Component : MeshComponents)
	{
		Component->DestroyComponent();
	}
}

void ARPGCharacterInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ARPGCharacterInstance, EquippedItems);
}

void ARPGCharacterInstance::EquipItem(ARPGItemInstance* ItemInstance, const FName ItemSlot)
{
	// Item should be equippable
	check(ItemInstance->GetItem());
	const URPGEquippableFeature* EquippableFeature = ItemInstance->GetItem()->GetFeature<URPGEquippableFeature>();
	if (!IsValid(EquippableFeature) || EquippableFeature->ItemSlots.Num() < 1 || ItemSlot != NAME_None && !EquippableFeature->ItemSlots.Contains(ItemSlot))
	{
		return;
	}
	const FName SlotToEquipInto = ItemSlot == NAME_None ? EquippableFeature->ItemSlots[0] : ItemSlot;

	// Unequip currently equipped item for the appropriate item slot
	FRPGEquippedItemsArrayEntry* FoundEntry = EquippedItems.ItemEntries.FindByPredicate([&](const FRPGEquippedItemsArrayEntry& Entry)
	{
		return Entry.ItemSlot == SlotToEquipInto;
	});
	if (FoundEntry && IsValid(FoundEntry->ItemInstance))
	{
		UnequipItem(FoundEntry->ItemInstance);
	}

	// Equipped items should always be in the equipping character's inventory
	if (!InventoryComponent->ContainsItem(ItemInstance))
	{
		URPGFunctionLibrary::MoveItem(ItemInstance, false, InventoryComponent, InventoryMappingContainer, { -1, -1 });
	}

	// Tell the item to remember who is equipping it
	URPGEquippableItem* EquippableComponent = ItemInstance->FindComponentByClass<URPGEquippableItem>();
	if (IsValid(EquippableComponent))
	{
		EquippableComponent->SetEquippingCharacter(this);
	}

	// Add to equipped items array
	const int Index = EquippedItems.ItemEntries.Add(FRPGEquippedItemsArrayEntry(SlotToEquipInto, ItemInstance));
	EquippedItems.MarkItemDirty(EquippedItems.ItemEntries[Index]);
	EquippedItems.ItemEntries[Index].PostReplicatedAdd(EquippedItems);
}

void ARPGCharacterInstance::UnequipItem(ARPGItemInstance* ItemInstance)
{
	check(ItemInstance->GetItem());
	const URPGEquippableFeature* EquippableFeature = ItemInstance->GetItem()->GetFeature<URPGEquippableFeature>();
	if (!IsValid(EquippableFeature))
	{
		return;
	}

	bool bItemWasUnequipped = false;
	for (int i = EquippedItems.ItemEntries.Num() - 1; i >= 0; i--)
	{
		if (EquippedItems.ItemEntries[i].ItemInstance == ItemInstance)
		{
			EquippedItems.ItemEntries[i].PreReplicatedRemove(EquippedItems);
			EquippedItems.ItemEntries.RemoveAt(i);
			bItemWasUnequipped = true;
		}
	}
	if (bItemWasUnequipped)
	{
		EquippedItems.MarkArrayDirty();
	}

	URPGEquippableItem* EquippableComponent = ItemInstance->FindComponentByClass<URPGEquippableItem>();
	if (IsValid(EquippableComponent))
	{
		EquippableComponent->SetEquippingCharacter(nullptr);
	}
}

bool ARPGCharacterInstance::IsEquipped(ARPGItemInstance* ItemInstance) const
{
	const URPGEquippableItem* EquippableComponent = ItemInstance->FindComponentByClass<URPGEquippableItem>();
	if (IsValid(EquippableComponent) && EquippableComponent->GetEquippingCharacter() == this)
	{
		return true;
	}
	return false;
}

FName ARPGCharacterInstance::GetEquippedItemSlot(ARPGItemInstance* ItemInstance) const
{
	for (const FRPGEquippedItemsArrayEntry& Entry : EquippedItems.ItemEntries)
	{
		if (Entry.ItemInstance == ItemInstance)
		{
			return Entry.ItemSlot;
		}
	}
	return NAME_None;
}

FName ARPGCharacterInstance::FindSlotForItem(ARPGItemInstance* ItemInstance) const
{
	URPGEquippableFeature* EquippableFeature = ItemInstance->GetItem()->GetFeature<URPGEquippableFeature>();
	if (!IsValid(EquippableFeature) || EquippableFeature->ItemSlots.Num() < 1)
	{
		return NAME_None;
	}

	for (const FName ItemSlot : EquippableFeature->ItemSlots)
	{
		const FRPGEquippedItemsArrayEntry* FoundEntry = EquippedItems.ItemEntries.FindByPredicate([&](const FRPGEquippedItemsArrayEntry& Entry)
		{
			return Entry.ItemSlot == ItemSlot;
		});
		if (!FoundEntry)
		{
			return ItemSlot;
		}
	}
	return EquippableFeature->ItemSlots[0];
}

void ARPGCharacterInstance::OnEquippedItemRemoved_Internal(const FName InItemSlot, ARPGItemInstance* InItemInstance)
{
	if (HasAuthority())
	{
		URPGFunctionLibrary::HideEquippedItemInUI(InItemInstance);
		if (InventoryComponent->ContainsItem(InItemInstance))
		{
			URPGFunctionLibrary::ShowItemInInventory(InItemInstance, InventoryMappingContainer, { 0, 0 });
		}
	}
	RefreshAppearance();
}

void ARPGCharacterInstance::OnEquippedItemAdded_Internal(const FName InItemSlot, ARPGItemInstance* InItemInstance)
{
	if (HasAuthority())
	{
		URPGFunctionLibrary::ShowEquippedItemInUI(InItemInstance);
		if (InventoryComponent->ContainsItem(InItemInstance))
		{
			URPGFunctionLibrary::HideItemInInventory(InItemInstance, InventoryMappingContainer);
		}
	}
	RefreshAppearance();
}

void ARPGCharacterInstance::OnEquippedItemChanged_Internal(const FName InItemSlot, ARPGItemInstance* InItemInstance)
{
	if (HasAuthority())
	{
		URPGFunctionLibrary::ShowEquippedItemInUI(InItemInstance);
		if (InventoryComponent->ContainsItem(InItemInstance))
		{
			URPGFunctionLibrary::HideItemInInventory(InItemInstance, InventoryMappingContainer);
		}
	}
	RefreshAppearance();
}

void ARPGCharacterInstance::OnItemRemoved(ARPGItemInstance* RemovedItem)
{
	if (IsEquipped(RemovedItem))
	{
		UnequipItem(RemovedItem);
	}
}

void ARPGCharacterInstance::UpdateAppearanceFromEquipment(ARPGItemInstance* InItemInstance)
{
	check(InItemInstance->GetItem());
	URPGEquippableFeature* EquippableFeature = InItemInstance->GetItem()->GetFeature<URPGEquippableFeature>();
	if (!IsValid(EquippableFeature))
	{
		return;
	}

	if (IsValid(Character) && IsValid(Character->CharacterAppearance))
	{
		const FName ModelTag = URPGFunctionLibrary::GetGameplayTagShortName(CharacterSex);
		URPGModularCharacterModel* Model = Cast<URPGModularCharacterModel>(Character->CharacterAppearance->GetModel(ModelTag));
		if (IsValid(Model))
		{
			FRPGModularModelPartReplacement* BodyReplacement = EquippableFeature->GetModel(Character->CharacterAppearance->AppearanceName, Model->ModelTag, URPGCharacterAppearanceTags::Body);
			if (BodyReplacement && IsValid(EquippableFeature))
			{
				GetMesh()->SetSkeletalMesh(BodyReplacement->SkeletalMesh);
			}
			TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
			for (const FRPGModularModelPart& ModelPart : Model->ModelParts)
			{
				USkeletalMeshComponent* Component = URPGFunctionLibrary::GetComponentByTag<USkeletalMeshComponent>(MeshComponents, ModelPart.ModelPartTag);
				if (IsValid(Component))
				{
					FRPGModularModelPartReplacement* PartReplacement = EquippableFeature->GetModel(Character->CharacterAppearance->AppearanceName, Model->ModelTag, ModelPart.ModelPartTag);
					if (PartReplacement)
					{
						Component->SetSkeletalMesh(PartReplacement->SkeletalMesh);
					}
				}
			}
		}

		if (IsValid(CharacterPreview))
		{
			CharacterPreview->UpdatePreviewFromEquipment(InItemInstance);
		}
	}
}

void ARPGCharacterInstance::LoadBaseAppearance(URPGCharacter* InCharacter)
{
	if (IsValid(InCharacter->CharacterAppearance))
	{
		URPGCharacterModelBase* Model = InCharacter->CharacterAppearance->GetModel(URPGFunctionLibrary::GetGameplayTagShortName(CharacterSex));
		if (IsValid(Model))
		{
			Model->ApplyModel(this);
		}
		if (IsValid(InCharacter->CharacterAppearance->AnimationBlueprint))
		{
			GetMesh()->SetAnimInstanceClass(InCharacter->CharacterAppearance->AnimationBlueprint->GeneratedClass);
		}
	}
}

void ARPGCharacterInstance::RefreshAppearance()
{
	if (IsValid(Character) && IsValid(Character->CharacterAppearance))
	{
		const FName ModelTag = URPGFunctionLibrary::GetGameplayTagShortName(CharacterSex);
		URPGCharacterModelBase* Model = Character->CharacterAppearance->GetModel(ModelTag);
		if (IsValid(Model))
		{
			Model->ApplyModel(this);
			if (IsValid(CharacterPreview))
			{
				CharacterPreview->LoadBasePreview(Character->CharacterAppearance, ModelTag);
			}
			for (const FRPGEquippedItemsArrayEntry& Entry : EquippedItems.ItemEntries)
			{
				if (!Entry.bIsAboutToBeRemoved && IsValid(Entry.ItemInstance))
				{
					UpdateAppearanceFromEquipment(Entry.ItemInstance);
				}
			}
		}
	}
}
