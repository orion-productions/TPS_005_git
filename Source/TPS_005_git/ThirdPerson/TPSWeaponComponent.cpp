// Copyright Epic Games, Inc. All Rights Reserved.
// Weapon equip/fire/reload/unequip via PlaySlotAnimationAsDynamicMontage.

#include "TPSWeaponComponent.h"
#include "TPSInventoryComponent.h"
#include "TPS_005_gitPlayerController.h"
#include "TPS_005_git.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UTPSWeaponComponent::UTPSWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTPSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponMeshComp = NewObject<UStaticMeshComponent>(GetOwner(), TEXT("WeaponMesh"));
	WeaponMeshComp->RegisterComponent();
	WeaponMeshComp->SetVisibility(false);
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComp->SetCastShadow(false);
}

// â??â?? Equip â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??

void UTPSWeaponComponent::EquipWeapon(ETPSWeaponFamily Family)
{
	UE_LOG(LogTPS_005_git, Log, TEXT("[Weapon] EquipWeapon called: Family=%d bIsEquipping=%d bIsReloading=%d EquippedWeapon=%d"),
		(int32)Family, bIsEquipping, bIsReloading, (int32)EquippedWeapon);

	if (bIsEquipping || bIsReloading)
	{
		UE_LOG(LogTPS_005_git, Log, TEXT("[Weapon] EquipWeapon blocked: busy"));
		return;
	}

	if (EquippedWeapon == Family)
	{
		UnequipWeapon();
		return;
	}

	UTPSInventoryComponent* Inv = GetInventoryComponent();
	UE_LOG(LogTPS_005_git, Log, TEXT("[Weapon] Inventory=%s HasWeapon=%d"),
		Inv ? TEXT("valid") : TEXT("null"), Inv ? (int32)Inv->HasWeapon(Family) : -1);
	if (!Inv || !Inv->HasWeapon(Family))
	{
		return;
	}

	const FTPSWeaponFamilyConfig* Config = GetConfig(Family);
	if (!Config)
	{
		return;
	}

	if (EquippedWeapon != ETPSWeaponFamily::None)
	{
		GetWorld()->GetTimerManager().ClearTimer(UnequipTimerHandle);
		DetachWeaponMesh();
	}

	EquippedWeapon = Family;
	bIsEquipping   = true;

	AttachWeaponMesh(*Config);

	if (Config->EquipMontage)
	{
		const float Duration = PlayAnimOnOwner(Config->EquipMontage);
		if (Duration > 0.f)
		{
			FTimerDelegate Delegate;
			Delegate.BindUObject(this, &UTPSWeaponComponent::OnEquipComplete);
			GetWorld()->GetTimerManager().SetTimer(EquipTimerHandle, Delegate, Duration * 0.9f, false);
		}
		else
		{
			bIsEquipping = false;
		}
	}
	else
	{
		bIsEquipping = false;
	}
}

void UTPSWeaponComponent::UnequipWeapon()
{
	if (EquippedWeapon == ETPSWeaponFamily::None)
	{
		return;
	}

	const FTPSWeaponFamilyConfig* Config = GetConfig(EquippedWeapon);

	// Clear timers and block new actions
	GetWorld()->GetTimerManager().ClearTimer(EquipTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	EquippedWeapon = ETPSWeaponFamily::None;
	bIsReloading   = false;
	bIsEquipping   = false;

	// Play unequip anim, then hide mesh after it finishes
	if (Config && Config->UnequipMontage)
	{
		const float Duration = PlayAnimOnOwner(Config->UnequipMontage);
		if (Duration > 0.f)
		{
			FTimerDelegate Del;
			Del.BindUObject(this, &UTPSWeaponComponent::OnUnequipComplete);
			GetWorld()->GetTimerManager().SetTimer(UnequipTimerHandle, Del, Duration * 0.9f, false);
			return;
		}
	}
	OnUnequipComplete();
}

void UTPSWeaponComponent::OnUnequipComplete()
{
	DetachWeaponMesh();
	ACharacter* Char = GetOwnerCharacter();
	if (Char)
	{
		if (UAnimInstance* AnimInst = Char->GetMesh()->GetAnimInstance())
		{
			AnimInst->StopAllMontages(0.25f);
		}
	}
}

// â??â?? Fire â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??

void UTPSWeaponComponent::FireWeapon()
{
	if (EquippedWeapon == ETPSWeaponFamily::None || bIsEquipping)
	{
		return;
	}

	const FTPSWeaponFamilyConfig* Config = GetConfig(EquippedWeapon);
	if (!Config)
	{
		return;
	}

	UTPSInventoryComponent* Inv = GetInventoryComponent();
	const int32 MagAmmo = Inv ? Inv->GetWeaponMagazineAmmo(EquippedWeapon) : 0;

	if (MagAmmo <= 0)
	{
		if (Config->DryFireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, Config->DryFireSound, GetOwner()->GetActorLocation());
		}
		return;
	}

	if (Inv)
	{
		Inv->ConsumeWeaponAmmo(EquippedWeapon, 1);
	}

	PlayAnimOnOwner(Config->FireMontage);

	if (Config->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Config->FireSound, GetOwner()->GetActorLocation());
	}

	ACharacter* Char = GetOwnerCharacter();
	if (Char)
	{
		if (APlayerController* PC = Cast<APlayerController>(Char->GetController()))
		{
			FVector CamLoc;
			FRotator CamRot;
			PC->GetPlayerViewPoint(CamLoc, CamRot);

			const FVector TraceEnd = CamLoc + CamRot.Vector() * FireTraceDistance;

			FHitResult Hit;
			FCollisionQueryParams Params(TEXT("WeaponFire"), false, GetOwner());
			if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
			{
				UE_LOG(LogTPS_005_git, Verbose, TEXT("[Weapon] Hit %s at %s"),
					*GetNameSafe(Hit.GetActor()), *Hit.ImpactPoint.ToString());
			}
		}
	}
}

