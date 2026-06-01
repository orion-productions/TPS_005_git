// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TPSInventoryTypes.generated.h"

UENUM(BlueprintType)
enum class ETPSInventoryItemType : uint8
{
	Currency UMETA(DisplayName = "Currency"),
	Weapon   UMETA(DisplayName = "Weapon"),
	Ammo     UMETA(DisplayName = "Ammo"),
};

UENUM(BlueprintType)
enum class ETPSWeaponFamily : uint8
{
	None    UMETA(DisplayName = "None"),
	Pistol  UMETA(DisplayName = "Pistol"),
	Rifle   UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Sniper  UMETA(DisplayName = "Sniper"),
};
