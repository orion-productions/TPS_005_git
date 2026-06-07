// Copyright Epic Games, Inc. All Rights Reserved.
// Weapon equip/fire/reload/unequip � aim stance, recoil, dry-fire, coin hits.

#include "TPSWeaponComponent.h"
#include "TPSInventoryComponent.h"
#include "TPSPickup.h"
#include "TPS_005_gitCharacter.h"
#include "TPS_005_gitPlayerController.h"
#include "TPS_005_git.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

namespace TPSWeaponComponentPrivate
{
	template<typename TObjectType>
	TObjectType* LoadDefaultObject(const TCHAR* Path)
	{
		ConstructorHelpers::FObjectFinder<TObjectType> Finder(Path);
		return Finder.Succeeded() ? Finder.Object : nullptr;
	}

	void ApplyFamilyDefaults(FTPSWeaponFamilyConfig& Config,
		UAnimSequenceBase* AimStance,
		UAnimSequenceBase* DryFireAnim,
		float RecoilPitch,
		float RecoilYaw)
	{
		if (!Config.AimStanceMontage)
		{
			Config.AimStanceMontage = AimStance;
		}
		if (!Config.DryFireMontage)
		{
			Config.DryFireMontage = DryFireAnim;
		}
		if (Config.RecoilPitch <= 0.f)
		{
			Config.RecoilPitch = RecoilPitch;
		}
		if (Config.RecoilYawRandom <= 0.f)
		{
			Config.RecoilYawRandom = RecoilYaw;
		}
	}
}

UTPSWeaponComponent::UTPSWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	DefaultDryFireSound = TPSWeaponComponentPrivate::LoadDefaultObject<USoundBase>(
		TEXT("/Game/Interface_And_Item_Sounds/Cues/Click_01_Cue.Click_01_Cue"));

	UAnimSequenceBase* PistolAim = TPSWeaponComponentPrivate::LoadDefaultObject<UAnimSequenceBase>(
		TEXT("/Game/Characters/Mannequins/Anims/Pistol/MF_Pistol_Idle_ADS.MF_Pistol_Idle_ADS"));
	UAnimSequenceBase* RifleAim = TPSWeaponComponentPrivate::LoadDefaultObject<UAnimSequenceBase>(
		TEXT("/Game/Characters/Mannequins/Anims/Rifle/MF_Rifle_Idle_ADS.MF_Rifle_Idle_ADS"));
	UAnimSequenceBase* PistolDryFire = TPSWeaponComponentPrivate::LoadDefaultObject<UAnimSequenceBase>(
		TEXT("/Game/Characters/Mannequins/Anims/Pistol/MM_Pistol_DryFire.MM_Pistol_DryFire"));
	UAnimSequenceBase* RifleDryFire = TPSWeaponComponentPrivate::LoadDefaultObject<UAnimSequenceBase>(
		TEXT("/Game/Characters/Mannequins/Anims/Rifle/MM_Rifle_DryFire.MM_Rifle_DryFire"));

	TPSWeaponComponentPrivate::ApplyFamilyDefaults(PistolConfig, PistolAim, PistolDryFire, 0.35f, 0.06f);
	TPSWeaponComponentPrivate::ApplyFamilyDefaults(RifleConfig, RifleAim, RifleDryFire, 0.55f, 0.08f);
	TPSWeaponComponentPrivate::ApplyFamilyDefaults(ShotgunConfig, RifleAim, RifleDryFire, 1.1f, 0.12f);
	TPSWeaponComponentPrivate::ApplyFamilyDefaults(SniperConfig, RifleAim, RifleDryFire, 1.4f, 0.05f);
}

void UTPSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsPlayableOwnerCharacter())
	{
		return;
	}

	ACharacter* Char = GetOwnerCharacter();
	if (Char && Char->GetCharacterMovement())
	{
		DefaultMaxWalkSpeed = Char->GetCharacterMovement()->MaxWalkSpeed;
	}

	WeaponMeshComp = NewObject<UStaticMeshComponent>(Char, TEXT("WeaponMesh"));
	WeaponMeshComp->RegisterComponent();
	WeaponMeshComp->SetVisibility(false);
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComp->SetCastShadow(false);
}

void UTPSWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(EquipTimerHandle);
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
		World->GetTimerManager().ClearTimer(UnequipTimerHandle);
		World->GetTimerManager().ClearTimer(FireAnimTimerHandle);
	}

	if (IsPlayableOwnerCharacter())
	{
		ApplyAimMovementStance(false);
		StopAimStance();
	}

	EquippedWeapon = ETPSWeaponFamily::None;
	bIsReloading = false;
	bIsEquipping = false;
	DetachWeaponMesh();

	Super::EndPlay(EndPlayReason);
}

