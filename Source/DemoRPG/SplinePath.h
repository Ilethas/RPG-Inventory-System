// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"
#include "SplinePath.generated.h"

UCLASS()
class DEMORPG_API ASplinePath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplinePath();

	UFUNCTION(BlueprintCallable, Category = Spline)
	void UpdateSplineMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	class USplineComponent* Spline;
	
	UPROPERTY(VisibleAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<USplineMeshComponent*> MeshSegments;

	UPROPERTY(EditAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ESplineMeshAxis::Type> ForwardAxis;

	UPROPERTY(EditAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float MaxTangentLength;

	UPROPERTY(EditAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FVector SegmentScale;

	UPROPERTY(VisibleAnywhere, Category = Spline, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float SegmentLength = 100.f;
};
