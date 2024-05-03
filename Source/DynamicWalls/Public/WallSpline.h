#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WallSpline.generated.h"


UCLASS()
class DYNAMICWALLS_API AWallSpline : public AActor
{
	GENERATED_BODY()
	
public:	

	AWallSpline();

	void GenerateSpline();
	void DestroyWall();

	void AddSplinePoint(const FVector& Point);
	void RemoveLastSplinePoint();
	void RemoveAllSplinePoints();
	bool HasAnySegment();

private:
	UPROPERTY()
	TArray<UStaticMeshComponent*> SplineMeshes;

	UPROPERTY()
	TArray<FVector> PointsArray;

	UPROPERTY(EditDefaultsOnly)
	class USplineComponent* Spline;

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* SplineSegmentMesh;

};
