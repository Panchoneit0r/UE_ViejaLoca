// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrossbowBase.generated.h"

UCLASS()
class VIEJALOCA_API ACrossbowBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACrossbowBase();

	// Sphere component used to test collision.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UBoxComponent* BoxComponent;

	// Static Mesh used to provide a visual representation of the object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	class UStaticMeshComponent* CrossbowMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Projectile")
	class UStaticMeshComponent* arrowPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	class UArrowComponent* FirePoint;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Projectile")
	TSubclassOf<class AActor> ArrowClass;

	/** Delay between shots in seconds. Used to control fire rate for your test projectile, but also to prevent an overflow of server functions from binding SpawnProjectile directly to input.*/
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float FireRate;

	/** If true, you are in the process of firing projectiles. */
	bool bIsFiringWeapon;

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle FiringTimer;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	int Amunition;

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void Roleded();
};
