// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCoinCountWidget.h"
#include "TPS_005_gitPlayerController.h"
#include "TPSInventoryComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateColor.h"

UTPSCoinCountWidget::UTPSCoinCountWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = false;
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
	BackgroundImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.92f));

	CoinText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CoinText"));
	FSlateFontInfo Font = CoinText->GetFont();
	Font.Size = 22;
	CoinText->SetFont(Font);
	CoinText->SetColorAndOpacity(FSlateColor(FLinearColor(0.15f, 0.95f, 1.f, 1.f)));
	CoinText->SetShadowOffset(FVector2D(1.f, 1.f));
	CoinText->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.85f));
	UpdateCoinCount(0);

	InventoryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InventoryText"));
	FSlateFontInfo InvFont = InventoryText->GetFont();
	InvFont.Size = 16;
	InventoryText->SetFont(InvFont);
	InventoryText->SetColorAndOpacity(FSlateColor(FLinearColor(0.85f, 0.95f, 1.f, 1.f)));
	InventoryText->SetShadowOffset(FVector2D(1.f, 1.f));
	InventoryText->SetShadowColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.85f));
	UpdateInventorySummary(TEXT(""));

	if (UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(BackgroundImage))
	{
		PanelSlot->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
		PanelSlot->SetAlignment(FVector2D(0.f, 1.f));
		PanelSlot->SetPosition(FVector2D(16.f, -16.f));
		PanelSlot->SetSize(FVector2D(300.f, 140.f));
	}

	if (UCanvasPanelSlot* TextSlot = Canvas->AddChildToCanvas(CoinText))
	{
		TextSlot->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
		TextSlot->SetAlignment(FVector2D(0.f, 1.f));
		TextSlot->SetPosition(FVector2D(36.f, -38.f));
		TextSlot->SetSize(FVector2D(260.f, 28.f));
	}

	if (UCanvasPanelSlot* InvSlot = Canvas->AddChildToCanvas(InventoryText))
	{
		InvSlot->SetAnchors(FAnchors(0.f, 1.f, 0.f, 1.f));
		InvSlot->SetAlignment(FVector2D(0.f, 1.f));
		InvSlot->SetPosition(FVector2D(36.f, -118.f));
		InvSlot->SetSize(FVector2D(260.f, 72.f));
	}
}

void UTPSCoinCountWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ATPS_005_gitPlayerController* PC = Cast<ATPS_005_gitPlayerController>(GetOwningPlayer()))
	{
		UpdateCoinCount(PC->CoinCount);
		if (UTPSInventoryComponent* Inventory = PC->GetInventoryComponent())
		{
			FString Summary = Inventory->BuildInventorySummary();
			TArray<FString> Lines;
			Summary.ParseIntoArrayLines(Lines, false);
			if (Lines.Num() > 0 && Lines[0].StartsWith(TEXT("COINS")))
			{
				Lines.RemoveAt(0);
			}
			UpdateInventorySummary(FString::Join(Lines, TEXT("\n")));
		}
	}
}

void UTPSCoinCountWidget::UpdateCoinCount(int32 Count)
{
	if (CoinText)
	{
		CoinText->SetText(FText::FromString(FString::Printf(TEXT("COINS: %d"), Count)));
	}
}

void UTPSCoinCountWidget::UpdateInventorySummary(const FString& Summary)
{
	if (InventoryText)
	{
		InventoryText->SetText(FText::FromString(Summary));
	}
}
