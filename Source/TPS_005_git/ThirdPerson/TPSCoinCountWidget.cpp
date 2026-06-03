// Copyright Epic Games, Inc. All Rights Reserved.
// Inventory HUD: 580x400 panel, 40pt font, red/yellow/light-blue ownership colors.

#include "TPSCoinCountWidget.h"
#include "TPS_005_gitPlayerController.h"
#include "TPS_005_gitCharacter.h"
#include "TPSInventoryComponent.h"
#include "ThirdPerson/TPSWeaponComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateColor.h"

const FLinearColor UTPSCoinCountWidget::NoneOwnedTextColor  = FLinearColor(0.95f, 0.12f, 0.12f, 1.f);
const FLinearColor UTPSCoinCountWidget::SomeOwnedTextColor  = FLinearColor(1.f,   0.92f, 0.1f,  1.f);
const FLinearColor UTPSCoinCountWidget::EquippedWeaponColor = FLinearColor(0.4f,  0.85f, 1.f,   1.f);

namespace TPSCoinCountWidgetPrivate
{
	UTextBlock* MakeLine(UWidgetTree* Tree, UVerticalBox* Parent, const FName& Name)
	{
		UTextBlock* Line = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		FSlateFontInfo Font = Line->GetFont();
		Font.Size = UTPSCoinCountWidget::HudFontSize;
		Line->SetFont(Font);
		Line->SetShadowOffset(FVector2D(1.f, 1.f));
		Line->SetShadowColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.35f));

		if (UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Line))
		{
			Slot->SetPadding(FMargin(0.f, 0.f, 0.f, 10.f));
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
		}

		return Line;
	}
}

UTPSCoinCountWidget::UTPSCoinCountWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(false);
}

void UTPSCoinCountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree)
	{
		return;
	}

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvas"));
	WidgetTree->RootWidget = Canvas;

	BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("PanelImage"));
	if (UTexture2D* PanelTex = LoadObject<UTexture2D>(
		nullptr, TEXT("/Game/SciFi_UI_Project/RP_Item_Container_A.RP_Item_Container_A")))
	{
		BackgroundImage->SetBrushFromTexture(PanelTex, true);
	}
	BackgroundImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.94f));

	LinesBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("LinesBox"));
	CoinsLine = TPSCoinCountWidgetPrivate::MakeLine(WidgetTree, LinesBox, TEXT("CoinsLine"));
	AmmoLine = TPSCoinCountWidgetPrivate::MakeLine(WidgetTree, LinesBox, TEXT("AmmoLine"));
	PistolLine = TPSCoinCountWidgetPrivate::MakeLine(WidgetTree, LinesBox, TEXT("PistolLine"));
	RifleLine = TPSCoinCountWidgetPrivate::MakeLine(WidgetTree, LinesBox, TEXT("RifleLine"));
	ShotgunLine = TPSCoinCountWidgetPrivate::MakeLine(WidgetTree, LinesBox, TEXT("ShotgunLine"));
	SniperLine = TPSCoinCountWidgetPrivate::MakeLine(WidgetTree, LinesBox, TEXT("SniperLine"));

	if (UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(BackgroundImage))
	{
		PanelSlot->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
		PanelSlot->SetAlignment(FVector2D(0.f, 1.f));
		PanelSlot->SetPosition(FVector2D(24.f, -24.f));
		PanelSlot->SetSize(FVector2D(580.f, 400.f));
	}

	if (UCanvasPanelSlot* BoxSlot = Canvas->AddChildToCanvas(LinesBox))
	{
		BoxSlot->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
		BoxSlot->SetAlignment(FVector2D(0.f, 1.f));
		BoxSlot->SetPosition(FVector2D(52.f, -368.f));
		BoxSlot->SetSize(FVector2D(500.f, 340.f));
	}

	RefreshFromInventory(nullptr);
}

void UTPSCoinCountWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ATPS_005_gitPlayerController* PC = Cast<ATPS_005_gitPlayerController>(GetOwningPlayer()))
	{
		ETPSWeaponFamily Equipped = ETPSWeaponFamily::None;
		if (const ATPS_005_gitCharacter* Char = Cast<ATPS_005_gitCharacter>(PC->GetPawn()))
		{
			if (const UTPSWeaponComponent* WC = Char->GetWeaponComponent())
			{
				Equipped = WC->GetEquippedWeapon();
			}
		}

		RefreshFromInventory(PC->GetInventoryComponent(), Equipped);
	}
}

void UTPSCoinCountWidget::SetLineText(UTextBlock* Line, const FString& Text, bool bHasSome)
{
	SetLineTextWithColor(Line, Text, bHasSome ? SomeOwnedTextColor : NoneOwnedTextColor);
}

void UTPSCoinCountWidget::SetLineTextWithColor(UTextBlock* Line, const FString& Text, const FLinearColor& Color)
{
	if (!Line)
	{
		return;
	}
	Line->SetText(FText::FromString(Text));
	Line->SetColorAndOpacity(FSlateColor(Color));
}

void UTPSCoinCountWidget::RefreshFromInventory(UTPSInventoryComponent* Inventory, ETPSWeaponFamily EquippedWeapon)
{
	const int32 Coins = Inventory ? Inventory->GetCoinCount() : 0;
	const int32 Ammo  = Inventory ? Inventory->GetReserveAmmo() : 0;

	SetLineText(CoinsLine, FString::Printf(TEXT("Coins: %d"), Coins), Coins > 0);
	SetLineText(AmmoLine,  FString::Printf(TEXT("Ammo: %d"),  Ammo),  Ammo  > 0);

	auto UpdateWeaponLine = [this, Inventory, EquippedWeapon](UTextBlock* Line, ETPSWeaponFamily Family, const TCHAR* Label)
	{
		if (!Line)
		{
			return;
		}

		const bool bOwned    = Inventory && Inventory->HasWeapon(Family);
		const bool bEquipped = (EquippedWeapon == Family);

		if (bEquipped && Inventory)
		{
			const int32 Mag = Inventory->GetWeaponMagazineAmmo(Family);
			SetLineTextWithColor(Line, FString::Printf(TEXT("%s: %d"), Label, Mag), EquippedWeaponColor);
		}
		else if (bOwned)
		{
			const int32 Mag = Inventory->GetWeaponMagazineAmmo(Family);
			SetLineText(Line, FString::Printf(TEXT("%s: %d"), Label, Mag), true);
		}
		else
		{
			SetLineText(Line, Label, false);
		}
	};

	UpdateWeaponLine(PistolLine,  ETPSWeaponFamily::Pistol,  TEXT("Pistol"));
	UpdateWeaponLine(RifleLine,   ETPSWeaponFamily::Rifle,   TEXT("Rifle"));
	UpdateWeaponLine(ShotgunLine, ETPSWeaponFamily::Shotgun, TEXT("ShotGun"));
	UpdateWeaponLine(SniperLine,  ETPSWeaponFamily::Sniper,  TEXT("Sniper"));
}
