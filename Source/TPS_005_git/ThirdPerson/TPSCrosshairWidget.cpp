// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSCrosshairWidget.h"
#include "ThirdPerson/TPSWeaponComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Blueprint/WidgetTree.h"
#include "GameFramework/PlayerController.h"
#include "Styling/SlateBrush.h"

namespace TPSCrosshairWidgetPrivate
{
	static constexpr float CrosshairArmLength = 22.f;
	static constexpr float CrosshairThickness = 6.f;
	static constexpr float CrosshairGap = 8.f;

	static UImage* MakeBar(
		UWidgetTree* Tree,
		UCanvasPanel* Parent,
		const FName& Name,
		const FVector2D& Size,
		const FVector2D& Position)
	{
		UImage* Bar = Tree->ConstructWidget<UImage>(UImage::StaticClass(), Name);
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::Box;
		Brush.TintColor = FSlateColor(FLinearColor(1.f, 0.92f, 0.1f, 1.f));
		Brush.ImageSize = Size;
		Bar->SetBrush(Brush);

		if (UCanvasPanelSlot* Slot = Parent->AddChildToCanvas(Bar))
		{
			Slot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
			Slot->SetAlignment(FVector2D(0.5f, 0.5f));
			Slot->SetAutoSize(true);
			Slot->SetPosition(Position);
		}

		return Bar;
	}
}

UTPSCrosshairWidget::UTPSCrosshairWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(false);
	bHasScriptImplementedTick = true;
}

void UTPSCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WidgetTree)
	{
		return;
	}

	RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CrosshairCanvas"));
	WidgetTree->RootWidget = RootCanvas;

	using namespace TPSCrosshairWidgetPrivate;
	const FVector2D HorizontalSize(CrosshairArmLength, CrosshairThickness);
	const FVector2D VerticalSize(CrosshairThickness, CrosshairArmLength);
	const float HalfArm = CrosshairArmLength * 0.5f;
	const float HalfGap = CrosshairGap * 0.5f;

	HorizontalLeft = MakeBar(WidgetTree, RootCanvas, TEXT("HorizontalLeft"), HorizontalSize,
		FVector2D(-(HalfGap + HalfArm), 0.f));
	HorizontalRight = MakeBar(WidgetTree, RootCanvas, TEXT("HorizontalRight"), HorizontalSize,
		FVector2D(HalfGap + HalfArm, 0.f));
	VerticalTop = MakeBar(WidgetTree, RootCanvas, TEXT("VerticalTop"), VerticalSize,
		FVector2D(0.f, -(HalfGap + HalfArm)));
	VerticalBottom = MakeBar(WidgetTree, RootCanvas, TEXT("VerticalBottom"), VerticalSize,
		FVector2D(0.f, HalfGap + HalfArm));

	if (UCanvasPanelSlot* RootSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		RootSlot->SetAnchors(FAnchors(0.f, 0.f, 1.f, 1.f));
		RootSlot->SetOffsets(FMargin(0.f));
	}

	SetCrosshairVisible(false);
}

void UTPSCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	bool bShowCrosshair = false;
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (const APawn* Pawn = PC->GetPawn())
		{
			if (const UTPSWeaponComponent* WeaponComp = Pawn->FindComponentByClass<UTPSWeaponComponent>())
			{
				bShowCrosshair = WeaponComp->IsWeaponEquipped();
			}
		}
	}

	SetCrosshairVisible(bShowCrosshair);
}

void UTPSCrosshairWidget::SetCrosshairVisible(bool bVisible)
{
	const ESlateVisibility Vis = bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed;
	SetVisibility(Vis);
	if (HorizontalLeft) { HorizontalLeft->SetVisibility(Vis); }
	if (HorizontalRight) { HorizontalRight->SetVisibility(Vis); }
	if (VerticalTop) { VerticalTop->SetVisibility(Vis); }
	if (VerticalBottom) { VerticalBottom->SetVisibility(Vis); }
}
