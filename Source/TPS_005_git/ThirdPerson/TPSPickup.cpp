// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPickup.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TPS_005_gitPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ATPSPickup::ATPSPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(80.0f);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &ATPSPickup::BeginOverlap);

	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundAsset(
		TEXT("/Game/Interface_And_Item_Sounds/Cues/Coins_01_Cue.Coins_01_Cue"));
	if (PickupSoundAsset.Succeeded())
	{
		PickupSound = PickupSoundAsset.Object;
	}
}

void ATPSPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
}

void ATPSPickup::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor || !Sphere || !Sphere->IsCollisionEnabled())
	{
		return;
	}

	// Player walking in, or physics ball (credits player via instigator set on spawn).
	const APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (!PlayerPawn)
	{
		PlayerPawn = Cast<APawn>(OtherActor->GetInstigator());
	}

	if (!PlayerPawn || !PlayerPawn->IsPlayerControlled())
	{
		return;
	}

	ATPS_005_gitPlayerController* PC = Cast<ATPS_005_gitPlayerController>(PlayerPawn->GetController());
	if (!PC)
	{
		return;
	}

	PC->CoinCount++;

	if (PickupSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PickupSound);
	}

	SetActorEnableCollision(false);
	BP_OnPickedUp();
}
