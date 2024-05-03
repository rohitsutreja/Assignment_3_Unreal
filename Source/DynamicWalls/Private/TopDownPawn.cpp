#include "TopDownPawn.h"
#include "EnhancedInputSubsystems.h"
#include "InputModifiers.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "math.h"

ATopDownPawn::ATopDownPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneRoot;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, "SpringEndpoint");

	//To get movement functionality in our pawn.
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}

void ATopDownPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	MappingContext = NewObject<UInputMappingContext>(this);

	MoveAction = NewObject<UInputAction>(this);
	MoveAction->ValueType = EInputActionValueType::Axis2D;
	
	UInputModifierNegate* NegateModifire = NewObject<UInputModifierNegate>(this);
	UInputModifierSwizzleAxis* SwizzleAxisModifire = NewObject<UInputModifierSwizzleAxis>(this);

	auto& WKeyMapping = MappingContext->MapKey(MoveAction, EKeys::W);
	WKeyMapping.Modifiers.Add(SwizzleAxisModifire);

	auto& SKeyMapping = MappingContext->MapKey(MoveAction, EKeys::S);
	SKeyMapping.Modifiers.Add(SwizzleAxisModifire);
	SKeyMapping.Modifiers.Add(NegateModifire);
	
	auto& AKeyMapping = MappingContext->MapKey(MoveAction, EKeys::A);
	AKeyMapping.Modifiers.Add(NegateModifire);

	auto& DKeyMapping = MappingContext->MapKey(MoveAction, EKeys::D);


	ZoomAction = NewObject<UInputAction>(this);
	ZoomAction->ValueType = EInputActionValueType::Axis1D;
	auto& MouseWheelMapping = MappingContext->MapKey(ZoomAction, EKeys::MouseWheelAxis);


	if (auto EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (MoveAction) {
			EIC->BindAction(MoveAction,ETriggerEvent::Triggered,this,&ATopDownPawn::HandleMove);
		}
		if (ZoomAction) {
			EIC->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATopDownPawn::HandleZoom);
		}
	}


	if (auto MPC = Cast<APlayerController>(GetController())) {
		if (auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(MPC->GetLocalPlayer())) {
			InputSubsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void ATopDownPawn::HandleMove(const FInputActionValue& value) {
	FVector2D movement = value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(),movement.Y);
	AddMovementInput(GetActorRightVector(),movement.X);
}

void ATopDownPawn::HandleZoom(const FInputActionValue& value) {
	auto val = value.Get<float>();

	auto OldOffset = SpringArm->TargetOffset;
	auto ArmLength = SpringArm->TargetArmLength;

	if (!IsOnGround) {
		auto NewOffset = OldOffset - FVector{ 0,0,val * 30 };
		NewOffset.Z = FMath::Clamp(NewOffset.Z, 50, 2500);

		auto DeltaAngle = asin((val * 30) / ArmLength) * 57.2958;

		SpringArm->TargetOffset = NewOffset;

		if (NewOffset.Z <= 700) {
			auto NewRotation = Camera->GetRelativeRotation() + FRotator(DeltaAngle, 0, 0);
			NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 0.f);
			Camera->SetRelativeRotation(NewRotation);


			if (NewOffset.Z == 50) {
				IsOnGround = true;
			}
		}
	} 
	else { // In case we hit the ground, will change arm length only.
		auto NewArmLength = ArmLength - val * 30;

		NewArmLength = FMath::Clamp(NewArmLength, 10, 500);
		SpringArm->TargetArmLength = NewArmLength;

		if (NewArmLength == 500) {
			SpringArm->TargetOffset = { 0,0,51 };
			IsOnGround = false;
		}
	}
}