// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGNetworkedObject.h"

AActor* URPGNetworkedObject::GetOwningActor() const
{
	return GetTypedOuter<AActor>();
}

ENetRole URPGNetworkedObject::GetOwnerRole() const
{
	AActor* MyOwner = GetOwningActor();
	return (MyOwner ? MyOwner->GetLocalRole() : ROLE_None);
}

bool URPGNetworkedObject::HasAuthority() const
{
	return (GetOwnerRole() == ROLE_Authority);
}

UWorld* URPGNetworkedObject::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}
	return IsValid(GetOuter()) ? GetOuter()->GetWorld() : nullptr;
}

bool URPGNetworkedObject::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return false;
}

void URPGNetworkedObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Add any Blueprint properties
	// This is not required if you do not want the class to be "Blueprintable"
	if (const UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass()))
	{
		BPClass->GetLifetimeBlueprintReplicationList(OutLifetimeProps);
	}
}

bool URPGNetworkedObject::IsSupportedForNetworking() const
{
	return true;
}

int32 URPGNetworkedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if ((Function->FunctionFlags & FUNC_Static))
	{
		// Try to use the same logic as function libraries for static functions, will try to use the global context to check authority only/cosmetic
		return GEngine->GetGlobalFunctionCallspace(Function, this, Stack);
	}

	AActor* MyOwner = GetOwningActor();
	return (MyOwner ? MyOwner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local);
}

bool URPGNetworkedObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParameters, FFrame* Stack)
{
	bool bProcessed = false;
	if (AActor* MyOwner = GetOwningActor())
	{
		FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
		if (Context != nullptr)
		{
			for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
			{
				if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(MyOwner, Function))
				{
					Driver.NetDriver->ProcessRemoteFunction(MyOwner, Function, Parameters, OutParameters, Stack, this);
					bProcessed = true;
				}
			}
		}
	}
	return bProcessed;
}
