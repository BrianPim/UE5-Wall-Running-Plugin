// Copyright (c) 2025 Brian Pimentel


#include "WallRunController.h"

#include "WallRunningLog.h"
#include "WallRunningTags.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWallRunController::UWallRunController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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
		
	}
}

void UWallRunController::SetupWallRunning()
{
	PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	checkf(PlayerCharacter, TEXT("Unable to get reference to the Local Player's Character"));

	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	checkf(PlayerController, TEXT("Unable to get reference to the Local Player's PlayerController"));

	MovementComponent = PlayerCharacter->GetCharacterMovement();
	checkf(MovementComponent, TEXT("Unable to get reference to the Player Character's CharacterMovementComponent"));

	ColliderComponent = PlayerCharacter->GetCapsuleComponent();
	checkf(ColliderComponent, TEXT("Unable to get reference to the Player Character's CapsuleComponent"));

	ColliderComponent->OnComponentHit.AddDynamic(
		this,
		&UWallRunController::OnWallHit);
}

void UWallRunController::StartWallRun(FVector ImpactPosition, FVector ImpactNormal)
{
	IsWallRunning = true;
	
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

	FVector ModifiedImpactPosition = FVector(ImpactPosition.X, ImpactPosition.Y, PlayerCharacter->GetActorLocation().Z);
	FVector TargetLocation = ModifiedImpactPosition + ImpactNormal * DistanceToWallDuringRun;
	FRotator TargetRotation = FRotationMatrix::MakeFromXZ(WallTangent, FVector::UpVector).Rotator();

	MovementComponent->StopMovementImmediately();
	PlayerController->SetIgnoreMoveInput(true);
	MovementComponent->SetMovementMode(MOVE_Flying);
	MovementComponent->GravityScale = 0.0f;
	
	PlayerCharacter->SetActorLocationAndRotation(
	TargetLocation,
	TargetRotation,
	false,
	nullptr,
	ETeleportType::TeleportPhysics);
}

void UWallRunController::CancelWallRun()
{
	if (!IsWallRunning)
	{
		return;
	}

	IsWallRunning = false;
}

void UWallRunController::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Only start Wall Running if we aren't already AND if the actor collided is tagged.
	if (!IsWallRunning && OtherActor->ActorHasTag(TAG_WallRun.GetTag().GetTagName()))
	{
		StartWallRun(Hit.ImpactPoint, Hit.ImpactNormal);
	}
}

