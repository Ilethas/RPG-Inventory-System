#include "RPGCharacterPreview.h"

#include "RPGCharacterAppearance.h"
#include "Components/SceneCaptureComponent2D.h"
#include "DemoRPG/RPGFunctionLibrary.h"
#include "DemoRPG/RPGGameplayTags.h"
#include "DemoRPG/Items/RPGItem.h"
#include "DemoRPG/Items/RPGItemInstance.h"
#include "DemoRPG/Items/Features/RPGEquippableFeature.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetRenderingLibrary.h"

ARPGCharacterPreview::ARPGCharacterPreview()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpringArm->SetRelativeRotation(FRotator(350.0f, 240.0f, 0.0f));
	SpringArm->TargetArmLength = 225.0f;
	SpringArm->bDoCollisionTest = false;
	
	SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	SceneCaptureComponent->SetupAttachment(SpringArm);
	SceneCaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	SceneCaptureComponent->CaptureSource = SCS_FinalToneCurveHDR;
	SceneCaptureComponent->ShowFlags.SetAtmosphere(false);
	SceneCaptureComponent->ShowFlags.SetSkyLighting(false);
	SceneCaptureComponent->ShowFlags.SetEyeAdaptation(false);
	SceneCaptureComponent->bConsiderUnrenderedOpaquePixelAsFullyTranslucent = true;
	SceneCaptureComponent->FOVAngle = 50.0f;
}

void ARPGCharacterPreview::LoadBasePreview(URPGCharacterAppearance* InCharacterAppearance, const FName InModelTag)
{
	CharacterAppearance = InCharacterAppearance;
	ModelTag = InModelTag;
	
	URPGCharacterModelBase* Model = CharacterAppearance->GetModel(InModelTag);
	if (IsValid(Model))
	{
		Model->ApplyModel(this);
	}
	if (IsValid(InCharacterAppearance->AnimationBlueprint))
	{
		USkeletalMeshComponent* BodyMesh =  URPGFunctionLibrary::GetComponentByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Body);
		if (IsValid(BodyMesh))
		{
			BodyMesh->SetAnimInstanceClass(InCharacterAppearance->AnimationBlueprint->GeneratedClass);
		}
	}
}

void ARPGCharacterPreview::UpdatePreviewFromEquipment(ARPGItemInstance* InItemInstance)
{
	check(InItemInstance->GetItem());
	URPGEquippableFeature* EquippableFeature = InItemInstance->GetItem()->GetFeature<URPGEquippableFeature>();
	if (!IsValid(EquippableFeature))
	{
		return;
	}

	if (IsValid(CharacterAppearance))
	{
		URPGModularCharacterModel* Model = Cast<URPGModularCharacterModel>(CharacterAppearance->GetModel(ModelTag));
		if (IsValid(Model))
		{
			TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
			for (const FRPGModularModelPart& ModelPart : Model->ModelParts)
			{
				USkeletalMeshComponent* Component = URPGFunctionLibrary::GetComponentByTag<USkeletalMeshComponent>(MeshComponents, ModelPart.ModelPartTag);
				if (IsValid(Component))
				{
					FRPGModularModelPartReplacement* PartReplacement = EquippableFeature->GetModel(CharacterAppearance->AppearanceName, Model->ModelTag, ModelPart.ModelPartTag);
					if (PartReplacement)
					{
						Component->SetSkeletalMesh(PartReplacement->SkeletalMesh);
					}
				}
			}
		}
	}
}

