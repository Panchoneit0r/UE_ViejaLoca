// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightC.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AKnightC::AKnightC():
CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete)),
JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
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

	if(CrossbowClass != nullptr)
	{
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
		
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.f,
				FColor::Purple,
				FString::Printf(TEXT("Found Subsystem: %s"), *OnlineSubsystem->GetSubsystemName().ToString())
			);
		}
	}
	
}

void AKnightC::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr && !death)
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
	if( !death)
	{
		Crossbow->Roleded();
	}
}

void AKnightC::Shot()
{
	if (!Crossbow->bIsFiringWeapon && !death)
	{
		Crossbow->bIsFiringWeapon = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &AKnightC::StopFire, Crossbow->FireRate,false);
		//Crossbow->Shooting();
		FireServer();
	}
	
}

void AKnightC::ChangeCamera(const FInputActionValue& Value)
{
	if(death)
	{
		float inputValue = Value.Get<float>();
		actualCamera+= inputValue;
		if(actualCamera > 3)
		{
			actualCamera = 0;
		}
		else if(actualCamera < 0)
		{
			actualCamera = 3;
		}
		APlayerController* PlayerController = Cast<APlayerController>(Controller);

		PlayerController->SetViewTargetWithBlend(Cameras[actualCamera], 0.3f);
	}
}

void AKnightC::Death()
{
	death = true;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		AnimInstance->Montage_Play(DeathAnim, 1.0f);
	}
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(DeathTimer, this, &AKnightC::DeathSystem, 1.8f,false);
	
}

void AKnightC::Respawn()
{
	death = false;
	setCurrentHealth(maxHealth);
	
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		PlayerController->SetViewTargetWithBlend(this);
	}
	
	SetActorLocation(FVector(0.0f,0.0f,0.0f));
}

void AKnightC::DeathSystem()
{
	SetActorLocation(FVector(1500.0f,1500.0f,0.0f));
	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		PlayerController->SetViewTargetWithBlend(Cameras[0]);
	}
	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(RespawnTimer, this, &AKnightC::Respawn, 10.0f,false);
}

void AKnightC::setCurrentHealth(float newHealth)
{
	currentHealth = newHealth;
	OnHealthUpdate();
}

void AKnightC::setCameras(TArray<AActor*> newCameras)
{
	Cameras = newCameras;
}

void AKnightC::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void AKnightC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(AKnightC, currentHealth);
}

void AKnightC::OnHealthUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), currentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

		if (currentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
		}
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), currentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
	}

	
	if (currentHealth <= 0)
	{
		Death();
	}
}

void AKnightC::Damaged(float _damage)
{
	if(!death)
	{
		currentHealth -= _damage;
		setCurrentHealth(currentHealth);
	
		/*if(currentHealth <= 0.0f)
		{
			DeathSystem();
		}
		*/
	}
}

void AKnightC::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	//Add the Dalegate on DelegateList
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	//Create Session
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	SessionSettings->bIsLANMatch = false;
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->Set(FName("MatchType"), FString("TeamMach"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
	
}

void AKnightC::JoinGameSession()
{
	if(!OnlineSessionInterface)
	{
		return;
	}

	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->bIsLanQuery = false;
	//SessionSearch->QuerySettings.Set(Presence, true, EOnlineComparisonOp::Equals);

	
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void AKnightC::OnCreateSessionComplete(FName SessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Created Session %s"),*SessionName.ToString())
			);
		}

		UWorld* World = GetWorld();
		if(World)
		{
			World->ServerTravel(MapRoute);
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Red,
					FString::Printf(TEXT("No world"))
				);
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Blue,
			FString::Printf(TEXT("Created Session Failed"))
		);
	}
}

void AKnightC::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface)
	{
		return;
	}

	
	for(auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		//Debug
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Orange,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
			);
		}

		if(MatchType == FString("TeamMach"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Orange,
					FString::Printf(TEXT("Joining MatchType %s"), *MatchType)
				);
			}

			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}

void AKnightC::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	FString Address;

	if(OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Connect to: %s"), *Address)
			);
			
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if(PlayerController)
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
}

void AKnightC::setMapRoute(FString NewMapRoute)
{
	MapRoute = NewMapRoute;
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

		//ChangeCamera
		EnhancedInputComponent->BindAction(ChangeCameraAction, ETriggerEvent::Started, this, &AKnightC::ChangeCamera);
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
		//FVector spawnLocation = Crossbow->GetActorLocation();
		FVector spawnLocation = GetActorLocation() + ( GetActorRotation().Vector()  * 100.0f ) + (GetActorUpVector() * 50.0f);
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

