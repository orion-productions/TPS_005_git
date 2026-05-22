// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TPSCoinHUD.generated.h"

/**
 * Draws the coin counter ("COINS: N") at the bottom-left of the screen.
 * Set this as the HUD class in BP_ThirdPersonGameMode.
 */
UCLASS()
class TPS_005_GIT_API ATPSCoinHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
