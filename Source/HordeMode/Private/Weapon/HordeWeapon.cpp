// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "HordeMode.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "HordeCharacter.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

// Sets default values
AHordeWeapon::AHordeWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	CurrentAmmo = 0;
	CurrentAmmoInClip = 0;
	BurstCounter = 0;
	LastFireTime = 0.0f;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void AHordeWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	}

	MyPawn = Cast<AHordeCharacter>(GetOwner());

	//DetachMeshFromPawn();
}

void AHordeWeapon::Destroyed()
{
	Super::Destroyed();

	//StopSimulatingWeaponFire();
}

//////////////////////////////////////////////////////////////////////////
// Inventory


//////////////////////////////////////////////////////////////////////////
// Input

void AHordeWeapon::StartFire()
{
	UE_LOG(LogTemp, Warning, TEXT("StartFire"));

	if (Role < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void AHordeWeapon::StopFire()
{
	if (Role < ROLE_Authority)
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void AHordeWeapon::StartReload(bool bFromReplication)
{
	if (!bFromReplication && Role < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();

		float AnimDuration = PlayWeaponAnimation(ReloadAnim);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = WeaponConfig.NoAnimReloadDuration;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AHordeWeapon::StopReload, AnimDuration, false);
		if (Role == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AHordeWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}

void AHordeWeapon::StopReload()
{
	if (CurrentState == EWeaponState::Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		StopWeaponAnimation(ReloadAnim);
	}
}

void AHordeWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool AHordeWeapon::ServerStartFire_Validate()
{
	return true;
}

void AHordeWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool AHordeWeapon::ServerStopFire_Validate()
{
	return true;
}

void AHordeWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

bool AHordeWeapon::ServerStartReload_Validate()
{
	return true;
}

void AHordeWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

void AHordeWeapon::ClientStartReload_Implementation()
{
	StartReload();
}

bool AHordeWeapon::ServerStopReload_Validate()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Control

bool AHordeWeapon::CanFire() const
{
	bool bCanFire = true; //MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool AHordeWeapon::CanReload() const
{
	bool bCanReload = true; //(!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void AHordeWeapon::HandleFiring()
{
	if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			//FireWeapon();

			UseAmmo();

			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (MyPawn && MyPawn->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);
			//AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(MyPawn->Controller);
			//AShooterHUD* MyHUD = MyPC ? Cast<AShooterHUD>(MyPC->GetHUD()) : NULL;
			//if (MyHUD)
			//{
			//	MyHUD->NotifyOutOfAmmo();
			//}
		}

		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		// local client will notify server
		if (Role < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload after firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AHordeWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AHordeWeapon::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
}

bool AHordeWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void AHordeWeapon::UseAmmo()
{
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	//AShooterAIController* BotAI = MyPawn ? Cast<AShooterAIController>(MyPawn->GetController()) : NULL;
	//AShooterPlayerController* PlayerController = MyPawn ? Cast<AShooterPlayerController>(MyPawn->GetController()) : NULL;
	//if (BotAI)
	//{
	//	BotAI->CheckAmmo(this);
	//}
	//else if (PlayerController)
	//{
	//	AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>(PlayerController->PlayerState);
	//	switch (GetAmmoType())
	//	{
	//	case EAmmoType::ERocket:
	//		PlayerState->AddRocketsFired(1);
	//		break;
	//	case EAmmoType::EBullet:
	//	default:
	//		PlayerState->AddBulletsFired(1);
	//		break;
	//	}
	//}
}

//void AHordeWeapon::GiveAmmo(int AddAmount)
//{
//	//const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
//	//AddAmount = FMath::Min(AddAmount, MissingAmmo);
//	//CurrentAmmo += AddAmount;
//
//	//AShooterAIController* BotAI = MyPawn ? Cast<AShooterAIController>(MyPawn->GetController()) : NULL;
//	//if (BotAI)
//	//{
//	//	BotAI->CheckAmmo(this);
//	//}
//
//	//// start reload if clip was empty
//	//if (GetCurrentAmmoInClip() <= 0 &&
//	//	CanReload() &&
//	//	MyPawn && (MyPawn->GetWeapon() == this))
//	//{
//	//	ClientStartReload();
//	//}
//}

void AHordeWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (HasInfiniteClip())
	{
		ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	}

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}

	if (HasInfiniteClip())
	{
		CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	}
}

void AHordeWeapon::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

	bIsEquipped = true; // REMOVE
	if (bIsEquipped)
	{
		if (bPendingReload)
		{
			if (CanReload() == false)
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::Reloading;
			}
		}
		else if ((bPendingReload == false) && (bWantsToFire == true) && (CanFire() == true))
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	SetWeaponState(NewState);
}

void AHordeWeapon::SetWeaponState(EWeaponState::Type NewState)
{
	const EWeaponState::Type PrevState = CurrentState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}

void AHordeWeapon::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
		LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AHordeWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void AHordeWeapon::OnBurstFinished()
{
	// stop firing FX on remote clients
	BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers

UAudioComponent* AHordeWeapon::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = NULL;
	if (Sound && MyPawn)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, MyPawn->GetRootComponent());
	}

	return AC;
}

float AHordeWeapon::PlayWeaponAnimation(UAnimMontage* Animation)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayAnimMontage1"));
		Duration = MyPawn->PlayAnimMontage(Animation);
	}

	return Duration;
}


