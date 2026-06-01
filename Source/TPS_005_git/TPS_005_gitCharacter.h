// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TPS_005_gitCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UTPSPhysicsShooterComponent;
class UTPSWeaponComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class ATPS_005_gitCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UTPSPhysicsShooterComponent* PhysicsShooter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	UTPSWeaponComponent* WeaponComponent;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ShootAction;

	/** Runtime K->Shoot mapping (does not depend on IMC_Default asset). */
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> ShootInputMappingContext;

	/** Runtime weapon input actions (1-4 equip, R reload) — created without content assets. */
	UPROPERTY(Transient)
	TObjectPtr<UInputAction> EquipPistolAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> EquipRifleAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> EquipShotgunAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> EquipSniperAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> WeaponInputMappingContext;

public:

	/** Constructor */
	ATPS_005_gitCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;

	void EnsureShootInputMapping();
	void EnsureWeaponInputMapping();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void ShootPressed();

	void EquipPistolPressed();
	void EquipRiflePressed();
	void EquipShotgunPressed();
	void EquipSniperPressed();
	void ReloadPressed();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category="Shoot")
	void ShootProjectileFromTorso();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE UTPSPhysicsShooterComponent* GetPhysicsShooter() const { return PhysicsShooter; }
	FORCEINLINE UTPSWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
};

