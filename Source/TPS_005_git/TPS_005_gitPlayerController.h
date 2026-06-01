// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TPS_005_gitPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UTPSCoinCountWidget;
class UTPSInventoryComponent;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class ATPS_005_gitPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Runtime K->Shoot mapping (backup if character mapping is not applied). */
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> ShootInputMappingContext;

public:
	ATPS_005_gitPlayerController();

	/** Tracks coins, weapons, and ammo pickups. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UTPSInventoryComponent> InventoryComponent;

	/** Deprecated: use InventoryComponent coin quantity. Kept for Blueprint compatibility. */
	UPROPERTY(BlueprintReadOnly, Category = "Coins")
	int32 CoinCount = 0;

	/** Sci-fi UMG coin counter (bottom-left) */
	UPROPERTY()
	TObjectPtr<UTPSCoinCountWidget> CoinCountWidget;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	UTPSInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	void EnsureShootInputMapping();

	UFUNCTION()
	void HandleInventoryChanged();

};
