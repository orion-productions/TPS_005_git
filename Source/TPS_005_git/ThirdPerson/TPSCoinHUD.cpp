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

	const UTPSInventoryComponent* Inventory =
		Cast<ATPS_005_gitPlayerController>(PlayerOwner)
			? Cast<ATPS_005_gitPlayerController>(PlayerOwner)->GetInventoryComponent()
			: nullptr;

	const float PanelW = 580.f;
	const float PanelH = 400.f;
	const float X = 24.f;
	const float Y = FMath::Max(10.f, Canvas->SizeY - PanelH - 24.f);

	if (UTexture2D* PanelTex = LoadObject<UTexture2D>(
		nullptr, TEXT("/Game/SciFi_UI_Project/RP_Item_Container_A.RP_Item_Container_A")))
	{
		DrawTexture(PanelTex, X, Y, PanelW, PanelH, 0.f, 0.f, 1.f, 1.f, FLinearColor(1.f, 1.f, 1.f, 0.94f));
	}

	const float Scale = 2.35f;
	float LineY = Y + 36.f;
	const float LineStep = 48.f;

	const FColor Red(242, 31, 31);
	const FColor Yellow(255, 235, 26);

	auto DrawLine = [&](const FString& Text, bool bHasSome)
	{
		DrawText(Text, bHasSome ? Yellow : Red, X + 28.f, LineY, nullptr, Scale, false);
		LineY += LineStep;
	};

	auto DrawWeaponLine = [&](const TCHAR* Label, ETPSWeaponFamily Family)
	{
		const bool bOwned = Inventory && Inventory->HasWeapon(Family);
		FString Text(Label);
		if (bOwned)
		{
			Text = FString::Printf(TEXT("%s: %d"), Label, Inventory->GetWeaponMagazineAmmo(Family));
		}

		DrawLine(Text, bOwned);
	};

	const int32 Coins = Inventory ? Inventory->GetCoinCount() : 0;
	const int32 Ammo = Inventory ? Inventory->GetReserveAmmo() : 0;

	DrawLine(FString::Printf(TEXT("Coins: %d"), Coins), Coins > 0);
	DrawLine(FString::Printf(TEXT("Ammo: %d"), Ammo), Ammo > 0);
	DrawWeaponLine(TEXT("Pistol"), ETPSWeaponFamily::Pistol);
	DrawWeaponLine(TEXT("Rifle"), ETPSWeaponFamily::Rifle);
	DrawWeaponLine(TEXT("ShotGun"), ETPSWeaponFamily::Shotgun);
	DrawWeaponLine(TEXT("Sniper"), ETPSWeaponFamily::Sniper);
}
