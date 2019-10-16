// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HordeCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AHordeWeapon;
class UHordeHealthComponent;
class AHordeLoot;
enum class EAmmoType : uint8;

// OnAmmoChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnCurrentWeaponAmmoChangedSignature, int32, ammo, int32, maxAmmo, int32, ammoInClip, int32, ammoPerClip);

UCLASS()
class HORDEMODE_API AHordeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHordeCharacter();

public:

	//////////////////////////////////////////////////////////////////////////
	// Inventory /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// [Server] Add weapon to inventory
	void AddWeapon(class AHordeWeapon* Weapon);

	// [Server] Remove weapon from inventory
	void RemoveWeapon(class AHordeWeapon* Weapon);

	// [Server + Client] Equips weapon from inventory
	void EquipWeapon(class AHordeWeapon* Weapon, bool NewLootWeapon = false);

	// Find weapon in inventory
	class AHordeWeapon* FindWeapon(TSubclassOf<class AHordeWeapon> WeaponClass);

	//////////////////////////////////////////////////////////////////////////
	// Input handlers ////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// Player pressed equip weapon action
	void OnEquipWeapon0();
	void OnEquipWeapon1();
	void OnEquipWeapon2();
	void OnEquipWeapon3();

	// Player pressed next weapon action
	void OnNextWeapon();

	// Player pressed prev weapon action
	void OnPrevWeapon();

	//////////////////////////////////////////////////////////////////////////
	// Reading data

	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	/** get weapon attach point */
	FName GetWeaponEquipAttachPoint() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	AHordeWeapon* GetCurrentWeapon() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHordeHealthComponent* HealthComp;

	UPROPERTY(Replicated)
	bool bWantsToZoom;

	/* Default FOV set during begin play */
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	void BeginZoom();

	void EndZoom();

	UPROPERTY(Replicated)
	AHordeWeapon* CurrentWeapon;

	UPROPERTY(Replicated)
	AHordeWeapon* PreviousWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<AHordeWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachEquipSocketName;

	UFUNCTION()
	void OnHealthChanged(UHordeHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnAmmoChangedDelegate(int32 ammo, int32 maxAmmo, int32 ammoInClip, int32 ammoPerClip);

	/* Pawn died previously */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	// Default inventory list
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AHordeWeapon>> DefaultInventory;

	// Weapons inventory
	UPROPERTY(Transient, Replicated)
	TArray<class AHordeWeapon*> Inventory;

	//////////////////////////////////////////////////////////////////////////
	// Actor Pickup

	FTimerHandle TraceLineTimerHandle;

	AHordeLoot* InspectedLoot = nullptr;


	FTimerHandle HoldKeyTimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	//////////////////////////////////////////////////////////////////////////
	// Input handlers

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	/** player pressed reload action */
	void OnReload();

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	FRotator GetAimOffsets() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	bool GetIsAiming() const;

	//

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnCurrentWeaponAmmoChangedSignature OnCurrentWeaponAmmoChanged;

	//////////////////////////////////////////////////////////////////////////
	// Ammo //////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxPistolAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxAssaultRifleAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxShotgunAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxSniperAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	int32 MaxExplosiveAmmo;

	int32 CurrentPistolAmmo;
	int32 CurrentAssaultRifleAmmo;
	int32 CurrentShotgunAmmo;
	int32 CurrentSniperAmmo;
	int32 CurrentExplosiveAmmo;

	int32 ReceiveAmmo(EAmmoType AmmoType, int32 AmmoCount);
	void GiveAmmo(EAmmoType AmmoType, int32 AmmoCount);

	int32 GetMaxAmmo(EAmmoType AmmoType);
	int32 GetCurrentAmmo(EAmmoType AmmoType);

protected:

	//////////////////////////////////////////////////////////////////////////
	// Inventory

	// [Server] Initialise's default inventory
	void InitialiseDefaultInventory();

	// [Server] Remove all weapons from inventory and destroy them */
	void DestroyInventory();

	// [Server] Equip weapon
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class AHordeWeapon* NewWeapon);
	void ServerEquipWeapon_Implementation(class AHordeWeapon* NewWeapon);
	bool ServerEquipWeapon_Validate(class AHordeWeapon* NewWeapon);

	// Updates current weapon
	void SetCurrentWeapon(class AHordeWeapon* NewWeapon, bool NewLootWeapon, class AHordeWeapon* CurrentWeapon1);

	// Current weapon rep handler
	UFUNCTION()
	void OnRep_CurrentWeapon(class AHordeWeapon* LastWeapon);

	//////////////////////////////////////////////////////////////////////////
	// Actor Pickup

	// TraceLine
	UFUNCTION()
	void InspectActor();

	UFUNCTION()
	void PickUpInspectedLoot();

	// [Server] Equip weapon
	UFUNCTION(reliable, server, WithValidation)
	void ServerInspectedLoot();
	void ServerInspectedLoot_Implementation();
	bool ServerInspectedLoot_Validate();

	UFUNCTION()
	void EquipInspectedLoot();
	
	// [Server +  Client] Throw weapon
	UFUNCTION()
	void ThrowWeaponLoot(AHordeLoot* WeaponLoot);

	UFUNCTION(reliable, NetMulticast, WithValidation)
	void MulticastThrowWeaponLoot(AHordeLoot* WeaponLoot);
	void MulticastThrowWeaponLoot_Implementation(AHordeLoot* WeaponLoot);
	bool MulticastThrowWeaponLoot_Validate(AHordeLoot* WeaponLoot);


	/** update targeting state */
	//UFUNCTION(reliable, server, WithValidation)
	//void ServerSetTargeting(bool bNewTargeting);

	/** update targeting state */
	//UFUNCTION(reliable, server, WithValidation)
	//void ServerSetRunning(bool bNewRunning, bool bToggle);

	/** Builds list of points to check for pausing replication for a connection*/
	//void BuildPauseReplicationCheckPoints(TArray<FVector>& RelevancyCheckPoints);

};
