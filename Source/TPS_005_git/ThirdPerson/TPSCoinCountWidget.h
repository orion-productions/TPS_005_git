// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPSCoinCountWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * Sci-fi coin counter HUD (bottom-left). Built in C++ so no Widget Designer setup is required.
 */
UCLASS()
class TPS_005_GIT_API UTPSCoinCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTPSCoinCountWidget(const FObjectInitializer& ObjectInitializer);

	void UpdateCoinCount(int32 Count);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY()
	TObjectPtr<UTextBlock> CoinText;
};
