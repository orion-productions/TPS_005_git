// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPSCrosshairWidget.generated.h"

class UCanvasPanel;
class UImage;

/**
 * Screen-center crosshair shown while a weapon is equipped.
 */
UCLASS()
class TPS_005_GIT_API UTPSCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTPSCrosshairWidget(const FObjectInitializer& ObjectInitializer);

	void SetCrosshairVisible(bool bVisible);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY()
	TObjectPtr<UCanvasPanel> RootCanvas;

	UPROPERTY()
	TObjectPtr<UImage> HorizontalLeft;

	UPROPERTY()
	TObjectPtr<UImage> HorizontalRight;

	UPROPERTY()
	TObjectPtr<UImage> VerticalTop;

	UPROPERTY()
	TObjectPtr<UImage> VerticalBottom;
};
