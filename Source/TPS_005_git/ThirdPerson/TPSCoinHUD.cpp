// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCoinHUD.h"
#include "TPS_005_gitPlayerController.h"
#include "TPSInventoryComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"

void ATPSCoinHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	FString Summary = TEXT("COINS: 0");
	if (const ATPS_005_gitPlayerController* PC = Cast<ATPS_005_gitPlayerController>(PlayerOwner))
	{
		if (const UTPSInventoryComponent* Inventory = PC->GetInventoryComponent())
		{
			Summary = Inventory->BuildInventorySummary();
		}
		else
		{
			Summary = FString::Printf(TEXT("COINS: %d"), PC->CoinCount);
		}
	}

	const float PanelW = 300.f;
	const float PanelH = 140.f;
	const float X = 16.f;
	const float Y = FMath::Max(10.f, Canvas->SizeY - PanelH - 16.f);

	if (UTexture2D* PanelTex = LoadObject<UTexture2D>(
		nullptr, TEXT("/Game/SciFi_UI_Project/RP_Item_Container_A.RP_Item_Container_A")))
	{
		DrawTexture(PanelTex, X, Y, PanelW, PanelH, 0.f, 0.f, 1.f, 1.f, FLinearColor(1.f, 1.f, 1.f, 0.92f));
	}

	float LineY = Y + 18.f;
	TArray<FString> Lines;
	Summary.ParseIntoArrayLines(Lines, false);
	for (const FString& Line : Lines)
	{
		DrawText(Line, FColor(38, 242, 255), X + 20.f, LineY, nullptr, Line.StartsWith(TEXT("COINS")) ? 1.4f : 1.1f, false);
		LineY += Line.StartsWith(TEXT("COINS")) ? 28.f : 22.f;
	}
}
