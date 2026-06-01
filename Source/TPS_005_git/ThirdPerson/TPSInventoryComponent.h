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

	/** Shared reserve ammo pool (each ammo pickup adds AmmoPerPickup). */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetReserveAmmo() const { return ReserveAmmo; }

	/** Rounds currently in the weapon magazine (full mag when first picked up). */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetWeaponMagazineAmmo(ETPSWeaponFamily WeaponFamily) const;

	/** Max magazine size for a weapon family. */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	static int32 GetMaxMagazineForWeapon(ETPSWeaponFamily WeaponFamily);

	/** Fills magazine up to max from reserve (for future equip/reload). */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ReloadWeaponMagazine(ETPSWeaponFamily WeaponFamily);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	FString BuildInventorySummary() const;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnTPSInventoryChanged OnInventoryChanged;

	/** Added to reserve ammo for every ammo pickup, regardless of pickup type. */
	static constexpr int32 AmmoPerPickup = 20;

protected:
	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TMap<FName, int32> ItemQuantities;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TMap<FName, uint8> ItemTypes;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	int32 ReserveAmmo = 0;

	UPROPERTY(VisibleInstanceOnly, Category = "Inventory")
	TMap<uint8, int32> WeaponMagazineAmmo;

	static FName AmmoItemIdForWeaponFamily(ETPSWeaponFamily WeaponFamily);
	static FName WeaponItemIdForWeaponFamily(ETPSWeaponFamily WeaponFamily);
	static ETPSWeaponFamily WeaponFamilyFromItemId(FName ItemId);
};
