// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunController.generated.h"

class UCapsuleComponent;
class UCharacterMovementComponent;
class UInputAction;

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

private:

	//Defaults
	static constexpr float BaseWallRunSpeed = 300.0f;
	static constexpr float BaseWallRunDuration = 1.5f;
	static constexpr float BaseWallRunGravityScale = 0.1f;
	static constexpr float BaseDistanceToWallDuringRun = 32.0f;

	//Player Character's velocity while Wall Running.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float WallRunSpeed = BaseWallRunSpeed;

	//How long to Wall Run for before falling. Set to <= 0 to Wall Run indefinitely.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float WallRunDuration = BaseWallRunDuration;

	//Gravity Scale during the wall run. Set to 0 to remain at the same Z coordinate the entire time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float WallRunGravityScale = BaseWallRunGravityScale;

	//How far the Player Character is positioned from the wall during Wall Run. Recommended to make this the radius of
	//your Player Character's Capsule Component.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Running", meta = (AllowPrivateAccess = "true"))
	float DistanceToWallDuringRun = BaseDistanceToWallDuringRun;

	//Used to store a reference to the Player's PlayerController.
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController = nullptr;

	//Used to store a reference to the Character we are controlling.
	UPROPERTY()
	TObjectPtr<ACharacter> PlayerCharacter = nullptr;
	
	//Used to store a reference to the Player Character's MoveComponent.
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent = nullptr;

	//Used to store a reference to the player's collision component, used for tracking Hit events.
	UPROPERTY()
	TObjectPtr<UCapsuleComponent> ColliderComponent = nullptr;
	//Used to store a reference to the InputComponent cast to an EnhancedInputComponent
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;

	bool IsWallRunning = false;

	float PreviousGravityScale = 0;
	
	FTimerHandle EndWallRunTimerHandle;
	FVector WallRunDirection = FVector::ZeroVector;
};
