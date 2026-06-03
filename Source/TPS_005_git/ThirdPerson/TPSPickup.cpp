// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "TPSInventoryComponent.h"
#include "TPSInventoryItemDefinition.h"
#include "TPS_005_git.h"
#include "UObject/ConstructorHelpers.h"

namespace TPSPickupPrivate
{
	static const AActor* AsValidOverlapActor(const UObject* Obj)
	{
		if (!IsValid(Obj) || Obj->HasAnyFlags(RF_ClassDefaultObject))
		{
			return nullptr;
		}

		if (!Obj->IsA(AActor::StaticClass()))
		{
			return nullptr;
		}

		return static_cast<const AActor*>(Obj);
	}
}

ATPSPickup::ATPSPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bGenerateOverlapEventsDuringLevelStreaming = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(80.0f);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMesh->SetupAttachment(RootComponent);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundAsset(
		TEXT("/Game/Interface_And_Item_Sounds/Cues/Coins_01_Cue.Coins_01_Cue"));
	if (PickupSoundAsset.Succeeded())
	{
		PickupSound = PickupSoundAsset.Object;
	}
}

bool ATPSPickup::IsAmmoPickup() const
{
	if (ItemDefinition && ItemDefinition->ItemType == ETPSInventoryItemType::Ammo)
	{
		return true;
	}

	if (FallbackItemType == ETPSInventoryItemType::Ammo)
	{
		return true;
	}

	return FallbackItemId.ToString().StartsWith(TEXT("Ammo_"));
}

void ATPSPickup::BeginAmmoRespawnCooldown()
{
	UWorld* World = GetWorld();
	if (!World || bAmmoRespawnPending)
	{
		return;
	}

	bAmmoRespawnPending = true;
	SetActorTickEnabled(false);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);

	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* Component : MeshComponents)
	{
		if (Component)
		{
			Component->SetVisibility(false, true);
			Component->SetHiddenInGame(true);
		}
	}

	UE_LOG(LogTPS_005_git, Log, TEXT("Ammo pickup %s hidden; respawn in %.1fs"), *GetName(), AmmoRespawnDelaySeconds);

	World->GetTimerManager().ClearTimer(AmmoRespawnTimerHandle);
	World->GetTimerManager().SetTimer(
		AmmoRespawnTimerHandle,
		this,
		&ATPSPickup::FinishAmmoRespawn,
		AmmoRespawnDelaySeconds,
		false);
}

void ATPSPickup::FinishAmmoRespawn()
{
	bAmmoRespawnPending = false;

	SetActorTransform(SpawnTransform);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Sphere->SetGenerateOverlapEvents(true);
	}

	ApplyWorldMesh();

	TArray<UStaticMeshComponent*> MeshComponents;
	GetComponents<UStaticMeshComponent>(MeshComponents);
	for (UStaticMeshComponent* Component : MeshComponents)
	{
		if (Component)
		{
			Component->SetVisibility(true, true);
			Component->SetHiddenInGame(false);
		}
	}

	UE_LOG(LogTPS_005_git, Log, TEXT("Ammo pickup %s respawned at %s"), *GetName(), *SpawnTransform.GetLocation().ToString());
}

void ATPSPickup::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();

	if (!FallbackItemId.IsNone() && FallbackItemId != FName(TEXT("Coin")))
	{
		const FRotator ActorRotation = GetActorRotation();
		SetActorRotation(FRotator(0.0f, ActorRotation.Yaw, 0.0f));

		if (Sphere)
		{
			Sphere->SetSphereRadius(120.0f);
		}
	}

	ApplyWorldMesh();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		OnActorBeginOverlap.AddUniqueDynamic(this, &ATPSPickup::BeginOverlap);
	}
}

void ATPSPickup::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AmmoRespawnTimerHandle);
	}

	OnActorBeginOverlap.RemoveDynamic(this, &ATPSPickup::BeginOverlap);
	Super::EndPlay(EndPlayReason);
}

void ATPSPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Pickup logic runs from BeginOverlap. Suppress Blueprint ReceiveActorBeginOverlap
	// so cast nodes cannot run on invalid partners during level streaming.
}

void ATPSPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	ApplyWorldMesh();
}

