// Copyright (c) 2025 Brian Pimentel


#include "WallRunController.h"

#include "EnhancedInputComponent.h"
#include "WallRunningLog.h"
#include "WallRunningTags.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UWallRunController::UWallRunController()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}


// Called when the game starts
void UWallRunController::BeginPlay()
{
	Super::BeginPlay();

	SetupWallRunning();
}

// Called every frame
void UWallRunController::TickComponent(float DeltaTime, ELevelTick TickType,
									   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsWallRunning)
	{
		if (MovementComponent->MovementMode != MOVE_Falling || !WallFound())
		{
			CancelWallRun();
		}
		else
		{
			PlayerCharacter->AddMovementInput(WallRunDirection, 1.0f);

			if (!WallRunNormal.IsNearlyZero())
			{
				MovementComponent->AddForce(-WallRunNormal * 60000.0f);
			}

			FVector CurrentVelocity = MovementComponent->Velocity;
			float Speed2D = FVector(CurrentVelocity.X, CurrentVelocity.Y, 0.0f).Size();

			if (Speed2D > Speed && Speed2D > KINDA_SMALL_NUMBER)
			{
				FVector Clamped2D = FVector(CurrentVelocity.X, CurrentVelocity.Y, 0.0f).GetSafeNormal() * Speed;
				CurrentVelocity.X = Clamped2D.X;
				CurrentVelocity.Y = Clamped2D.Y;
				MovementComponent->Velocity = CurrentVelocity;
			}
		}
	}
}