// â??â?? Reload â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??

void UTPSWeaponComponent::ReloadWeapon()
{
	if (EquippedWeapon == ETPSWeaponFamily::None || bIsReloading || bIsEquipping)
	{
		return;
	}

	UTPSInventoryComponent* Inv = GetInventoryComponent();
	if (!Inv || Inv->GetReserveAmmo() <= 0)
	{
		return;
	}

	const int32 MaxMag = UTPSInventoryComponent::GetMaxMagazineForWeapon(EquippedWeapon);
	if (Inv->GetWeaponMagazineAmmo(EquippedWeapon) >= MaxMag)
	{
		return;
	}

	const FTPSWeaponFamilyConfig* Config = GetConfig(EquippedWeapon);
	if (!Config)
	{
		return;
	}

	bIsReloading = true;

	if (Config->ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, Config->ReloadSound, GetOwner()->GetActorLocation());
	}

	if (Config->ReloadMontage)
	{
		const float Duration = PlayAnimOnOwner(Config->ReloadMontage);
		if (Duration > 0.f)
		{
			ScheduleReloadComplete(Duration);
		}
		else
		{
			OnReloadComplete();
		}
	}
	else
	{
		OnReloadComplete();
	}
}

// â??â?? Helpers â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??â??

const FTPSWeaponFamilyConfig* UTPSWeaponComponent::GetConfig(ETPSWeaponFamily Family) const
{
	switch (Family)
	{
		case ETPSWeaponFamily::Pistol:  return &PistolConfig;
		case ETPSWeaponFamily::Rifle:   return &RifleConfig;
		case ETPSWeaponFamily::Shotgun: return &ShotgunConfig;
		case ETPSWeaponFamily::Sniper:  return &SniperConfig;
		default:                        return nullptr;
	}
}

ACharacter* UTPSWeaponComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

UTPSInventoryComponent* UTPSWeaponComponent::GetInventoryComponent() const
{
	const ACharacter* Char = GetOwnerCharacter();
	if (!Char)
	{
		return nullptr;
	}
	const ATPS_005_gitPlayerController* PC = Cast<ATPS_005_gitPlayerController>(Char->GetController());
	return PC ? PC->GetInventoryComponent() : nullptr;
}

void UTPSWeaponComponent::AttachWeaponMesh(const FTPSWeaponFamilyConfig& Config)
{
	if (!WeaponMeshComp)
	{
		return;
	}
	ACharacter* Char = GetOwnerCharacter();
	if (!Char)
	{
		return;
	}

	WeaponMeshComp->SetStaticMesh(Config.WeaponMesh);
	WeaponMeshComp->AttachToComponent(
		Char->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		HandSocketName);
	WeaponMeshComp->SetRelativeLocationAndRotation(Config.MeshRelativeOffset, Config.MeshRelativeRotation);
	// Apply configured scale; fall back to 0.3 for Weapons_Free meshes (they're very large relative to Manny).
	// After a full editor restart the MeshRelativeScale Blueprint property will be tunable per weapon.
	FVector Scale = Config.MeshRelativeScale;
	if (Scale.Equals(FVector::OneVector, 0.001f))
	{
		Scale = FVector(0.3f);
	}
	WeaponMeshComp->SetRelativeScale3D(Scale);
	WeaponMeshComp->SetVisibility(true);
}

void UTPSWeaponComponent::DetachWeaponMesh()
{
	if (!WeaponMeshComp)
	{
		return;
	}
	WeaponMeshComp->SetVisibility(false);
	WeaponMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

float UTPSWeaponComponent::PlayAnimOnOwner(UAnimSequenceBase* AnimAsset)
{
	if (!AnimAsset)
	{
		return 0.f;
	}
	ACharacter* Char = GetOwnerCharacter();
	if (!Char)
	{
		return 0.f;
	}

	if (UAnimMontage* Montage = Cast<UAnimMontage>(AnimAsset))
	{
		return Char->PlayAnimMontage(Montage);
	}

	UAnimInstance* AnimInst = Char->GetMesh()->GetAnimInstance();
	if (!AnimInst)
	{
		return 0.f;
	}
	UAnimMontage* DynMontage = AnimInst->PlaySlotAnimationAsDynamicMontage(AnimAsset, TEXT("DefaultSlot"), 0.15f, 0.15f);
	return DynMontage ? DynMontage->GetPlayLength() : 0.f;
}

void UTPSWeaponComponent::ScheduleReloadComplete(float Duration)
{
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UTPSWeaponComponent::OnReloadComplete);
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, Delegate, Duration * 0.9f, false);
}

void UTPSWeaponComponent::OnEquipComplete()
{
	bIsEquipping = false;
}

void UTPSWeaponComponent::OnReloadComplete()
{
	bIsReloading = false;
	if (EquippedWeapon != ETPSWeaponFamily::None)
	{
		if (UTPSInventoryComponent* Inv = GetInventoryComponent())
		{
			Inv->ReloadWeaponMagazine(EquippedWeapon);
		}
	}
}