void ATPSPickup::ApplyWorldMesh()
{
	if (!PickupMesh)
	{
		return;
	}

	if (WorldMesh)
	{
		FRotator DisplayRotation = WorldMeshRotation;
		const FString ItemIdStr = FallbackItemId.IsNone() ? TEXT("") : FallbackItemId.ToString();
		if (DisplayRotation.IsZero() && ItemIdStr.StartsWith(TEXT("Ammo_")))
		{
			DisplayRotation = FRotator(90.0f, 0.0f, 0.0f);
		}

		FVector DisplayOffset = WorldMeshOffset;
		if (!FallbackItemId.IsNone() && FallbackItemId != FName(TEXT("Coin")))
		{
			if (ItemIdStr.StartsWith(TEXT("Ammo_")) && DisplayOffset.Z < 40.0f)
			{
				DisplayOffset.Z = 55.0f;
			}
			else if (ItemIdStr.StartsWith(TEXT("Weapon_")) && DisplayOffset.IsNearlyZero())
			{
				DisplayOffset = FVector(0.0f, 0.0f, 25.0f);
			}
		}

		TArray<UStaticMeshComponent*> MeshComponents;
		GetComponents<UStaticMeshComponent>(MeshComponents);
		for (UStaticMeshComponent* Component : MeshComponents)
		{
			if (Component && Component != PickupMesh)
			{
				Component->SetVisibility(false, true);
				Component->SetHiddenInGame(true);
			}
		}

		PickupMesh->SetStaticMesh(WorldMesh);
		PickupMesh->SetRelativeRotation(DisplayRotation);
		PickupMesh->SetRelativeLocation(DisplayOffset);
		PickupMesh->SetRelativeScale3D(WorldMeshScale);
		PickupMesh->SetVisibility(true, true);
		PickupMesh->SetHiddenInGame(false);
	}
	else
	{
		PickupMesh->SetStaticMesh(nullptr);
		PickupMesh->SetVisibility(false, true);
	}
}

void ATPSPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!WorldMesh)
	{
		AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
		return;
	}

	AddActorWorldRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

// Pickup feedback uses 2D + at-location audio for reliable playback.
USoundBase* ATPSPickup::ResolvePickupSound() const
{
	if (ItemDefinition && ItemDefinition->PickupSound)
	{
		return ItemDefinition->PickupSound;
	}

	if (PickupSound)
	{
		return PickupSound;
	}

	static ConstructorHelpers::FObjectFinder<USoundBase> DefaultPickupSound(
		TEXT("/Game/Interface_And_Item_Sounds/Cues/Coins_01_Cue.Coins_01_Cue"));
	return DefaultPickupSound.Succeeded() ? DefaultPickupSound.Object : nullptr;
}

void ATPSPickup::PlayPickupFeedback(APlayerController* PlayerController, USoundBase* SoundToPlay) const
{
	if (!SoundToPlay || !PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	UGameplayStatics::PlaySound2D(PlayerController, SoundToPlay);
	UGameplayStatics::PlaySoundAtLocation(
		PlayerController,
		SoundToPlay,
		GetActorLocation(),
		FRotator::ZeroRotator,
		1.0f,
		1.0f,
		0.0f,
		nullptr,
		nullptr,
		PlayerController);
}

void ATPSPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bAmmoRespawnPending)
	{
		return;
	}

	const AActor* ValidOther = TPSPickupPrivate::AsValidOverlapActor(OtherActor);
	if (!ValidOther || !Sphere || !Sphere->IsCollisionEnabled())
	{
		return;
	}

	const APawn* PlayerPawn = Cast<APawn>(ValidOther);
	if (!PlayerPawn)
	{
		const AActor* OtherInstigator = TPSPickupPrivate::AsValidOverlapActor(ValidOther->GetInstigator());
		if (OtherInstigator)
		{
			PlayerPawn = Cast<APawn>(OtherInstigator);
		}
	}

	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PC)
	{
		return;
	}

	ApplyPickup(PC);
}

void ATPSPickup::ApplyPickup(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	const bool bHasInventoryConfig = ItemDefinition != nullptr || !FallbackItemId.IsNone();
	bool bCollected = false;

	if (UTPSInventoryComponent* Inventory = PlayerController->FindComponentByClass<UTPSInventoryComponent>())
	{
		if (ItemDefinition)
		{
			const int32 Amount = QuantityOverride > 0 ? QuantityOverride : 0;
			bCollected = Inventory->AddItemFromDefinition(ItemDefinition, Amount);
		}
		else if (!FallbackItemId.IsNone())
		{
			const int32 Amount = QuantityOverride > 0 ? QuantityOverride : FallbackQuantity;
			ETPSInventoryItemType ItemType = FallbackItemType;
			const FString ItemIdStr = FallbackItemId.ToString();
			if (ItemIdStr.StartsWith(TEXT("Weapon_")))
			{
				ItemType = ETPSInventoryItemType::Weapon;
			}
			else if (ItemIdStr.StartsWith(TEXT("Ammo_")))
			{
				ItemType = ETPSInventoryItemType::Ammo;
			}
			else if (ItemIdStr == TEXT("Coin"))
			{
				ItemType = ETPSInventoryItemType::Currency;
			}
			bCollected = Inventory->AddItem(FallbackItemId, Amount, ItemType);
		}
	}

	if (bHasInventoryConfig && !bCollected)
	{
		return;
	}

	if (bCollected)
	{
		PlayPickupFeedback(PlayerController, ResolvePickupSound());

		if (IsAmmoPickup())
		{
			BeginAmmoRespawnCooldown();
			return;
		}

		SetActorEnableCollision(false);
		SetActorHiddenInGame(true);
		Destroy();
		return;
	}

	PlayPickupFeedback(PlayerController, ResolvePickupSound());

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	Destroy();
}
