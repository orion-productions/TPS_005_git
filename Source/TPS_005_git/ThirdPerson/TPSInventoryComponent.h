// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSInventoryTypes.h"
#include "TPSInventoryComponent.generated.h"

class UTPSInventoryItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTPSInventoryChanged);

/**
 * Tracks coins, weapons, and ammo collected by the owning player controller.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TPS_005_GIT_API UTPSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static const FName CoinItemId;

	UTPSInventoryComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItemFromDefinition(const UTPSInventoryItemDefinition* ItemDefinition, int32 AmountOverride = 0);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName ItemId, int32 Amount, ETPSInventoryItemType ItemType = ETPSInventoryItemType::Currency);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(FName ItemId) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetCoinCount() const { return GetItemQuantity(CoinItemId); }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasWeapon(ETPSWeaponFamily WeaponFamily) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetAmmoForWeaponFamily(ETPSWeaponFamily WeaponFamily) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FString BuildInventorySummary() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTPSInventoryChanged OnInventoryChanged;

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TMap<FName, int32> ItemQuantities;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TMap<FName, uint8> ItemTypes;

	static FName AmmoItemIdForWeaponFamily(ETPSWeaponFamily WeaponFamily);
	static FName WeaponItemIdForWeaponFamily(ETPSWeaponFamily WeaponFamily);
};