// ?? Equip ???????????????????????????????????????????????????????????????????????????

void UTPSWeaponComponent::EquipWeapon(ETPSWeaponFamily Family)
{
	if (!IsPlayableOwnerCharacter() || bIsEquipping || bIsReloading)
	{
		return;
	}

	if (EquippedWeapon == Family)
	{
		UnequipWeapon();
		return;
	}

	UTPSInventoryComponent* Inv = GetInventoryComponent();
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
		StopAimStance();
		DetachWeaponMesh();
	}

	EquippedWeapon = Family;
	bIsEquipping   = true;

	AttachWeaponMesh(*Config);
	ApplyAimMovementStance(true);

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
			OnEquipComplete();
		}
	}
	else
	{
		OnEquipComplete();
	}
}

void UTPSWeaponComponent::UnequipWeapon()
{
	if (EquippedWeapon == ETPSWeaponFamily::None)
	{
		return;
	}

	const FTPSWeaponFamilyConfig* Config = GetConfig(EquippedWeapon);

	GetWorld()->GetTimerManager().ClearTimer(EquipTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FireAnimTimerHandle);
	EquippedWeapon = ETPSWeaponFamily::None;
	bIsReloading   = false;
	bIsEquipping   = false;

	ApplyAimMovementStance(false);
	StopAimStance();

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

// ?? Fire ???????????????????????????????????????????????????????????????????????????

void UTPSWeaponComponent::FireWeapon()
{
	if (!IsPlayableOwnerCharacter() || EquippedWeapon == ETPSWeaponFamily::None || bIsEquipping)
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
		if (USoundBase* DrySound = ResolveDryFireSound(*Config))
		{
			UGameplayStatics::PlaySoundAtLocation(this, DrySound, GetOwner()->GetActorLocation());
		}

		if (Config->DryFireMontage)
		{
			PlayAnimOnOwner(Config->DryFireMontage);
		}

		return;
	}

	if (Inv)
	{
		Inv->ConsumeWeaponAmmo(EquippedWeapon, 1);
	}

	const float FireAnimDuration = PlayAnimOnOwner(Config->FireMontage);
	if (FireAnimDuration > 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireAnimTimerHandle);
		FTimerDelegate Delegate;
		Delegate.BindUObject(this, &UTPSWeaponComponent::OnFireAnimComplete);
		GetWorld()->GetTimerManager().SetTimer(FireAnimTimerHandle, Delegate, FireAnimDuration * 0.95f, false);
	}

	if (Config->FireSound)
	{
		if (const ACharacter* Char = GetOwnerCharacter())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Config->FireSound, Char->GetActorLocation());
		}
	}

	ApplyRecoil(*Config);

	ACharacter* Char = GetOwnerCharacter();
	if (Char)
	{
		if (APlayerController* PC = Cast<APlayerController>(Char->GetController()))
		{
			FVector CamLoc;
			FRotator CamRot;
			PC->GetPlayerViewPoint(CamLoc, CamRot);

			const FVector TraceEnd = CamLoc + CamRot.Vector() * FireTraceDistance;
			ProcessFireTrace(CamLoc, TraceEnd, PC);
		}
	}
}

void UTPSWeaponComponent::ProcessFireTrace(const FVector& TraceStart, const FVector& TraceEnd, APlayerController* PC)
{
	if (!PC || !GetWorld())
	{
		return;
	}

	FCollisionQueryParams Params(TEXT("WeaponFire"), false, GetOwnerCharacter());

	TArray<FHitResult> Hits;
	FCollisionObjectQueryParams ObjQuery;
	ObjQuery.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjQuery.AddObjectTypesToQuery(ECC_WorldStatic);
	GetWorld()->LineTraceMultiByObjectType(Hits, TraceStart, TraceEnd, ObjQuery, Params);

	ATPSPickup* ClosestPickup = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor) || HitActor->HasAnyFlags(RF_ClassDefaultObject))
		{
			continue;
		}

		ATPSPickup* Pickup = Cast<ATPSPickup>(HitActor);
		if (!Pickup)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared(TraceStart, Hit.ImpactPoint);
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestPickup = Pickup;
		}
	}

	if (ClosestPickup)
	{
		ClosestPickup->TryCollectByPlayer(PC);
		return;
	}

	FHitResult VisibilityHit;
	if (GetWorld()->LineTraceSingleByChannel(VisibilityHit, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		UE_LOG(LogTPS_005_git, Verbose, TEXT("[Weapon] Hit %s at %s"),
			*GetNameSafe(VisibilityHit.GetActor()), *VisibilityHit.ImpactPoint.ToString());
	}
}

// ?? Reload ??????????????????????????????????????????????????????????????????????????

