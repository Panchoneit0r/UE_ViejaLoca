// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossbowBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ACrossbowBase::ACrossbowBase()
{

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	BoxComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = BoxComponent;
	
	//Definition for the Mesh that will serve as your visual representation.
	//	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	CrossbowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrossbowMesh"));
	CrossbowMesh->SetupAttachment(RootComponent);

	arrowPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	arrowPoint->SetupAttachment(RootComponent);
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACrossbowBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrossbowBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACrossbowBase::StartFire()
{
	if (Amunition > 0)
	{
		FVector spawnLocation = arrowPoint->GetComponentLocation();
		FRotator spawnRotation = GetActorRotation();

		FActorSpawnParameters spawnParameters;
		spawnParameters.Instigator = GetInstigator();
		spawnParameters.Owner = this;

		AActor* projectile = GetWorld()->SpawnActor<AActor>(ArrowClass, spawnLocation, spawnRotation, spawnParameters);
		Amunition--;
		if(Amunition > 0)
		{
	
		}
	}
	/*
     		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
     		if (AnimInstance != nullptr)
     		{
     			AnimInstance->Montage_Play(AttackAnim, 2.0f);
     		}
     		*/
     
     		/*
     		if (!bIsFiringWeapon)
     		{
     			bIsFiringWeapon = true;
     			UWorld* World = GetWorld();
     			World->GetTimerManager().SetTimer(FiringTimer, this, &ACrossbowBase::StopFire, false);
     			HandleFire();
     		}
     		*/
}

void ACrossbowBase::StopFire()
{
	bIsFiringWeapon = false;
}

void ACrossbowBase::Roleded()
{
	Amunition = 1;
	arrowPoint->SetVisibleInRayTracing(true);
}

void ACrossbowBase::HandleFire_Implementation()
{
	FVector spawnLocation = GetActorLocation() + ( GetActorRotation().Vector()  * 100.0f ) + (GetActorUpVector() * 50.0f);
	FRotator spawnRotation = GetActorRotation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.Instigator = GetInstigator();
	spawnParameters.Owner = this;

	/*
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr)
	{
		AnimInstance->Montage_Play(AttackAnim, 2.0f);
	}
	*/
	
	AActor* projectile = GetWorld()->SpawnActor<AActor>(ArrowClass, spawnLocation, spawnRotation, spawnParameters);
	
}


