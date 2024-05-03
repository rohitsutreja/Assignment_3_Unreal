#include "WallSpline.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "GameFramework/Actor.h"

AWallSpline::AWallSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = Spline;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WallSegmentMesh(TEXT("/Script/Engine.StaticMesh'/Game/_GENERATED/ZTI/Wall_Segment.Wall_Segment'"));

	if (WallSegmentMesh.Succeeded()) {
		SplineSegmentMesh = WallSegmentMesh.Object;
	}
}


//Helper functions to manipulate Points array from Player Controller.
void AWallSpline::AddSplinePoint(const FVector& Point)
{
	if (!PointsArray.IsEmpty()) {
		if (PointsArray[PointsArray.Num() - 1] != Point) {
			PointsArray.Add(Point);
		}
	}
	else {
		PointsArray.Add(Point);
	}
}
void AWallSpline::RemoveLastSplinePoint()
{
	if (!PointsArray.IsEmpty())
	{
		PointsArray.Pop();
	}
}
void AWallSpline::RemoveAllSplinePoints()
{
	PointsArray.Empty();
}

//Used to check if at least two spline points and therefore at least one visible wall component is there.
bool AWallSpline::HasAnySegment() {
	if (PointsArray.Num() >= 2) {
		return true;
	}
	else {
		return false;
	}
}


//Destroy the old Spline Components and Generate new one from Points Array.
void AWallSpline::GenerateSpline()
{
	DestroyWall();

	Spline->SetSplinePoints(PointsArray, ESplineCoordinateSpace::World);

	int NumberOfSplinePoints = Spline->GetNumberOfSplinePoints();

	for (int i = 0; i < NumberOfSplinePoints - 1; i++) {

		//So we can get an angle rather than smooth curve.
		Spline->SetSplinePointType(i,ESplinePointType::Linear);

		FVector StartLocation, StartTangent, EndLocation, EndTangent;
		Spline->GetLocationAndTangentAtSplinePoint(i, StartLocation, StartTangent, ESplineCoordinateSpace::World);
		Spline->GetLocationAndTangentAtSplinePoint(i + 1, EndLocation, EndTangent, ESplineCoordinateSpace::World);

		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
		SplineMeshComponent->RegisterComponent();
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->SetStaticMesh(SplineSegmentMesh);
		SplineMeshComponent->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		SplineMeshComponent->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);

		SplineMeshes.Add(SplineMeshComponent);
	}
}



//To Destroy all the spline wall components.
void AWallSpline::DestroyWall()
{
	for (int i = 0; i < SplineMeshes.Num(); i++) {
		if (SplineMeshes[i]) {
			SplineMeshes[i]->DestroyComponent();
		}
	}
	SplineMeshes.Empty();																													
}

