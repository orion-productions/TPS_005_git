// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCoinHUD.h"
#include "TPS_005_gitPlayerController.h"
#include "TPSInventoryComponent.h"
#include "ThirdPerson/TPSWeaponComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/Pawn.h"

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
	const FColor EquippedBlue(102, 217, 255);

	ETPSWeaponFamily EquippedWeapon = ETPSWeaponFamily::None;
	if (const APawn* Pawn = PlayerOwner ? PlayerOwner->GetPawn() : nullptr)
	{
		if (const UTPSWeaponComponent* WeaponComp = Pawn->FindComponentByClass<UTPSWeaponComponent>())
		{
			EquippedWeapon = WeaponComp->GetEquippedWeapon();
		}
	}

	auto DrawLine = [&](const FString& Text, FColor Color)
	{
		DrawText(Text, Color, X + 28.f, LineY, nullptr, Scale, false);
		LineY += LineStep;
	};

	auto DrawWeaponLine = [&](const TCHAR* Label, ETPSWeaponFamily Family)
	{
		const bool bOwned = Inventory && Inventory->HasWeapon(Family);
		const bool bEquipped = (EquippedWeapon == Family);
		FString Text(Label);
		if (bOwned)
		{
			Text = FString::Printf(TEXT("%s: %d"), Label, Inventory->GetWeaponMagazineAmmo(Family));
		}

		if (bEquipped)
		{
			DrawLine(Text, EquippedBlue);
		}
		else
		{
			DrawLine(Text, bOwned ? Yellow : Red);
		}
	};

	const int32 Coins = Inventory ? Inventory->GetCoinCount() : 0;
	const int32 Ammo = Inventory ? Inventory->GetReserveAmmo() : 0;

	DrawLine(FString::Printf(TEXT("Coins: %d"), Coins), Coins > 0 ? Yellow : Red);
	DrawLine(FString::Printf(TEXT("Ammo: %d"), Ammo), Ammo > 0 ? Yellow : Red);
	DrawWeaponLine(TEXT("Pistol"), ETPSWeaponFamily::Pistol);
	DrawWeaponLine(TEXT("Rifle"), ETPSWeaponFamily::Rifle);
	DrawWeaponLine(TEXT("ShotGun"), ETPSWeaponFamily::Shotgun);
	DrawWeaponLine(TEXT("Sniper"), ETPSWeaponFamily::Sniper);

	if (EquippedWeapon != ETPSWeaponFamily::None)
	{
		const float CenterX = Canvas->SizeX * 0.5f;
		const float CenterY = Canvas->SizeY * 0.5f;
		const float ArmLength = 22.f;
		const float Thickness = 6.f;
		const float Gap = 8.f;
		const FLinearColor CrosshairYellow(1.f, 0.92f, 0.1f, 1.f);

		DrawRect(CrosshairYellow, CenterX - Gap - ArmLength, CenterY - Thickness * 0.5f, ArmLength, Thickness);
		DrawRect(CrosshairYellow, CenterX + Gap, CenterY - Thickness * 0.5f, ArmLength, Thickness);
		DrawRect(CrosshairYellow, CenterX - Thickness * 0.5f, CenterY - Gap - ArmLength, Thickness, ArmLength);
		DrawRect(CrosshairYellow, CenterX - Thickness * 0.5f, CenterY + Gap, Thickness, ArmLength);
	}
}
