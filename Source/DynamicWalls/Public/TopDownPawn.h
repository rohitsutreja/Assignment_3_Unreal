#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputModifiers.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/SphereComponent.h"
#include "TopDownPawn.generated.h"


UCLASS()
class DYNAMICWALLS_API ATopDownPawn : public APawn
{
	GENERATED_BODY()

public:	
	ATopDownPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleMove(const FInputActionValue& value);
	void HandleZoom(const FInputActionValue& value);

private:

	UPROPERTY()
	USceneComponent* SceneRoot;

	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY()
	UFloatingPawnMovement* Movement;

	UPROPERTY()
	UInputAction* MoveAction;

	UPROPERTY()
	UInputAction* ZoomAction;

	UPROPERTY()
	UInputMappingContext* MappingContext;


	bool IsOnGround = false;
};
