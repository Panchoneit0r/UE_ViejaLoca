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
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	
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

