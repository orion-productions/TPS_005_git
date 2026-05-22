// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPickup.generated.h"

class USphereComponent;
class USoundBase;

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

public:

	ATPSPickup();

	virtual void Tick(float DeltaTime) override;

protected:

	/** Degrees per second the pickup rotates around its Z axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	float RotationSpeed = 90.0f;

	/** Sound played when the player collects this pickup */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pickup")
	TObjectPtr<USoundBase> PickupSound;

	/** Handles pickup collision */
	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	/** Override in Blueprint to play effects and destroy the actor */
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta=(DisplayName="On Picked Up"))
	void BP_OnPickedUp();

public:

	FORCEINLINE USphereComponent* GetSphere() const { return Sphere; }
};
