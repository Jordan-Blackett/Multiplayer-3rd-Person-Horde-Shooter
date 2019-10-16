// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HordeWeapon.generated.h"

class AHordeCharacter;
class USkeletalMeshComponent;
class UParticleSystemComponent;
class UParticleSystem;
class UCameraShake;
class UForceFeedbackEffect;
class UAudioComponent;
class USoundCue;
class UAnimMontage;
class UUserWidget;

// OnAmmoChanged event - TODO: remove and just call pawn->nNotifyOutOfAmmo
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAmmoChangedSignature, int32, ammo, int32, maxAmmo, int32, ammoInClip, int32, ammoPerClip);

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	ENone,
	EPistol,
	EAssaultRifle,
	EShotgun,
	ESniper,
	EExplosive,
};

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	EAmmoType AmmoType;

	/** inifite ammo for reloads */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	bool bInfiniteClip;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 AmmoPerClip;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float TimeBetweenShots;

	/** failsafe reload duration if weapon doesn't have any animation for it */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float NoAnimReloadDuration;

	/** defaults */
	FWeaponData()
	{
		EAmmoType EAmmoType = EAmmoType::ENone;
		bInfiniteAmmo = false;
		bInfiniteClip = false;
		AmmoPerClip = 20;
		TimeBetweenShots = 0.2f;
		NoAnimReloadDuration = 1.0f;
	}
};

// Contains information of a single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	UPROPERTY()
	uint8 ReplicationCount;
};

UCLASS()
class HORDEMODE_API AHordeWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHordeWeapon();

protected:
	/** perform initial setup */
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* BaseMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BarrelMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StockMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GripMeshComp;

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class AHordeCharacter* MyPawn;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;

	//////////////////////////////////////////////////////////////////////////
	// Effects ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// Spawned Component For Muzzle FX */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

	// Muzzle Socket Attach Name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	FName MuzzleAttachPoint;

	// FX for muzzle flash
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFX;

	// is muzzle FX looped?
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	uint32 bLoopedMuzzleFX : 1;

	// 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UCameraShake> FireCameraShake;

	// force feedback effect to play when the weapon is fired
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UForceFeedbackEffect* FireForceFeedback;

	// Reticle UMG Widget
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UUserWidget> ReticleWidgetClass;

	// Reference to created user widget*/
	UPROPERTY(Transient)
	class UUserWidget* ReticleWidget;

	//////////////////////////////////////////////////////////////////////////
	// Sound /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* FireFinishSound;

	/** out of ammo sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* OutOfAmmoSound;

	/** reload sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* ReloadSound;

	/** equip sound */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* EquipSound;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	uint32 bLoopedFireSound : 1;

	//////////////////////////////////////////////////////////////////////////
	// Animation /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	// Equip animations
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* EquipAnimation;

	// Fire animations
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireAnimation;

	// Reload animations
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ReloadAnimation;

	// Is fire animation looped?
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	uint32 bLoopedFireAnim : 1;

	//////////////////////////////////////////////////////////////////////////
	// ///////// /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	/** is reload animation playing? */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Reload)
	uint32 bPendingReload : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	/** weapon is refiring */
	uint32 bRefiring;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	/** current total ammo */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	/** current ammo - inside clip */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

	/** Handle for efficient management of StopReload timer */
	FTimerHandle TimerHandle_StopReload;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	//////////////////////////////////////////////////////////////////////////
	// Input - Server Side ///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();
	void ServerStartFire_Implementation();
	bool ServerStartFire_Validate();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStartReload();
	void ServerStartReload_Implementation();
	bool ServerStartReload_Validate();

	UFUNCTION(reliable, server, WithValidation)
	void ServerStopReload();
	void ServerStopReload_Implementation();
	bool ServerStopReload_Validate();

	//////////////////////////////////////////////////////////////////////////
	// Replication & Client-Side Effects /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	UFUNCTION()
	void OnRep_MyPawn();

	UFUNCTION()
	void OnRep_BurstCounter();

	UFUNCTION()
	void OnRep_Reload();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	//////////////////////////////////////////////////////////////////////////
	// Weapon Usage //////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** [Local] Weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AShooterWeapon::FireWeapon, );

	/** [Server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
	void ServerHandleFiring();
	void ServerHandleFiring_Implementation();
	bool ServerHandleFiring_Validate();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();

	/** update weapon state */
	void SetWeaponState(EWeaponState::Type NewState);

	/** determine current weapon state */
	void DetermineWeaponState();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage helpers //////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** Get the aim of the camera */
	//FVector GetCameraAim() const;

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	/** play weapon animations */
	float PlayWeaponAnimation(UAnimMontage* Animation);

	/** stop playing weapon animations */
	void StopWeaponAnimation(UAnimMontage* Animation);

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	//////////////////////////////////////////////////////////////////////////
	// Ammo //////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** Consume a bullet */
	void UseAmmo();

	/** [server] performs actual reload */
	virtual void ReloadWeapon();

	/** query ammo type */
	virtual EAmmoType GetAmmoType() const
	{
		return WeaponConfig.AmmoType;
	}

	//////////////////////////////////////////////////////////////////////////
	// Inventory /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

	/** detaches weapon mesh from pawn */
	void AttachMeshToPawnPrevEquipSocket();

public:	

	//////////////////////////////////////////////////////////////////////////
	// Input /////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** [all] start weapon reload */
	virtual void StartReload(bool bFromReplication = false);

	/** [local + server] interrupt weapon reload */
	virtual void StopReload();

	//////////////////////////////////////////////////////////////////////////
	// Control ///////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** check if weapon can fire */
	bool CanFire() const;

	/** check if weapon can be reloaded */
	bool CanReload() const;

	//////////////////////////////////////////////////////////////////////////
	// Inventory /////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(const AHordeWeapon* LastWeapon);

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(AHordeCharacter* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();

	/** check if it's currently equipped */
	bool IsEquipped() const;

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

	void OnEquipToPlayerBack();

	void OnUnEquipFromPlayerBack();

	//////////////////////////////////////////////////////////////////////////
	// Reading Data //////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	/** get current weapon state */
	EWeaponState::Type GetCurrentState() const;

	//////////////////////////////////////////////////////////////////////////
	// Reading data - Ammo

	/** get current ammo amount (total) */
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	int32 GetMaxAmmo() const;

	/** check if weapon has infinite ammo (include owner's cheats) */
	bool HasInfiniteAmmo() const;

	/** check if weapon has infinite clip (include owner's cheats) */
	bool HasInfiniteClip() const;

	//////////////////////////////////////////////////////////////////////////
	// Reading data - 

	/** get weapon mesh (needs pawn owner to determine variant) */
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** set the weapon's owning pawn */
	void SetOwningPawn(AHordeCharacter* AHordeCharacter);

	/** gets last time when this weapon was switched to */
	float GetEquipStartedTime() const;

	/** gets the duration of equipping weapon*/
	float GetEquipDuration() const;

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnAmmoChangedSignature OnAmmoChanged;

	virtual void SetCurrentSpread(float spread);

	void SetReticleWidgetVisibility(bool hidden);

};
