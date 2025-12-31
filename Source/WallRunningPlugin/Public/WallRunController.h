// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunController.generated.h"

class UCapsuleComponent;
class UCharacterMovementComponent;
class UInputAction;
class UEnhancedInputComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WALLRUNNINGPLUGIN_API UWallRunController : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWallRunController();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	// Event handler function for "Hit" events
	UFUNCTION()
	void OnWallHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//Input Action to map to Jump. Can hook your existing IA into this.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running")
	TObjectPtr<UInputAction> ActionJump = nullptr;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Initial Wall Running Setup.
	void SetupWallRunning();
	
	void StartWallRun(FVector ImpactPosition, FVector ImpactNormal);

	void CancelWallRun();

	void EnableGravity();

	//Checks if the Player still has a wall to run along.
	bool WallFound();

private:

	//Defaults
	static constexpr float BaseSpeed = 300.0f;
	static constexpr float BaseDuration = 1.5f;
	static constexpr float BaseEnableGravityAfter = 0.5f;
	static constexpr float BaseGravityScale = 0.1f;
	static constexpr float BaseDistanceToWallDuringRun = 32.0f;

	//Player Character's velocity while Wall Running.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float Speed = BaseSpeed;

	//How long to Wall Run for. Set to <= 0 to Wall Run indefinitely.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float Duration = BaseDuration;

	//How long to Wall Run for before gravity begins to kick in. Set to <= 0 to immediately be affected by Gravity.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float EnableGravityAfter = BaseEnableGravityAfter;

	//Gravity Scale during the wall run. Set to 0 to remain at the same Z coordinate the entire time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float GravityScale = BaseGravityScale;

	//How far the Player Character is positioned from the wall during Wall Run. Recommended to make this the radius of
	//your Player Character's Capsule Component.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float DistanceToWallDuringRun = BaseDistanceToWallDuringRun;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> PlayerCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UCapsuleComponent> ColliderComponent = nullptr;
	
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;
	
	FTimerHandle EndWallRunTimerHandle;
	FTimerHandle EnableGravityTimerHandle;

	FCollisionQueryParams WallCollisionQueryParams;
	
	FVector WallRunDirection = FVector::ZeroVector;
	FVector WallRunNormal = FVector::ZeroVector;
	bool WallIsOnTheRight = false;

	bool IsWallRunning = false;
	float PreviousGravityScale = 0.0f;
	float PreviousAirControl = 0.0f;
	float PreviousMaxAcceleration = 0.0f;
	float PreviousBrakingDecelerationFalling = 0.0f;
	float PreviousBrakingFrictionFactor = 0.0f;
};
