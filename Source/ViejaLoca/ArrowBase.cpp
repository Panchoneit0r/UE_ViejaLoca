// Fill out your copyright notice in the Description page of Project Settings.


#include "ArrowBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AArrowBase::AArrowBase()
{
	bReplicates = true;
	
	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	//BoxComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = BoxComponent;
	
	//Definition for the Mesh that will serve as your visual representation.
	//	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ArrowMesh->SetupAttachment(RootComponent);

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetUpdatedComponent(BoxComponent);
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	
	//Damage = 10.0f;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AArrowBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArrowBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

