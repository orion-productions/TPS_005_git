// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSInventoryTypes.h"
#include "TPSWeaponComponent.generated.h"

class UAnimMontage;
class UAnimSequenceBase;
class USoundBase;
class UStaticMesh;
class UStaticMeshComponent;
class UTPSInventoryComponent;

/** Per-weapon-family configuration — assign meshes, animations, and sounds in the Blueprint. */
USTRUCT(BlueprintType)
struct FTPSWeaponFamilyConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UStaticMesh> WeaponMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UAnimSequenceBase> EquipMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UAnimSequenceBase> FireMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UAnimSequenceBase> ReloadMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<USoundBase> FireSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<USoundBase> ReloadSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<USoundBase> DryFireSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UAnimSequenceBase> AimStanceMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UAnimSequenceBase> DryFireMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="0.0"))
	float RecoilPitch = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="0.0"))
	float RecoilYawRandom = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FVector MeshRelativeOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FRotator MeshRelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FVector MeshRelativeScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<UAnimSequenceBase> UnequipMontage = nullptr;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TPS_005_GIT_API UTPSWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTPSWeaponComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void EquipWeapon(ETPSWeaponFamily Family);

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void UnequipWeapon();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void FireWeapon();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void ReloadWeapon();

	UFUNCTION(BlueprintPure, Category="Weapon")
	ETPSWeaponFamily GetEquippedWeapon() const { return EquippedWeapon; }

	UFUNCTION(BlueprintPure, Category="Weapon")
	bool IsWeaponEquipped() const { return EquippedWeapon != ETPSWeaponFamily::None; }

	UFUNCTION(BlueprintPure, Category="Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintPure, Category="Weapon")
	bool IsEquipping() const { return bIsEquipping; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Config")
	FTPSWeaponFamilyConfig PistolConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Config")
	FTPSWeaponFamilyConfig RifleConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Config")
	FTPSWeaponFamilyConfig ShotgunConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Config")
	FTPSWeaponFamilyConfig SniperConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FName HandSocketName = TEXT("hand_r");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon", meta=(ClampMin="100.0"))
	float FireTraceDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TObjectPtr<USoundBase> DefaultDryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	FName AimMontageSlotName = TEXT("DefaultSlot");

private:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> WeaponMeshComp;

	ETPSWeaponFamily EquippedWeapon = ETPSWeaponFamily::None;
	bool bIsReloading = false;
	bool bIsEquipping = false;
	float DefaultMaxWalkSpeed = 500.f;

	FTimerHandle EquipTimerHandle;
	FTimerHandle ReloadTimerHandle;
	FTimerHandle UnequipTimerHandle;
	FTimerHandle FireAnimTimerHandle;

	const FTPSWeaponFamilyConfig* GetConfig(ETPSWeaponFamily Family) const;
	bool IsPlayableOwnerCharacter() const;
	ACharacter* GetOwnerCharacter() const;
	UTPSInventoryComponent* GetInventoryComponent() const;

	void AttachWeaponMesh(const FTPSWeaponFamilyConfig& Config);
	void DetachWeaponMesh();
	float PlayAnimOnOwner(UAnimSequenceBase* AnimAsset);
	float PlayAnimOnOwnerSlot(UAnimSequenceBase* AnimAsset, FName SlotName, int32 LoopCount = 1);
	void ScheduleReloadComplete(float Duration);

	void OnEquipComplete();
	void OnReloadComplete();
	void OnUnequipComplete();
	void OnFireAnimComplete();

	void StartAimStance();
	void StopAimStance();
	void ApplyAimMovementStance(bool bAiming);
	void ApplyRecoil(const FTPSWeaponFamilyConfig& Config);
	USoundBase* ResolveDryFireSound(const FTPSWeaponFamilyConfig& Config) const;
	void ProcessFireTrace(const FVector& TraceStart, const FVector& TraceEnd, APlayerController* PC);
};
