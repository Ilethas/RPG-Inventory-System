// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeCollection.h"

void UAttributeCollection::ServerSet_Implementation(const int AttributeName, const ERPGAttributeValueType ValueType, const float Value)
{
	MulticastSet(AttributeName, ValueType, Value);
}

void UAttributeCollection::MulticastSet_Implementation(const int AttributeName, const ERPGAttributeValueType ValueType, const float Value)
{
	Set(AttributeName, ValueType, Value);
}

void UAttributeCollection::ServerClear_Implementation()
{
	MulticastClear();
}

void UAttributeCollection::MulticastClear_Implementation()
{
	Clear();
}

void UAttributeCollection::ServerClearAttribute_Implementation(const int AttributeName)
{
	MulticastClearAttribute(AttributeName);
}

void UAttributeCollection::MulticastClearAttribute_Implementation(const int AttributeName)
{
	ClearAttribute(AttributeName);
}
