// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttributeCollection.generated.h"


USTRUCT(BlueprintType)
struct FRPGAttribute
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BaseValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentValue;
};


UENUM(BlueprintType)
enum class ERPGAttributeValueType : uint8
{
	Base,
	Current
};


UCLASS(BlueprintType)
class DEMORPG_API UAttributeCollection : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_EVENT_ThreeParams(UAttributeCollection, FAttributeChanged, int, float, float)
	FAttributeChanged& OnAttributeChanged() { return AttributeChangedEvent; }
	
	template <typename V, typename K>
	V Get(const K& AttributeName, const ERPGAttributeValueType ValueType, bool& bSuccess) const;
	UFUNCTION(BlueprintPure, DisplayName="Get")
	float Get_BP(const int AttributeName, const ERPGAttributeValueType ValueType, bool& bSuccess) const { return Get<float>(AttributeName, ValueType, bSuccess); }
	
	template <typename V, typename K>
	void Set(const K& AttributeName, const ERPGAttributeValueType ValueType, const V& Value);
	UFUNCTION(BlueprintCallable, DisplayName="Set")
	void Set_BP(const int AttributeName, const ERPGAttributeValueType ValueType, const float Value) { Set<float>(AttributeName, ValueType, Value); }

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSet(const int AttributeName, const ERPGAttributeValueType ValueType, const float Value);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MulticastSet(const int AttributeName, const ERPGAttributeValueType ValueType, const float Value);

	UFUNCTION(BlueprintCallable)
	void Clear() { Attributes.Empty(); }

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerClear();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MulticastClear();
	
	template <typename K>
	bool ClearAttribute(const K& AttributeName) { return Attributes.Remove(AttributeName) > 0; };
	UFUNCTION(BlueprintCallable, DisplayName="Clear")
	bool ClearAttribute_BP(const int AttributeName) { return ClearAttribute(AttributeName); }

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerClearAttribute(const int AttributeName);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void MulticastClearAttribute(const int AttributeName);

	template <typename K>
	bool HasAttribute(const K& AttributeName) const { return Attributes.Find(static_cast<int>(AttributeName)) != nullptr; };
	UFUNCTION(BlueprintCallable, DisplayName="Has Attribute")
	bool HasAttribute_BP(const int AttributeName) const { return HasAttribute(AttributeName); }

	template <typename T>
	TArray<T> GetAttributeNames() const;
	UFUNCTION(BlueprintCallable, DisplayName="Get Attribute Names")
	TArray<int> GetAttributeNames_BP() const { return GetAttributeNames<int>(); }

private:
	UPROPERTY(VisibleAnywhere)
	TMap<int, FRPGAttribute> Attributes;

	FAttributeChanged AttributeChangedEvent;
};

template <typename V, typename K>
V UAttributeCollection::Get(const K& AttributeName, const ERPGAttributeValueType ValueType, bool& bSuccess) const
{
	const FRPGAttribute* Attribute = Attributes.Find(static_cast<int>(AttributeName));
	if (Attribute)
	{
		bSuccess = true;
		switch (ValueType)
		{
			case ERPGAttributeValueType::Base: return Attribute->BaseValue;
			case ERPGAttributeValueType::Current: return Attribute->CurrentValue;
		}
	}
	bSuccess = false;
	return V();
}

template <typename V, typename K>
void UAttributeCollection::Set(const K& AttributeName, const ERPGAttributeValueType ValueType, const V& Value)
{
	const int Key = static_cast<int>(AttributeName);
	FRPGAttribute* Attribute = Attributes.Find(Key);
	if (Attribute)
	{
		switch (ValueType)
		{
			float PreviousValue;
			
		case ERPGAttributeValueType::Base:
			PreviousValue = Attribute->BaseValue;
			Attribute->BaseValue = Value;
			AttributeChangedEvent.Broadcast(Key, PreviousValue, Value);
			break;
		case ERPGAttributeValueType::Current:
			PreviousValue = Attribute->CurrentValue;
			Attribute->CurrentValue = Value;
			AttributeChangedEvent.Broadcast(Key, PreviousValue, Value);
			break;
		}
	}
	else
	{
		Attributes.Add(Key, { static_cast<float>(Value), static_cast<float>(Value) });
	}
}

template <typename T>
TArray<T> UAttributeCollection::GetAttributeNames() const
{
	TArray<T> Names;
	for (const TPair<int, FRPGAttribute>& Element : Attributes)
	{
		Names.Add(static_cast<T>(Element.Key));
	}
	return Names;
}
