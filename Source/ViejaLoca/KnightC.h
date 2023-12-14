// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrossbowBase.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "KnightC.generated.h"

UCLASS(config=Game)
class VIEJALOCA_API AKnightC : public ACharacter
{
	GENERATED_BODY()
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly,Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ShotAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* RelodedAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ChangeCameraAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Death, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> Cameras;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Death, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> RespawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadedAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathAnim;

public:
	// Sets default values for this character's properties
	AKnightC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeWeapon();

	void Reload();

	void Shot();

	void ChangeCamera(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	IOnlineSessionPtr OnlineSessionInterface;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	UFUNCTION(BlueprintCallable)
	void Damaged(float _damage);


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay")
	bool CarryingItem = false;

	FORCEINLINE float GetCarryingItem() const{return  CarryingItem;}

	UFUNCTION(BlueprintCallable)
	void setCarryingItem(bool _CarryingItem);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay")
	float Points;

	FORCEINLINE float GetPoints() const{return  Points;}

	UFUNCTION(BlueprintCallable)
	void setPoints(float _Points);
	
protected:
	
	//UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Crossbow")
	bool death = false;

	FORCEINLINE float GetDeath() const{return  death;}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Crossbow")
	float actualCamera = 0.0f;

	FORCEINLINE float GetActualCamera() const{return  actualCamera;}

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float currentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	float maxHealth;

	FORCEINLINE float GetCurrentHealth() const{return  currentHealth;}

	FORCEINLINE float GetMaxHealth() const{return  maxHealth;}

	UFUNCTION(BlueprintCallable)
	void setCurrentHealth(float newHealth);
	
	UFUNCTION(BlueprintCallable)
	void setCameras(TArray<AActor*> newCameras);

	UFUNCTION(BlueprintCallable)
	void setRespawnPoints(TArray<FVector> RPoints);

	/** RepNotify for changes made to current health.*/
	UFUNCTION()
	void OnRep_CurrentHealth();
	
	/** Property replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Response to health being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
	void OnHealthUpdate();

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle DeathTimer;

	FTimerHandle RespawnTimer;
	
	UFUNCTION(BlueprintCallable)
	void Death();

	UFUNCTION(BlueprintCallable)
	void Respawn();

	UFUNCTION(Blueprintable)
	void DeathSystem();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Crossbow")
	TSubclassOf<class ACrossbowBase> CrossbowClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Crossbow")
	class ACrossbowBase* Crossbow;

	/** Function for ending weapon fire. Once this is called, the player can use StartFire again.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle FiringTimer;

	UFUNCTION(Server,Reliable)
	void FireServer();

	UFUNCTION(NetMulticast,Reliable)
	void FireClient();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="Gameplay|Crossbow")
	bool Reloaded;
	
	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle ReloadTimer;

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable)
	void ServerReload();
	
	UFUNCTION(NetMulticast, Reliable)
	void ClientReload();
		
	void StopReload();	

protected:
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

	//Callbacks
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccess);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = online, meta = (AllowPrivateAccess = "true"))
	FString MapRoute;

	UFUNCTION(BlueprintCallable)
	void setMapRoute(FString NewMapRoute);

private:
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
};
