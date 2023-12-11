// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossbowBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

// Sets default values
ACrossbowBase::ACrossbowBase()
{

	//bReplicates = true;
	
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

	FirePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(RootComponent);
	
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


void ACrossbowBase::Shooting()
{
	
}

void ACrossbowBase::Roleded()
{
	Amunition = 1;
	arrowPoint->SetWorldScale3D(FVector(0.005f,0.005f,0.005f));
}



