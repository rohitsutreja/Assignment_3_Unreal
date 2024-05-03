#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "WallSpline.h"
#include "NetworkReplayStreaming.h"
#include "AWallBuilderController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSendMessageToUI,FString,Data);

UCLASS()
class DYNAMICWALLS_API AWallBuilderController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FSendMessageToUI SendMessageToUIDelegate;
	
	AWallBuilderController() = default;



protected:
	void SetupInputComponent() override;

	void HandleLeftClick(const FInputActionValue& value);
	void HandleRightClick(const FInputActionValue& value);
	void HandleBackSpace(const FInputActionValue& value);
	void HandleDelete(const FInputActionValue& value);

	UFUNCTION(BlueprintCallable)
	void DeleteAllWalls();

	UPROPERTY()
	TArray<AWallSpline*> SplineArray;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* LeftClickAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* RightClickAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* DeleteWallAction;

	UPROPERTY(EditDefaultsOnly)
	UInputAction* DeleteWallSegmentAction;

	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditDefaultsOnly)
	AWallSpline* CurrentSplineActor;

	void BeginPlay() override;

};
