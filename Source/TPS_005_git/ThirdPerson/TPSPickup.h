// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSInventoryTypes.h"
#include "TPSPickup.generated.h"

class USphereComponent;
class USoundBase;
class UStaticMeshComponent;
class APlayerController;
class UTPSInventoryItemDefinition;

/**
 *  Base class for collectible pickups in the ThirdPerson variant.
 *  Rotates continuously at runtime; Blueprint subclasses add mesh/effects
 *  and implement BP_OnPickedUp to play feedback and destroy the actor.
 */
UCLASS(abstract)
class TPS_005_GIT_API ATPSPickup : public AActor
{
	GENERATED_BODY()

	/** Pickup bounding sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	USphereComponent* Sphere;

	/** Optional world mesh (weapons/ammo). Coins usually use a Blueprint mesh instead. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PickupMesh;

public:

	ATPSPickup();

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

protected:

	/** Degrees per second the pickup rotates around its Z axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	float RotationSpeed = 90.0f;

	/** Sound played when the player collects this pickup */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UTPSInventoryItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 0))
	int32 QuantityOverride = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Fallback")
	FName FallbackItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Fallback")
	ETPSInventoryItemType FallbackItemType = ETPSInventoryItemType::Currency;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Fallback", meta = (ClampMin = 1))
	int32 FallbackQuantity = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Visual")
	TObjectPtr<UStaticMesh> WorldMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Visual")
	FVector WorldMeshScale = FVector(1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Visual")
	FRotator WorldMeshRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Visual")
	FVector WorldMeshOffset = FVector::ZeroVector;

	/** Ammo pickups hide and reappear after this delay instead of being destroyed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup|Respawn", meta = (ClampMin = 0.1))
	float AmmoRespawnDelaySeconds = 10.0f;

	FTimerHandle AmmoRespawnTimerHandle;
	FTransform SpawnTransform;
	bool bAmmoRespawnPending = false;

	bool IsAmmoPickup() const;
	void BeginAmmoRespawnCooldown();
	void FinishAmmoRespawn();

	/** Handles pickup collision */
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Apply collection logic (inventory, VFX). Called after overlap validation. */
	virtual void ApplyPickup(APlayerController* PlayerController);

	USoundBase* ResolvePickupSound() const;
	void PlayPickupFeedback(APlayerController* PlayerController, USoundBase* SoundToPlay) const;

	void ApplyWorldMesh();

	/** Override in Blueprint to play effects and destroy the actor */
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta=(DisplayName="On Picked Up"))
	void BP_OnPickedUp();

public:

	FORCEINLINE USphereComponent* GetSphere() const { return Sphere; }
};
