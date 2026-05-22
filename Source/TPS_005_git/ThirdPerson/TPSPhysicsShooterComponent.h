// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPhysicsShooterComponent.generated.h"

/**
 * Blueprint-accessible component that spawns and launches physics projectiles.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_005_GIT_API UTPSPhysicsShooterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTPSPhysicsShooterComponent();

	/**
	 * Spawns a projectile and applies a physics impulse along the launch rotation forward axis.
	 * @param ProjectileClass Class to spawn (null = DefaultProjectileClass).
	 * @param bUseCustomTransform False = use owner camera location/rotation.
	 * @param SpawnLocation Used when bUseCustomTransform is true.
	 * @param SpawnRotation Used when bUseCustomTransform is true.
	 * @param LaunchSpeed Impulse strength; <= 0 uses DefaultLaunchSpeed.
	 */
	UFUNCTION(BlueprintCallable, Category="Shoot", meta=(AdvancedDisplay="SpawnLocation,SpawnRotation"))
	AActor* ShootObject(
		TSubclassOf<AActor> ProjectileClass,
		bool bUseCustomTransform,
		FVector SpawnLocation,
		FRotator SpawnRotation,
		float LaunchSpeed = 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot")
	TSubclassOf<AActor> DefaultProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shoot", meta=(ClampMin="100.0"))
	float DefaultLaunchSpeed = 2500.0f;
};
