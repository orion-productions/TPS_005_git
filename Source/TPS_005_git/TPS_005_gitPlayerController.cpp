// Copyright Epic Games, Inc. All Rights Reserved.


#include "TPS_005_gitPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "Blueprint/UserWidget.h"
#include "TPS_005_git.h"
#include "ThirdPerson/TPSCoinCountWidget.h"
#include "ThirdPerson/TPSInventoryComponent.h"
#include "Widgets/Input/SVirtualJoystick.h"

ATPS_005_gitPlayerController::ATPS_005_gitPlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UTPSInventoryComponent>(TEXT("InventoryComponent"));
}

void ATPS_005_gitPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &ATPS_005_gitPlayerController::HandleInventoryChanged);
		CoinCount = InventoryComponent->GetCoinCount();
	}

	if (IsLocalPlayerController())
	{
		if (!CoinCountWidget)
		{
			CoinCountWidget = CreateWidget<UTPSCoinCountWidget>(this, UTPSCoinCountWidget::StaticClass());
			if (CoinCountWidget)
			{
				CoinCountWidget->AddToPlayerScreen(10);
			}
		}

		if (!GetHUD())
		{
			SpawnDefaultHUD();
		}
	}

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogTPS_005_git, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ATPS_005_gitPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}

			EnsureShootInputMapping();
		}
	}
}

void ATPS_005_gitPlayerController::EnsureShootInputMapping()
{
	if (!IsLocalPlayerController())
	{
		return;
	}

	UInputAction* ShootAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Shoot.IA_Shoot"));
	if (!ShootAction)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	if (!ShootInputMappingContext)
	{
		ShootInputMappingContext = NewObject<UInputMappingContext>(this, TEXT("ShootInputMappingContext"));
		ShootInputMappingContext->MapKey(ShootAction, EKeys::K);
	}

	Subsystem->AddMappingContext(ShootInputMappingContext, 1);
}

bool ATPS_005_gitPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void ATPS_005_gitPlayerController::HandleInventoryChanged()
{
	if (InventoryComponent)
	{
		CoinCount = InventoryComponent->GetCoinCount();
	}
}
