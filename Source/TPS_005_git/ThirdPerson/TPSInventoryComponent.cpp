// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSInventoryComponent.h"
#include "TPSInventoryItemDefinition.h"

const FName UTPSInventoryComponent::CoinItemId(TEXT("Coin"));

UTPSInventoryComponent::UTPSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UTPSInventoryComponent::AddItemFromDefinition(const UTPSInventoryItemDefinition* ItemDefinition, int32 AmountOverride)
{
	if (!ItemDefinition || ItemDefinition->ItemId.IsNone())
	{
		return false;
	}

	const int32 Amount = AmountOverride > 0 ? AmountOverride : ItemDefinition->DefaultPickupQuantity;
	return AddItem(ItemDefinition->ItemId, Amount, ItemDefinition->ItemType);
}

bool UTPSInventoryComponent::AddItem(FName ItemId, int32 Amount, ETPSInventoryItemType ItemType)
{
	if (ItemId.IsNone() || Amount <= 0)
	{
		return false;
	}

	if (ItemType == ETPSInventoryItemType::Weapon && GetItemQuantity(ItemId) > 0)
	{
		return false;
	}

	int32* Existing = ItemQuantities.Find(ItemId);
	const int32 NewTotal = (Existing ? *Existing : 0) + Amount;
	ItemQuantities.Add(ItemId, NewTotal);
	ItemTypes.Add(ItemId, static_cast<uint8>(ItemType));

	OnInventoryChanged.Broadcast();
	return true;
}

int32 UTPSInventoryComponent::GetItemQuantity(FName ItemId) const
{
	const int32* Quantity = ItemQuantities.Find(ItemId);
	return Quantity ? *Quantity : 0;
}

bool UTPSInventoryComponent::HasWeapon(ETPSWeaponFamily WeaponFamily) const
{
	const FName WeaponId = WeaponItemIdForWeaponFamily(WeaponFamily);
	return !WeaponId.IsNone() && GetItemQuantity(WeaponId) > 0;
}

int32 UTPSInventoryComponent::GetAmmoForWeaponFamily(ETPSWeaponFamily WeaponFamily) const
{
	const FName AmmoId = AmmoItemIdForWeaponFamily(WeaponFamily);
	return AmmoId.IsNone() ? 0 : GetItemQuantity(AmmoId);
}

FString UTPSInventoryComponent::BuildInventorySummary() const
{
	FString Summary = FString::Printf(TEXT("COINS: %d"), GetCoinCount());

	static const ETPSWeaponFamily Families[] = {
		ETPSWeaponFamily::Pistol,
		ETPSWeaponFamily::Rifle,
		ETPSWeaponFamily::Shotgun,
		ETPSWeaponFamily::Sniper,
	};

	for (ETPSWeaponFamily Family : Families)
	{
		const FName WeaponId = WeaponItemIdForWeaponFamily(Family);
		const int32 WeaponCount = GetItemQuantity(WeaponId);
		const int32 AmmoCount = GetAmmoForWeaponFamily(Family);

		if (WeaponCount > 0 || AmmoCount > 0)
		{
			const TCHAR* Label = TEXT("?");
			switch (Family)
			{
			case ETPSWeaponFamily::Pistol:  Label = TEXT("Pistol"); break;
			case ETPSWeaponFamily::Rifle:   Label = TEXT("Rifle"); break;
			case ETPSWeaponFamily::Shotgun: Label = TEXT("Shotgun"); break;
			case ETPSWeaponFamily::Sniper:  Label = TEXT("Sniper"); break;
			default: break;
			}

			if (WeaponCount > 0 && AmmoCount > 0)
			{
				Summary += FString::Printf(TEXT("\n%s (ammo %d)"), Label, AmmoCount);
			}
			else if (WeaponCount > 0)
			{
				Summary += FString::Printf(TEXT("\n%s"), Label);
			}
			else
			{
				Summary += FString::Printf(TEXT("\n%s ammo: %d"), Label, AmmoCount);
			}
		}
	}

	return Summary;
}

FName UTPSInventoryComponent::AmmoItemIdForWeaponFamily(ETPSWeaponFamily WeaponFamily)
{
	switch (WeaponFamily)
	{
	case ETPSWeaponFamily::Pistol:  return FName(TEXT("Ammo_Pistol"));
	case ETPSWeaponFamily::Rifle:   return FName(TEXT("Ammo_Rifle"));
	case ETPSWeaponFamily::Shotgun: return FName(TEXT("Ammo_Shotgun"));
	case ETPSWeaponFamily::Sniper:  return FName(TEXT("Ammo_Sniper"));
	default: return NAME_None;
	}
}

FName UTPSInventoryComponent::WeaponItemIdForWeaponFamily(ETPSWeaponFamily WeaponFamily)
{
	switch (WeaponFamily)
	{
	case ETPSWeaponFamily::Pistol:  return FName(TEXT("Weapon_Pistol"));
	case ETPSWeaponFamily::Rifle:   return FName(TEXT("Weapon_Rifle"));
	case ETPSWeaponFamily::Shotgun: return FName(TEXT("Weapon_Shotgun"));
	case ETPSWeaponFamily::Sniper:  return FName(TEXT("Weapon_Sniper"));
	default: return NAME_None;
	}
}
