// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightC.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AKnightC::AKnightC()
{
	// Character doesnt have a rifle at start
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AKnightC::BeginPlay()
{
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	FVector spawnLocation = GetActorLocation();
	FRotator spawnRotation = GetActorRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	Crossbow = GetWorld()->SpawnActor<ACrossbowBase>(CrossbowClass, spawnLocation, spawnRotation, spawnParameters);

	const FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules( EAttachmentRule::SnapToTarget, true );
	Crossbow->AttachToComponent(GetMesh(),AttachmentTransformRules, "CrossbowSocket");

	FirstPersonCameraComponent->AttachToComponent(GetMesh(),AttachmentTransformRules, "CameraSocket");
	
}

void AKnightC::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AKnightC::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AKnightC::ChangeWeapon()
{
}

void AKnightC::Reloded()
{
	Crossbow->Roleded();
}

void AKnightC::Shot()
{
	if (!Crossbow->bIsFiringWeapon)
	{
		Crossbow->bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AKnightC::StopFire, Crossbow->FireRate,false);
		//Crossbow->Shooting();
		FireServer();
	}
	
}

// Called every frame
void AKnightC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AKnightC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKnightC::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKnightC::Look);

		//Shot
		EnhancedInputComponent->BindAction(ShotAction, ETriggerEvent::Triggered, this, &AKnightC::Shot);

		//Reloded
		EnhancedInputComponent->BindAction(RelodedAction, ETriggerEvent::Triggered, this, &AKnightC::Reloded);

		//Change
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Completed, this, &AKnightC::ChangeWeapon);
	}
}

void AKnightC::StopFire()
{
	Crossbow->bIsFiringWeapon = false;
}

void AKnightC::FireServer_Implementation()
{
	if (Crossbow->Amunition > 0)
	{
		FVector spawnLocation = Crossbow->GetActorLocation();
		FRotator spawnRotation = GetActorRotation();

		FActorSpawnParameters spawnParameters;
		spawnParameters.Instigator = GetInstigator();
		spawnParameters.Owner = this;

		AActor* projectile = GetWorld()->SpawnActor<AActor>(Crossbow->ArrowClass, spawnLocation, spawnRotation, spawnParameters);
		Crossbow->Amunition--;
		if(Crossbow->Amunition < 1)
		{
			Crossbow->arrowPoint->SetWorldScale3D(FVector(0.0f,0.0f,0.0f));
		}
	}
}

