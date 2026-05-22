// Copyright Epic Games, Inc. All Rights Reserved.

#include "TPS_005_gitCharacter.h"
#include "ThirdPerson/TPSPhysicsShooterComponent.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "TPS_005_git.h"
#include "UObject/ConstructorHelpers.h"

ATPS_005_gitCharacter::ATPS_005_gitCharacter()
{
	// fn_vJulienMerceron_AddTwoVectors
	const FVector TestSum = FTwoVectors::fn_vJulienMerceron_AddTwoVectors(FVector(1, 2, 3), FVector(4, 5, 6));
	ensure(TestSum == FVector(5, 7, 9));

	// fn_vJulienMerceron_SubtractVectors
	const FVector TestDiff = FTwoVectors::fn_vJulienMerceron_SubtractVectors(FVector(5, 7, 9), FVector(4, 5, 6));
	ensure(TestDiff == FVector(1, 2, 3));

	// fn_vJulienMerceron_ScaleVectors
	const FTwoVectors TestScaled = FTwoVectors::fn_vJulienMerceron_ScaleVectors(FVector(1, 2, 3), FVector(4, 5, 6), 2.0f);
	ensure(TestScaled.v1 == FVector(2, 4, 6) && TestScaled.v2 == FVector(8, 10, 12));

	// fn_vJulienMerceron_DotProducts
	const FVector2D TestDots = FTwoVectors::fn_vJulienMerceron_DotProducts(FVector(1, 0, 0), FVector(0, 1, 0), FVector(1, 0, 0));
	ensure(FMath::IsNearlyEqual(TestDots.X, 1.0) && FMath::IsNearlyEqual(TestDots.Y, 0.0));

	// fn_vJulienMerceron_CrossProducts: (1,0,0)x(0,0,1)=(0,-1,0), (0,1,0)x(0,0,1)=(1,0,0)
	const FTwoVectors TestCross = FTwoVectors::fn_vJulienMerceron_CrossProducts(FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	ensure(TestCross.v1.Equals(FVector(0, -1, 0)) && TestCross.v2.Equals(FVector(1, 0, 0)));

	// fn_vJulienMerceron_Midpoint
	const FVector TestMid = FTwoVectors::fn_vJulienMerceron_Midpoint(FVector(0, 0, 0), FVector(2, 4, 6));
	ensure(TestMid == FVector(1, 2, 3));

	// fn_vJulienMerceron_Direction
	const FVector TestDir = FTwoVectors::fn_vJulienMerceron_Direction(FVector(0, 0, 0), FVector(3, 0, 0));
	ensure(TestDir.Equals(FVector(1, 0, 0)));

	// fn_vJulienMerceron_Length: 3-4-5 right triangle -> distance = 5
	const FVector::FReal TestLen = FTwoVectors::fn_vJulienMerceron_Length(FVector(0, 0, 0), FVector(3, 4, 0));
	ensure(FMath::IsNearlyEqual(TestLen, 5.0));

	// fn_vJulienMerceron_Lerp
	const FVector TestLerp = FTwoVectors::fn_vJulienMerceron_Lerp(FVector(0, 0, 0), FVector(10, 10, 10), 0.5f);
	ensure(TestLerp.Equals(FVector(5, 5, 5)));

	// fn_vJulienMerceron_IsWithinCone: B=(1,0.5,0) is within 45 deg of X-axis, B=(0,1,0) is not
	ensure( FTwoVectors::fn_vJulienMerceron_IsWithinCone(FVector(1, 0, 0), FVector(1, 0.5f, 0), FVector(1, 0, 0), 45.0f));
	ensure(!FTwoVectors::fn_vJulienMerceron_IsWithinCone(FVector(1, 0, 0), FVector(0, 1,    0), FVector(1, 0, 0), 45.0f));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PhysicsShooter = CreateDefaultSubobject<UTPSPhysicsShooterComponent>(TEXT("PhysicsShooter"));

	static ConstructorHelpers::FObjectFinder<UInputAction> ShootActionAsset(
		TEXT("/Game/Input/Actions/IA_Shoot.IA_Shoot"));
	if (ShootActionAsset.Succeeded())
	{
		ShootAction = ShootActionAsset.Object;
	}

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ATPS_005_gitCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	EnsureShootInputMapping();
}

void ATPS_005_gitCharacter::EnsureShootInputMapping()
{
	if (!ShootAction)
	{
		ShootAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Shoot.IA_Shoot"));
	}

	if (!ShootAction)
	{
		return;
	}

	const APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
	{
		return;
	}

	if (!ShootInputMappingContext)
	{
		ShootInputMappingContext = NewObject<UInputMappingContext>(this, TEXT("ShootInputMappingContext"));
		ShootInputMappingContext->MapKey(ShootAction, EKeys::K);
	}

	Subsystem->AddMappingContext(ShootInputMappingContext, 1);
}

void ATPS_005_gitCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (!ShootAction)
	{
		ShootAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Actions/IA_Shoot.IA_Shoot"));
	}

	EnsureShootInputMapping();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATPS_005_gitCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATPS_005_gitCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATPS_005_gitCharacter::Look);

		if (ShootAction)
		{
			EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATPS_005_gitCharacter::ShootPressed);
		}
	}
	else
	{
		UE_LOG(LogTPS_005_git, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ATPS_005_gitCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATPS_005_gitCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATPS_005_gitCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATPS_005_gitCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATPS_005_gitCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void ATPS_005_gitCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void ATPS_005_gitCharacter::ShootPressed()
{
	ShootProjectileFromTorso();
}

void ATPS_005_gitCharacter::ShootProjectileFromTorso()
{
	if (!PhysicsShooter)
	{
		return;
	}

	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	const float TorsoZ = Capsule ? Capsule->GetScaledCapsuleHalfHeight() * 0.35f : 50.0f;
	const FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, TorsoZ);

	PhysicsShooter->ShootObject(nullptr, true, SpawnLocation, GetActorRotation());
}
