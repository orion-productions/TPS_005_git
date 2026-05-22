// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPhysicsProjectile.generated.h"

class UStaticMeshComponent;

/** Physics sphere launched by UTPSPhysicsShooterComponent. */
UCLASS(Blueprintable)
class TPS_005_GIT_API ATPSPhysicsProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* Mesh;

public:
	ATPSPhysicsProjectile();
};
