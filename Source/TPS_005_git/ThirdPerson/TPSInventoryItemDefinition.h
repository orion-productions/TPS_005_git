// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TPSInventoryTypes.h"
#include "TPSInventoryItemDefinition.generated.h"

class USoundBase;
class UStaticMesh;

/**
 * Defines a collectible item (coin, weapon, ammo) for pickups and inventory storage.
 */
UCLASS(BlueprintType)
class TPS_005_GIT_API UTPSInventoryItemDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Stable id used in inventory maps (e.g. Coin, Weapon_Pistol, Ammo_Rifle). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FName ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	ETPSInventoryItemType ItemType = ETPSInventoryItemType::Currency;

	/** For weapons and matching ammo pickups. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	ETPSWeaponFamily WeaponFamily = ETPSWeaponFamily::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USoundBase> PickupSound;

	/** Reference mesh for pickup blueprint setup (not spawned automatically). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMesh> WorldMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = 1))
	int32 DefaultPickupQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = 1))
	int32 MaxStack = 99;
};
