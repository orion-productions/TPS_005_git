// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TPSCoinCountWidget.generated.h"

class UImage;
class UTextBlock;
class UVerticalBox;

/**
 * Bottom-left inventory HUD: coins, reserve ammo, and four weapons with magazine counts.
 */
UCLASS()
class TPS_005_GIT_API UTPSCoinCountWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTPSCoinCountWidget(const FObjectInitializer& ObjectInitializer);

	void RefreshFromInventory(class UTPSInventoryComponent* Inventory);

	static constexpr int32 HudFontSize = 40;
	static const FLinearColor NoneOwnedTextColor;
	static const FLinearColor SomeOwnedTextColor;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetLineText(UTextBlock* Line, const FString& Text, bool bHasSome);

	UPROPERTY()
	TObjectPtr<UImage> BackgroundImage;

	UPROPERTY()
	TObjectPtr<UVerticalBox> LinesBox;

	UPROPERTY()
	TObjectPtr<UTextBlock> CoinsLine;

	UPROPERTY()
	TObjectPtr<UTextBlock> AmmoLine;

	UPROPERTY()
	TObjectPtr<UTextBlock> PistolLine;

	UPROPERTY()
	TObjectPtr<UTextBlock> RifleLine;

	UPROPERTY()
	TObjectPtr<UTextBlock> ShotgunLine;

	UPROPERTY()
	TObjectPtr<UTextBlock> SniperLine;
};
