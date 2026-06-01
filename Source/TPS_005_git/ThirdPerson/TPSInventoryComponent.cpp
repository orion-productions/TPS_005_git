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

	if (ItemType == ETPSInventoryItemType::Ammo)
	{
		ReserveAmmo += AmmoPerPickup;
		OnInventoryChanged.Broadcast();
		return true;
	}

	if (ItemType == ETPSInventoryItemType::Weapon)
	{
		if (GetItemQuantity(ItemId) > 0)
		{
			return false;
		}

		ItemQuantities.Add(ItemId, 1);
		ItemTypes.Add(ItemId, static_cast<uint8>(ItemType));

		const ETPSWeaponFamily Family = WeaponFamilyFromItemId(ItemId);
		if (Family != ETPSWeaponFamily::None)
		{
			WeaponMagazineAmmo.Add(static_cast<uint8>(Family), GetMaxMagazineForWeapon(Family));
		}

		OnInventoryChanged.Broadcast();
		return true;
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
	return ReserveAmmo;
}

int32 UTPSInventoryComponent::GetWeaponMagazineAmmo(ETPSWeaponFamily WeaponFamily) const
{
	const int32* Rounds = WeaponMagazineAmmo.Find(static_cast<uint8>(WeaponFamily));
	return Rounds ? *Rounds : 0;
}

int32 UTPSInventoryComponent::GetMaxMagazineForWeapon(ETPSWeaponFamily WeaponFamily)
{
	switch (WeaponFamily)
	{
	case ETPSWeaponFamily::Pistol:  return 9;
	case ETPSWeaponFamily::Rifle:   return 18;
	case ETPSWeaponFamily::Shotgun: return 5;
	case ETPSWeaponFamily::Sniper:  return 3;
	default: return 0;
	}
}

bool UTPSInventoryComponent::ReloadWeaponMagazine(ETPSWeaponFamily WeaponFamily)
{
	if (!HasWeapon(WeaponFamily))
	{
		return false;
	}

	const int32 MaxRounds = GetMaxMagazineForWeapon(WeaponFamily);
	int32& Magazine = WeaponMagazineAmmo.FindOrAdd(static_cast<uint8>(WeaponFamily));
	const int32 Needed = MaxRounds - Magazine;
	if (Needed <= 0 || ReserveAmmo <= 0)
	{
		return false;
	}

	const int32 ToLoad = FMath::Min(Needed, ReserveAmmo);
	Magazine += ToLoad;
	ReserveAmmo -= ToLoad;
	OnInventoryChanged.Broadcast();
	return ToLoad > 0;
}

FString UTPSInventoryComponent::BuildInventorySummary() const
{
	FString Summary = FString::Printf(TEXT("Coins: %d\nAmmo: %d"), GetCoinCount(), ReserveAmmo);

	static const ETPSWeaponFamily Families[] = {
		ETPSWeaponFamily::Pistol,
		ETPSWeaponFamily::Rifle,
		ETPSWeaponFamily::Shotgun,
		ETPSWeaponFamily::Sniper,
	};

	for (ETPSWeaponFamily Family : Families)
	{
		const TCHAR* Label = TEXT("?");
		switch (Family)
		{
		case ETPSWeaponFamily::Pistol:  Label = TEXT("Pistol"); break;
		case ETPSWeaponFamily::Rifle:   Label = TEXT("Rifle"); break;
		case ETPSWeaponFamily::Shotgun: Label = TEXT("ShotGun"); break;
		case ETPSWeaponFamily::Sniper:  Label = TEXT("Sniper"); break;
		default: break;
		}

		if (HasWeapon(Family))
		{
			Summary += FString::Printf(TEXT("\n%s: %d"), Label, GetWeaponMagazineAmmo(Family));
		}
		else
		{
			Summary += FString::Printf(TEXT("\n%s"), Label);
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

bool UTPSInventoryComponent::ConsumeWeaponAmmo(ETPSWeaponFamily WeaponFamily, int32 Amount)
{
	const uint8 Key = static_cast<uint8>(WeaponFamily);
	int32* Current = WeaponMagazineAmmo.Find(Key);
	if (!Current || *Current <= 0)
	{
		return false;
	}
	*Current = FMath::Max(0, *Current - Amount);
	OnInventoryChanged.Broadcast();
	return true;
}

ETPSWeaponFamily UTPSInventoryComponent::WeaponFamilyFromItemId(FName ItemId)
{
	const FString Id = ItemId.ToString();
	if (Id == TEXT("Weapon_Pistol"))  { return ETPSWeaponFamily::Pistol; }
	if (Id == TEXT("Weapon_Rifle"))   { return ETPSWeaponFamily::Rifle; }
	if (Id == TEXT("Weapon_Shotgun")) { return ETPSWeaponFamily::Shotgun; }
	if (Id == TEXT("Weapon_Sniper"))  { return ETPSWeaponFamily::Sniper; }
	return ETPSWeaponFamily::None;
}
