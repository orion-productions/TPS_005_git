// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPSPhysicsProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATPSPhysicsProjectile::ATPSPhysicsProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(
		TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
	}

	Mesh->SetWorldScale3D(FVector(0.45f));
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(true);
	Mesh->SetMassOverrideInKg(NAME_None, 8.0f, true);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}
