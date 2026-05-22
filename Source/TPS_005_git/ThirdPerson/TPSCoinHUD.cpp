// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCoinHUD.h"
#include "TPS_005_gitPlayerController.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"

void ATPSCoinHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	int32 CoinCount = 0;
	if (const ATPS_005_gitPlayerController* PC = Cast<ATPS_005_gitPlayerController>(PlayerOwner))
	{
		CoinCount = PC->CoinCount;
	}

	const FString Text = FString::Printf(TEXT("COINS: %d"), CoinCount);

	const float PanelW = 280.f;
	const float PanelH = 72.f;
	const float X = 16.f;
	const float Y = FMath::Max(10.f, Canvas->SizeY - PanelH - 16.f);

	if (UTexture2D* PanelTex = LoadObject<UTexture2D>(
		nullptr, TEXT("/Game/SciFi_UI_Project/RP_Item_Container_A.RP_Item_Container_A")))
	{
		DrawTexture(PanelTex, X, Y, PanelW, PanelH, 0.f, 0.f, 1.f, 1.f, FLinearColor(1.f, 1.f, 1.f, 0.92f));
	}

	DrawText(Text, FColor(38, 242, 255), X + 20.f, Y + PanelH * 0.35f, nullptr, 1.4f, false);
}
