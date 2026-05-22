// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPhysicsShooterComponent.h"
#include "TPSPhysicsProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UTPSPhysicsShooterComponent::UTPSPhysicsShooterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DefaultProjectileClass = ATPSPhysicsProjectile::StaticClass();
}

AActor* UTPSPhysicsShooterComponent::ShootObject(
	TSubclassOf<AActor> ProjectileClass,
	bool bUseCustomTransform,
	FVector SpawnLocation,
	FRotator SpawnRotation,
	float LaunchSpeed)
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return nullptr;
	}

	const TSubclassOf<AActor> ClassToSpawn = ProjectileClass ? ProjectileClass : DefaultProjectileClass;
	if (!ClassToSpawn)
	{
		return nullptr;
	}

	FVector Location = SpawnLocation;
	FRotator Rotation = SpawnRotation;

	if (!bUseCustomTransform)
	{
		if (const UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>())
		{
			Location = Camera->GetComponentLocation();
			Rotation = Camera->GetComponentRotation();
		}
		else if (const APawn* Pawn = Cast<APawn>(Owner))
		{
			if (const APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
			{
				PC->GetPlayerViewPoint(Location, Rotation);
			}
			else
			{
				Location = Pawn->GetActorLocation();
				Rotation = Pawn->GetActorRotation();
			}
		}
	}

	const float Speed = LaunchSpeed > 0.0f ? LaunchSpeed : DefaultLaunchSpeed;

	FActorSpawnParameters Params;
	Params.Owner = Owner;
	Params.Instigator = Cast<APawn>(Owner);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Projectile = World->SpawnActor<AActor>(ClassToSpawn, Location, Rotation, Params);
	if (!Projectile)
	{
		return nullptr;
	}

	UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
	if (!Primitive)
	{
		Primitive = Projectile->FindComponentByClass<UPrimitiveComponent>();
	}

	if (Primitive)
	{
		Primitive->SetSimulatePhysics(true);
		Primitive->SetEnableGravity(true);
		Primitive->SetGenerateOverlapEvents(true);
		Primitive->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		Primitive->IgnoreActorWhenMoving(Owner, true);
		Primitive->AddImpulse(Rotation.Vector() * Speed, NAME_None, true);
	}

	return Projectile;
}