void AHordeWeapon::StopWeaponAnimation(UAnimMontage* Animation)
{
	if (MyPawn)
	{
		MyPawn->StopAnimMontage(Animation);
	}
}

FVector AHordeWeapon::GetMuzzleLocation() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector AHordeWeapon::GetMuzzleDirection() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

//FHitResult AShooterWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
//{
//
//	// Perform trace to retrieve hit info
//	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
//	TraceParams.bTraceAsyncScene = true;
//	TraceParams.bReturnPhysicalMaterial = true;
//
//	FHitResult Hit(ForceInit);
//	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
//
//	return Hit;
//}

void AHordeWeapon::SetOwningPawn(AHordeCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		Instigator = NewOwner;
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}

//////////////////////////////////////////////////////////////////////////
// Replication & effects

void AHordeWeapon::OnRep_MyPawn()
{
	//if (MyPawn)
	//{
	//	OnEnterInventory(MyPawn);
	//}
	//else
	//{
	//	OnLeaveInventory();
	//}
}

void AHordeWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}

void AHordeWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}

void AHordeWeapon::SimulateWeaponFire()
{
	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing)
	{
		return;
	}

	if (MuzzleFX)
	{
		USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
		if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
		{
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, UseWeaponMesh, MuzzleAttachPoint);
		}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	if (bLoopedFireSound)
	{
		if (FireAC == NULL)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}

	//AShooterPlayerController* PC = (MyPawn != NULL) ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
	//if (PC != NULL && PC->IsLocalController())
	//{
	//	if (FireCameraShake != NULL)
	//	{
	//		PC->ClientPlayCameraShake(FireCameraShake, 1);
	//	}
	//	if (FireForceFeedback != NULL && PC->IsVibrationEnabled())
	//	{
	//		PC->ClientPlayForceFeedback(FireForceFeedback, false, false, "Weapon");
	//	}
	//}
}

void AHordeWeapon::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX)
	{
		if (MuzzlePSC != NULL)
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = NULL;
		}
	}

	if (bLoopedFireAnim && bPlayingFireAnim)
	{
		StopWeaponAnimation(FireAnim);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = NULL;

		PlayWeaponSound(FireFinishSound);
	}
}

//bool AHordeWeapon::IsEquipped() const
//{
//	return bIsEquipped;
//}

//bool AHordeWeapon::IsAttachedToPawn() const
//{
//	return bIsEquipped || bPendingEquip;
//}

EWeaponState::Type AHordeWeapon::GetCurrentState() const
{
	return CurrentState;
}

int32 AHordeWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 AHordeWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 AHordeWeapon::GetAmmoPerClip() const
{
	return WeaponConfig.AmmoPerClip;
}

int32 AHordeWeapon::GetMaxAmmo() const
{
	return WeaponConfig.MaxAmmo;
}

USkeletalMeshComponent * AHordeWeapon::GetWeaponMesh() const
{
	return MeshComp;
}

bool AHordeWeapon::HasInfiniteAmmo() const
{
	//const AShooterPlayerController* MyPC = (MyPawn != NULL) ? Cast<const AShooterPlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteAmmo;// || (MyPC && MyPC->HasInfiniteAmmo());
}

bool AHordeWeapon::HasInfiniteClip() const
{
	//const AShooterPlayerController* MyPC = (MyPawn != NULL) ? Cast<const AShooterPlayerController>(MyPawn->Controller) : NULL;
	return WeaponConfig.bInfiniteClip;// || (MyPC && MyPC->HasInfiniteClip());
}

float AHordeWeapon::GetEquipStartedTime() const
{
	return EquipStartedTime;
}

//float AHordeWeapon::GetEquipDuration() const
//{
//	return EquipDuration;
//}

void AHordeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordeWeapon, MyPawn);

	DOREPLIFETIME_CONDITION(AHordeWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHordeWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(AHordeWeapon, BurstCounter, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AHordeWeapon, bPendingReload, COND_SkipOwner);
}

