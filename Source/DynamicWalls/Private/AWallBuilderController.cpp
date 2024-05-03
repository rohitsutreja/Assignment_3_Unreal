#include "AWallBuilderController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AWallBuilderController::BeginPlay()
{
	Super::BeginPlay();
	bShowMouseCursor = true;

	// To have starting wall actor by default.
	CurrentSplineActor = NewObject<AWallSpline>(this);
	SplineArray.Add(CurrentSplineActor);
}

void AWallBuilderController::SetupInputComponent()
{
	Super::SetupInputComponent();

	LeftClickAction = NewObject<UInputAction>(this);
	LeftClickAction->ValueType = EInputActionValueType::Boolean;

	RightClickAction = NewObject<UInputAction>(this);
	RightClickAction->ValueType = EInputActionValueType::Boolean;

	DeleteWallSegmentAction = NewObject<UInputAction>(this);
	DeleteWallSegmentAction->ValueType = EInputActionValueType::Boolean;

	DeleteWallAction = NewObject<UInputAction>(this);
	DeleteWallAction->ValueType = EInputActionValueType::Boolean;


	MappingContext = NewObject<UInputMappingContext>(this);
	
	MappingContext->MapKey(LeftClickAction, EKeys::LeftMouseButton);
	MappingContext->MapKey(RightClickAction, EKeys::RightMouseButton);
	MappingContext->MapKey(DeleteWallAction, EKeys::Delete);
	MappingContext->MapKey(DeleteWallSegmentAction, EKeys::BackSpace);
	

	if (auto EIC = Cast<UEnhancedInputComponent>(InputComponent)) {
		if (LeftClickAction) {
			EIC->BindAction(LeftClickAction, ETriggerEvent::Completed,this, &AWallBuilderController::HandleLeftClick);
		}

		if (RightClickAction) {
			EIC->BindAction(RightClickAction, ETriggerEvent::Completed, this, &AWallBuilderController::HandleRightClick);
		}

		if (DeleteWallSegmentAction) {
			EIC->BindAction(DeleteWallSegmentAction, ETriggerEvent::Completed, this, &AWallBuilderController::HandleBackSpace);
		}

		if (DeleteWallAction) {
			EIC->BindAction(DeleteWallAction, ETriggerEvent::Completed, this, &AWallBuilderController::HandleDelete);
		}

	}

	if (auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		InputSubsystem->AddMappingContext(MappingContext, 0);
	}

}


//This will add spline point to spline points array of current spline actor(Current Wall)
void AWallBuilderController::HandleLeftClick(const FInputActionValue& value) {
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_WorldDynamic, true, HitResult);

	if (HitResult.bBlockingHit)
	{
		if (CurrentSplineActor) {
			CurrentSplineActor->AddSplinePoint(HitResult.Location);
			CurrentSplineActor->GenerateSpline();
		}
	}
}


//This will start a new wall by creating new spline actor with its own spline point array. It will only start new wall if there is at least one wall segment(Two Spline Points) in current wall(Spline Actor)
void AWallBuilderController::HandleRightClick(const FInputActionValue& value) {
	if (CurrentSplineActor) {
		if (CurrentSplineActor->HasAnySegment()) {

			//Will broadcast the message so UI widget BP can display notification.
			SendMessageToUIDelegate.Broadcast("Wall Construction Completed");
			CurrentSplineActor = NewObject<AWallSpline>(this);
			SplineArray.Add(CurrentSplineActor);
		}
	}

}


//This will delete current spline wall(Spline Actor) and move control back to previous spline wall, so we can continue drawing in previous wall where we left.
void AWallBuilderController::HandleDelete(const FInputActionValue& value) {
	if (CurrentSplineActor) {

		CurrentSplineActor->DestroyWall();
		CurrentSplineActor->RemoveAllSplinePoints();

		//Remove current one from array and destroy it, then set previous one to current spline actor, except its the last one, becuase we want one default spline actor.
		if (!SplineArray.IsEmpty()) {
			if (SplineArray.Num() > 1)
			{	
				SplineArray.Pop();
				CurrentSplineActor->Destroy();
				CurrentSplineActor = SplineArray[SplineArray.Num() - 1];

				//Will broadcast the message so UI widget BP can display notification.
				SendMessageToUIDelegate.Broadcast("Current Wall Deleted");
			}	
		}
	}
}

//This will destroy all the spline actors, and set current actor(Spline wall) to default new.
void AWallBuilderController::DeleteAllWalls()
{


	if (!SplineArray.IsEmpty()) {

		for (int i{}; i < SplineArray.Num(); i++) {
			if (SplineArray[i]) {
				SplineArray[i]->DestroyWall();
				SplineArray[i]->Destroy();
				SplineArray[i] = nullptr;
			}
		}

		SplineArray.Empty();

		//Will broadcast the message so UI widget BP can display notification.
		SendMessageToUIDelegate.Broadcast("All Walls Deleted");

		CurrentSplineActor = nullptr;

		//To create default new current spline actor, because we deleted all.
		CurrentSplineActor = NewObject<AWallSpline>(this);

		SplineArray.Add(CurrentSplineActor);
	}
}


//This will delete last spline point and hence delete Last wall ssegment created in current spline wall(Spline Actor). It Works as an undo for current wall.
void AWallBuilderController::HandleBackSpace(const FInputActionValue& value) {
	CurrentSplineActor->RemoveLastSplinePoint();
	CurrentSplineActor->GenerateSpline();
}


