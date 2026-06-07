// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TPSAnimInstance.generated.h"

/**
 * Anim instance for third-person aim layering (bIsAiming drives upper-body ADS overlay).
 */
UCLASS()
class TPS_005_GIT_API UTPSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/** When true, layered upper-body ADS pose blends over locomotion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
	bool bIsAiming = false;
};
