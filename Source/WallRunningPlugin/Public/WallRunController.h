// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WallRunController.generated.h"

class UCapsuleComponent;
class UCharacterMovementComponent;

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Initial Wall Running Setup.
	void SetupWallRunning();
	
	void StartWallRun();

	void CancelWallRun();

private:

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
	
	//CollisionQueryParams that determine valid objects to consider in valid Wall Run target determination.
	FCollisionQueryParams WallRunCollisionQueryParams;
};
