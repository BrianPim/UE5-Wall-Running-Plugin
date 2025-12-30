// Copyright (c) 2025 Brian Pimentel


#include "WallRunController.h"

#include "WallRunningLog.h"
#include "WallRunningTags.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
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

void UWallRunController::SetupWallRunning()
{
	PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	checkf(PlayerCharacter, TEXT("Unable to get reference to the Local Player's Character"));

	PlayerController = Cast<APlayerController>(PlayerCharacter->GetController());
	checkf(PlayerController, TEXT("Unable to get reference to the Local Player's PlayerController"));

	MovementComponent = PlayerCharacter->GetCharacterMovement();
	checkf(MovementComponent, TEXT("Unable to get reference to the Local Player's CharacterMovementComponent"));

	if (ColliderComponent == nullptr)
	{
		ColliderComponent = PlayerCharacter->GetCapsuleComponent();
	}

	if (ColliderComponent)
	{
		ColliderComponent->OnComponentHit.AddDynamic(
			this,
			&UWallRunController::OnWallHit
		);
	}
}

void UWallRunController::StartWallRun()
{
	UE_LOG(WallRunningLog, Warning, TEXT("WallRunController::StartWallRun"));
}

void UWallRunController::CancelWallRun()
{
	
}


// Called every frame
void UWallRunController::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWallRunController::OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(WallRunningLog, Warning, TEXT("WallRunController::OnWallHit"));
	
	if (OtherActor->ActorHasTag(TAG_WallRun.GetTag().GetTagName()))
	{
		StartWallRun();
	}
}