void UWallRunController::SetupWallRunning()
{
	PlayerCharacter = Cast<ACharacter>(GetOwner());
	checkf(PlayerCharacter, TEXT("Unable to get reference to the Player's Character"));

	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	checkf(PlayerController, TEXT("Unable to get reference to the Player's PlayerController"));

	MovementComponent = PlayerCharacter->GetCharacterMovement();
	checkf(MovementComponent, TEXT("Unable to get reference to the Player Character's CharacterMovementComponent"));

	MovementComponent->AddTickPrerequisiteComponent(this);

	ColliderComponent = PlayerCharacter->GetCapsuleComponent();
	checkf(ColliderComponent, TEXT("Unable to get reference to the Player Character's CapsuleComponent"));
	
	ColliderComponent->OnComponentHit.AddDynamic(
		this,
		&UWallRunController::OnWallHit);

	EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	checkf(EnhancedInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent"));

	if (ActionJump)
	{
		EnhancedInputComponent->BindAction(ActionJump, ETriggerEvent::Triggered, this, &UWallRunController::JumpDuringWallRun);
	}

	WallCollisionQueryParams.AddIgnoredActor(PlayerCharacter);

	DistanceToWallDuringRun = PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

void UWallRunController::StartWallRun(FVector ImpactPosition, FVector ImpactNormal)
{
	//Direction perpendicular to the normal of the wall crossing the Z axis.
	FVector WallTangent = FVector::CrossProduct(FVector::UpVector, ImpactNormal).GetSafeNormal();
	//PlayerCharacter approach vector.
	FVector CharacterDirection = MovementComponent->Velocity.GetSafeNormal();

	//Dot product of the Character relative to the wall tangent provides us with the data necessary to determine
	//where they're approaching from.
	float SideSign = FVector::DotProduct(CharacterDirection, WallTangent);

	UE_LOG(WallRunningLog, Warning, TEXT("Wall Run Dot Product: %f"), SideSign);

	//If negative, Player Character is approaching from the left.
	if (SideSign < 0.0f)
	{
		WallTangent *= -1.0f;
	}

	WallIsOnTheRight = SideSign >= 0.0f;

	//Caching WallTangent so we know which direction to move in during Wall Running.
	WallRunDirection = FVector(WallTangent.X, WallTangent.Y, 0.0f).GetSafeNormal();
	WallRunNormal = FVector(ImpactNormal.X, ImpactNormal.Y, 0.0f).GetSafeNormal();
	
	FVector ModifiedImpactPosition = FVector(ImpactPosition.X, ImpactPosition.Y, PlayerCharacter->GetActorLocation().Z);
	FVector TargetLocation = ModifiedImpactPosition + ImpactNormal * DistanceToWallDuringRun;
	FRotator TargetRotation = FRotationMatrix::MakeFromXZ(WallTangent, FVector::UpVector).Rotator();
	
	PreviousGravityScale = MovementComponent->GravityScale;
	PreviousAirControl = MovementComponent->AirControl;
	PreviousMaxAcceleration = MovementComponent->MaxAcceleration;
	PreviousBrakingDecelerationFalling = MovementComponent->BrakingDecelerationFalling;
	PreviousBrakingFrictionFactor = MovementComponent->BrakingFrictionFactor;
	
	MovementComponent->StopMovementImmediately();
	PlayerController->SetIgnoreMoveInput(true);

	MovementComponent->GravityScale = EnableGravityAfter > 0.0f ? 0.0f : GravityScale;
	MovementComponent->AirControl = 1.0f;
	MovementComponent->MaxAcceleration = 20000.0f;
	MovementComponent->BrakingDecelerationFalling = 0.0f;
	MovementComponent->BrakingFrictionFactor = 0.0f;
	
	PlayerCharacter->SetActorLocationAndRotation(
	TargetLocation,
	TargetRotation,
	false,
	nullptr,
	ETeleportType::TeleportPhysics);

	FVector V = MovementComponent->Velocity;
	V.X = WallRunDirection.X * Speed;
	V.Y = WallRunDirection.Y * Speed;
	MovementComponent->Velocity = V;

	if (Duration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(EndWallRunTimerHandle, this, &UWallRunController::CancelWallRun,
											Duration, false);
	}

	if (EnableGravityAfter > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(EnableGravityTimerHandle, this, &UWallRunController::EnableGravity,
											EnableGravityAfter, false);
	}

	IsWallRunning = true;
}

void UWallRunController::CancelWallRun()
{
	if (!IsWallRunning)
	{
		return;
	}

	if (Duration > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(EndWallRunTimerHandle);
	}

	IsWallRunning = false;

	PlayerController->SetIgnoreMoveInput(false);

	MovementComponent->GravityScale = PreviousGravityScale;
	MovementComponent->AirControl = PreviousAirControl;
	MovementComponent->MaxAcceleration = PreviousMaxAcceleration;
	MovementComponent->BrakingDecelerationFalling = PreviousBrakingDecelerationFalling;
	MovementComponent->BrakingFrictionFactor = PreviousBrakingFrictionFactor;
}

void UWallRunController::JumpDuringWallRun()
{
	if (!IsWallRunning)
	{
		return;
	}
	
	FVector VerticalForce = FVector::UpVector * VerticalJumpForce;
	FVector HorizontalForce = PlayerCharacter->GetActorRightVector() * HorizontalJumpForce * (WallIsOnTheRight ? -1.0f : 1.0f);
	
	MovementComponent->Velocity = VerticalForce + HorizontalForce;
	MovementComponent->SetMovementMode(MOVE_Falling);
	
	CancelWallRun();
}

void UWallRunController::EnableGravity()
{
	if (EnableGravityAfter > 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(EnableGravityTimerHandle);
	}
	
	if (!IsWallRunning)
	{
		return;
	}
	
	UE_LOG(WallRunningLog, Warning, TEXT("Enable Gravity"));
	MovementComponent->GravityScale = GravityScale;
}

bool UWallRunController::WallFound()
{
	FHitResult HitResult;

	FVector CastStart = PlayerCharacter->GetActorLocation();
	FVector CastEnd = CastStart + PlayerCharacter->GetActorRightVector() * (WallIsOnTheRight ? 1 : -1) * DistanceToWallDuringRun * 1.1f;

	bool Hit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CastStart,
		CastEnd,
		ECC_Visibility,
		WallCollisionQueryParams
	);
	
	if (Hit && HitResult.GetActor()->ActorHasTag(TAG_WallRun.GetTag().GetTagName()))
	{
		WallRunNormal = FVector(HitResult.ImpactNormal.X, HitResult.ImpactNormal.Y, 0.0f).GetSafeNormal();
		return true;
	}
	
	return false;
}

void UWallRunController::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Only start Wall Running if we aren't already AND if the actor collided is tagged.
	if (!IsWallRunning && MovementComponent->IsFalling() && OtherActor->ActorHasTag(TAG_WallRun.GetTag().GetTagName()))
	{
		StartWallRun(Hit.ImpactPoint, Hit.ImpactNormal);
	}
}
