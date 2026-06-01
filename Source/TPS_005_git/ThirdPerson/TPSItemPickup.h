// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TPSPickup.h"
#include "TPSItemPickup.generated.h"

/**
 * Pickup that adds a defined item to the player's inventory (logic lives on ATPSPickup).
 */
UCLASS()
class TPS_005_GIT_API ATPSItemPickup : public ATPSPickup
{
	GENERATED_BODY()

public:
	ATPSItemPickup();
};