void UTPSWeaponComponent::ReloadWeapon()
{
	if (!IsPlayableOwnerCharacter() || EquippedWeapon == ETPSWeaponFamily::None || bIsReloading || bIsEquipping)
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
	StopAimStance();

	if (Config->ReloadSound)
	{
		if (const ACharacter* Char = GetOwnerCharacter())
		{
			UGameplayStatics::PlaySoundAtLocation(this, Config->ReloadSound, Char->GetActorLocation());
		}
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

// ?? Helpers ??????????????????????????????????????????????????????????????????????????

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

bool UTPSWeaponComponent::IsPlayableOwnerCharacter() const
{
	const UObject* Outer = GetOuter();
	if (!IsValid(Outer) || Outer->HasAnyFlags(RF_ClassDefaultObject) || !Outer->IsA<ACharacter>())
	{
		return false;
	}

	const ACharacter* Char = static_cast<const ACharacter*>(Outer);
	const UWorld* World = Char->GetWorld();
	return World && World->IsGameWorld();
}

ACharacter* UTPSWeaponComponent::GetOwnerCharacter() const
{
	if (!IsPlayableOwnerCharacter())
	{
		return nullptr;
	}

	return static_cast<ACharacter*>(GetOuter());
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
	return PlayAnimOnOwnerSlot(AnimAsset, TEXT("DefaultSlot"), 1);
}

float UTPSWeaponComponent::PlayAnimOnOwnerSlot(UAnimSequenceBase* AnimAsset, FName SlotName, int32 LoopCount)
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

	UAnimMontage* DynMontage = AnimInst->PlaySlotAnimationAsDynamicMontage(
		AnimAsset, SlotName, 0.15f, 0.15f, 1.f, LoopCount);
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
	StartAimStance();
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
		StartAimStance();
	}
}

void UTPSWeaponComponent::OnFireAnimComplete()
{
	StartAimStance();
}

void UTPSWeaponComponent::StartAimStance()
{
	if (EquippedWeapon == ETPSWeaponFamily::None || bIsEquipping || bIsReloading)
	{
		return;
	}

	const FTPSWeaponFamilyConfig* Config = GetConfig(EquippedWeapon);
	if (!Config || !Config->AimStanceMontage)
	{
		return;
	}

	ACharacter* Char = GetOwnerCharacter();
	if (!Char)
	{
		return;
	}

	UAnimInstance* AnimInst = Char->GetMesh()->GetAnimInstance();
	if (!AnimInst)
	{
		return;
	}

	if (ActiveAimMontage)
	{
		AnimInst->Montage_Stop(0.15f, ActiveAimMontage);
		ActiveAimMontage = nullptr;
	}

	ActiveAimMontage = AnimInst->PlaySlotAnimationAsDynamicMontage(
		Config->AimStanceMontage, AimMontageSlotName, 0.2f, 0.2f, 1.f, 0);
}

void UTPSWeaponComponent::StopAimStance()
{
	ACharacter* Char = GetOwnerCharacter();
	if (Char)
	{
		if (UAnimInstance* AnimInst = Char->GetMesh()->GetAnimInstance())
		{
			if (ActiveAimMontage)
			{
				AnimInst->Montage_Stop(0.2f, ActiveAimMontage);
			}
		}
	}
	ActiveAimMontage = nullptr;
}

void UTPSWeaponComponent::ApplyAimMovementStance(bool bAiming)
{
	ACharacter* Char = GetOwnerCharacter();
	if (!Char)
	{
		return;
	}

	Char->bUseControllerRotationYaw = bAiming;
	if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = !bAiming;
		if (bAiming)
		{
			Move->MaxWalkSpeed = 320.f;
		}
		else
		{
			Move->MaxWalkSpeed = DefaultMaxWalkSpeed;
		}
	}

	if (ATPS_005_gitCharacter* TPSChar = Cast<ATPS_005_gitCharacter>(Char))
	{
		TPSChar->SetAimCameraActive(bAiming);
	}
}

void UTPSWeaponComponent::ApplyRecoil(const FTPSWeaponFamilyConfig& Config)
{
	ACharacter* Char = GetOwnerCharacter();
	if (!Char)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Char->GetController());
	if (!PC)
	{
		return;
	}

	const float PitchKick = Config.RecoilPitch;
	const float YawKick = FMath::FRandRange(-Config.RecoilYawRandom, Config.RecoilYawRandom);
	PC->AddPitchInput(-PitchKick);
	PC->AddYawInput(YawKick);
}

USoundBase* UTPSWeaponComponent::ResolveDryFireSound(const FTPSWeaponFamilyConfig& Config) const
{
	if (Config.DryFireSound)
	{
		return Config.DryFireSound;
	}
	return DefaultDryFireSound;
}
