// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGTypes.h"
#include "AsyncActions/RotateCharacter.h"
#include "Engine/ActorChannel.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGFunctionLibrary.generated.h"

struct FGameplayTag;
UCLASS()
class DEMORPG_API URPGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Get an FString representation of an Enum value.
	 *
	 * @param	EnumValue	Concrete Enum value (for example EWeaponTypes::Slashing)
	 *
	 * @return	FString with a name of the passed Enum value. For EWeaponTypes::Slashing, the returned value will be Slashing.
	 */
	template <typename T>
	static FString EnumToString(const FString& EnumName, T EnumValue);

	/**
	 * Get an Enum value based on FString.
	 *
	 * @param	EnumName	Name of the Enum type (for example EWeaponTypes)
	 * @param	ValueName	Name of the Enum value (for example Slashing)
	 * @param	bOutSuccess	An out parameter that indicates if an enum value corresponding to the string has been found.
	 *
	 * @return	Enum value corresponding to the EnumName and ValueName strings.
	 */
	template <typename T>
	static T StringToEnum(const FString& EnumName, const FString& ValueName, bool& bOutSuccess);

	UFUNCTION(BlueprintPure)
	static bool IsWithEditor();

	UFUNCTION(BlueprintCallable)
	static bool IsDesignTime(class UWidget* Widget);

	UFUNCTION(BlueprintCallable)
	static UUserWidget* GetParentUserWidget(const class UUserWidget* Widget);

	UFUNCTION(BlueprintCallable)
	static bool IsChild(const class UUserWidget* Descendant, const class UUserWidget* Ancestor);

	UFUNCTION(BlueprintPure)
	static FVector2D GetWidgetScreenSize(UObject* WorldContextObject, const UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = Items)
	static void MoveItem(class ARPGItemInstance* ItemInstance, const bool bForceNewPosition, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(BlueprintCallable, Category = Items)
	static void SplitItem(class ARPGItemInstance* ItemInstance, const int AmountToSplit, class URPGInventory* TargetInventory, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);

	UFUNCTION(BlueprintCallable, Category = Items)
	static void ShowEquippedItemInUI(class ARPGItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = Items)
	static void HideEquippedItemInUI(class ARPGItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = Items)
	static void ShowItemInInventory(class ARPGItemInstance* ItemInstance, class URPGInventoryMappingContainer* TargetContainer, const FRPGUISlot& TargetSlot);
	
	UFUNCTION(BlueprintCallable, Category = Items)
	static void HideItemInInventory(class ARPGItemInstance* ItemInstance, class URPGInventoryMappingContainer* TargetContainer);

	UFUNCTION(BlueprintCallable, Category = UI)
	static void SetWidgetLayerPosition(class UWidget* Widget, const FVector2D Position);

	template <typename ComponentType>
	static ComponentType* GetComponentByTag(AActor* Actor, const FName Tag);

	template <typename ComponentType>
	static ComponentType* GetComponentByTag(TArray<ComponentType*>& Components, const FName Tag);

	template <typename ComponentType>
	static TArray<ComponentType*> GetComponentsByTag(AActor* Actor, const FName Tag);

	template <typename ComponentType>
	static TArray<ComponentType*> GetComponentsByTag(TArray<ComponentType*>& Components, const FName Tag);

	static FName GetGameplayTagShortName(const FGameplayTag &InTag);
};


template <typename T>
FString URPGFunctionLibrary::EnumToString(const FString& EnumName, T EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (!EnumPtr)
	{
		return FString("Invalid");
	}
	return EnumPtr->GetNameStringByIndex(static_cast<int32>(EnumValue));
}


template <typename T>
T URPGFunctionLibrary::StringToEnum(const FString& EnumName, const FString& ValueName, bool& bOutSuccess)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (EnumPtr)
	{
		int32 Index = EnumPtr->GetIndexByNameString(ValueName, EGetByNameFlags::ErrorIfNotFound);
		if (Index != INDEX_NONE)
		{
			bOutSuccess = true;
			return static_cast<T>(EnumPtr->GetValueByIndex(Index));
		}
	}
	bOutSuccess = false;
	return T(0);
}

template <typename ComponentType>
ComponentType* URPGFunctionLibrary::GetComponentByTag(AActor* Actor, const FName Tag)
{
	TArray<UActorComponent*> Components = Actor->GetComponentsByTag(ComponentType::StaticClass(), Tag);
	if (Components.Num() > 0)
	{
		return Cast<ComponentType>(Components[0]);
	}
	return nullptr;
}

template <typename ComponentType>
ComponentType* URPGFunctionLibrary::GetComponentByTag(TArray<ComponentType*>& Components, const FName Tag)
{
	for (ComponentType* Component : Components)
	{
		if (Component->ComponentHasTag(Tag))
		{
			return Component;
		}
	}
	return nullptr;
}

template <typename ComponentType>
TArray<ComponentType*> URPGFunctionLibrary::GetComponentsByTag(AActor* Actor, const FName Tag)
{
	TArray<ComponentType*> ComponentsByTag;
	ComponentsByTag.Reserve(Actor->GetComponents().Num());
	for (UActorComponent* Component : Actor->GetComponents())
	{
		ComponentType* CastedComponent = Cast<ComponentType>(Component);
		if (IsValid(CastedComponent) && CastedComponent->ComponentHasTag(Tag))
		{
			ComponentsByTag.Push(CastedComponent);
		}
	}
	return MoveTemp(ComponentsByTag);
}

template <typename ComponentType>
TArray<ComponentType*> URPGFunctionLibrary::GetComponentsByTag(TArray<ComponentType*>& Components, const FName Tag)
{
	TArray<ComponentType*> ComponentsByTag;
	ComponentsByTag.Reserve(Components.Num());
	for (ComponentType* Component : Components)
	{
		if (Component->ComponentHasTag(Tag))
		{
			ComponentsByTag.Push(Component);
		}
	}
	return MoveTemp(ComponentsByTag);
}