void ARPGCharacterPreview::ApplyModel(URPGCharacterModel* Model)
{
	TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
	for (USkeletalMeshComponent* Component : MeshComponents)
	{
		if (!Component->ComponentHasTag(URPGCharacterAppearanceTags::Body))
		{
			Component->DestroyComponent();
		}
	}
	USkeletalMeshComponent* BodyComponent = URPGFunctionLibrary::GetComponentByTag<USkeletalMeshComponent>(MeshComponents, URPGCharacterAppearanceTags::Body);
	if (!IsValid(BodyComponent))
	{
		BodyComponent = NewObject<USkeletalMeshComponent>(this);
		BodyComponent->ComponentTags.Append({ URPGCharacterAppearanceTags::Appearance, URPGCharacterAppearanceTags::Body });
		BodyComponent->RegisterComponent();
		AddInstanceComponent(BodyComponent);
		BodyComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	BodyComponent->SetSkeletalMesh(Model->SkeletalMesh);
	BodyComponent->SetForcedLOD(1);
	BodyComponent->bForceMipStreaming = true;
	BodyComponent->SetBoundsScale(2.0f);
	BodyComponent->SetVisibleInSceneCaptureOnly(true);
	BodyComponent->SetLightingChannels(false, true, false);
	if (IsValid(CharacterAppearance->AnimationBlueprint))
	{
		BodyComponent->SetAnimInstanceClass(CharacterAppearance->AnimationBlueprint->GeneratedClass);
	}
}

void ARPGCharacterPreview::ApplyModel(URPGModularCharacterModel* Model)
{
	USkeletalMeshComponent* BodyMesh = nullptr;
	TArray<USkeletalMeshComponent*> MeshComponents = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);

	// Spawn all mesh components for each available part. Remove unnecessary components
	for (const FRPGModularModelPart& ModelPart : Model->ModelParts)
	{
		USkeletalMeshComponent* Component = URPGFunctionLibrary::GetComponentByTag<USkeletalMeshComponent>(MeshComponents, ModelPart.ModelPartTag);
		if (!IsValid(Component))
		{
			Component = NewObject<USkeletalMeshComponent>(this, ModelPart.ModelPartTag);
			Component->ComponentTags.Append({ URPGCharacterAppearanceTags::Appearance, ModelPart.ModelPartTag });
			Component->RegisterComponent();
			AddInstanceComponent(Component);
		}
		if (ModelPart.ModelPartTag == URPGCharacterAppearanceTags::Body)
		{
			BodyMesh = Component;
		}
		Component->SetSkeletalMesh(ModelPart.SkeletalMesh);
		Component->SetForcedLOD(1);
		Component->bForceMipStreaming = true;
		Component->SetBoundsScale(2.0f);
		Component->SetVisibleInSceneCaptureOnly(true);
		Component->SetLightingChannels(false, true, false);
		MeshComponents.Remove(Component);
	}
	
	// Setup animations and attach meshes
	if (IsValid(BodyMesh))
	{
		BodyMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		TArray<USkeletalMeshComponent*> Components = URPGFunctionLibrary::GetComponentsByTag<USkeletalMeshComponent>(this, URPGCharacterAppearanceTags::Appearance);
		for (USkeletalMeshComponent* Component : Components)
		{
			if (Component != BodyMesh)
			{
				Component->AttachToComponent(BodyMesh, FAttachmentTransformRules::KeepRelativeTransform);
				Component->SetMasterPoseComponent(BodyMesh);
			}
		}
	}

	// Remove other unnecessary meshes
	for (USkeletalMeshComponent* Component : MeshComponents)
	{
		Component->DestroyComponent();
	}
}

void ARPGCharacterPreview::BeginPlay()
{
	Super::BeginPlay();

	SceneCaptureComponent->ShowOnlyActors.Add(this);
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, ETextureRenderTargetFormat::RTF_RGBA16f, FLinearColor::Transparent);
	if (IsValid(CharacterPreviewMaterial))
	{
		CharacterPreviewMaterialInstance = UMaterialInstanceDynamic::Create(CharacterPreviewMaterial, this);
		CharacterPreviewMaterialInstance->SetTextureParameterValue("RenderTarget", RenderTarget);
		SceneCaptureComponent->TextureTarget = RenderTarget;
	}
}
