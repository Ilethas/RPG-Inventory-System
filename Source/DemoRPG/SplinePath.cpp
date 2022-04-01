// Fill out your copyright notice in the Description page of Project Settings.


#include "SplinePath.h"

#include "DrawDebugHelpers.h"
#include "Components/SplineComponent.h"

// Sets default values
ASplinePath::ASplinePath()
{
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;
	MaxTangentLength = 300.f;
	SegmentScale = FVector(1.f);
	ForwardAxis = ESplineMeshAxis::X;
}

// Called when the game starts or when spawned
void ASplinePath::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASplinePath::UpdateSplineMesh()
{
	MeshSegments.RemoveAll([](USplineMeshComponent* Segment)
	{
		return !IsValid(Segment);
	});

	if (IsValid(Mesh))
	{
		if (ForwardAxis == ESplineMeshAxis::X)
		{
			SegmentLength = (Mesh->GetBoundingBox().Max.X - Mesh->GetBoundingBox().Min.X) * SegmentScale.X;
		}
		else if (ForwardAxis == ESplineMeshAxis::Y)
		{
			SegmentLength = (Mesh->GetBoundingBox().Max.Y - Mesh->GetBoundingBox().Min.Y) * SegmentScale.Y;
		}
		else if (ForwardAxis == ESplineMeshAxis::Z)
		{
			SegmentLength = (Mesh->GetBoundingBox().Max.Z - Mesh->GetBoundingBox().Min.Z) * SegmentScale.Z;
		}
	}
	else
	{
		return;
	}
	
	// Adjust the amount of mesh segments accordingly
	const int NumSegments = FMath::Max(static_cast<int>(Spline->GetSplineLength() / SegmentLength), 1);
	if (MeshSegments.Num() < NumSegments)
	{
		for (int i = 0; i < NumSegments; i++)
		{
			USplineMeshComponent *Segment = NewObject<USplineMeshComponent>(this);
			MeshSegments.Add(Segment);
			Segment->RegisterComponent();
			Segment->SetMobility(EComponentMobility::Movable);
		}
	}
	else if (MeshSegments.Num() > NumSegments)
	{
		for (int i = NumSegments; i < MeshSegments.Num(); i++)
		{
			MeshSegments[i]->DestroyComponent();
		}
		MeshSegments.RemoveAt(NumSegments, MeshSegments.Num() - NumSegments);
	}

	// Update each segment to follow the spline
	for (int i = 0; i < MeshSegments.Num(); i++)
	{
		const float Length = Spline->GetSplineLength() / NumSegments;
		MeshSegments[i]->SetStartPosition(Spline->GetLocationAtDistanceAlongSpline(i * Length, ESplineCoordinateSpace::World));
		MeshSegments[i]->SetStartTangent(Spline->GetTangentAtDistanceAlongSpline(i * Length, ESplineCoordinateSpace::World).GetClampedToMaxSize(MaxTangentLength));
		MeshSegments[i]->SetEndPosition(Spline->GetLocationAtDistanceAlongSpline((i + 1) * Length, ESplineCoordinateSpace::World));
		MeshSegments[i]->SetEndTangent(Spline->GetTangentAtDistanceAlongSpline((i + 1) * Length, ESplineCoordinateSpace::World).GetClampedToMaxSize(MaxTangentLength));
		MeshSegments[i]->SetStaticMesh(Mesh);
		MeshSegments[i]->SetForwardAxis(ForwardAxis);
		MeshSegments[i]->CastShadow = false;
	
		const FVector2D XYScale = FVector2D(SegmentScale.X, SegmentScale.Y);
		MeshSegments[i]->SetStartScale(XYScale);
		MeshSegments[i]->SetEndScale(XYScale);
	}
}

