// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "HordeMode.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "HordeCharacter.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Blueprint/UserWidget.h"
#include "Player/HordePlayerController.h"
#include "Loot/HordeWeaponPartDataAsset.h"

// Sets default values
AHordeWeapon::AHordeWeapon()
{
	GripMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GripMeshComp"));
	RootComponent = GripMeshComp;
	GripMeshComp->bReceivesDecals = false;
	GripMeshComp->CastShadow = true;
	GripMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	GripMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GripMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	GripMeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	GripMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	BaseMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BaseMeshComp"));
	BaseMeshComp->SetupAttachment(GripMeshComp, "BaseSocket");
	//MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	BaseMeshComp->bReceivesDecals = false;
	BaseMeshComp->CastShadow = true;
	BaseMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	BaseMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BaseMeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	BaseMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//MeshComp->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);

	// Weapon Parts

	BarrelMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BarrelMeshComp"));
	BarrelMeshComp->SetupAttachment(BaseMeshComp, "BarrelSocket");
	BarrelMeshComp->bReceivesDecals = false;
	BarrelMeshComp->CastShadow = true;
	BarrelMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	BarrelMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BarrelMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BarrelMeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	BarrelMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	StockMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StockMeshComp"));
	StockMeshComp->SetupAttachment(BaseMeshComp, "StockSocket");
	StockMeshComp->bReceivesDecals = false;
	StockMeshComp->CastShadow = true;
	StockMeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	StockMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StockMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StockMeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	StockMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	bLoopedMuzzleFX = false;
	bLoopedFireAnim = false;
	bPlayingFireAnim = false;
	bIsEquipped = false;
	bWantsToFire = false;
	bPendingReload = false;
	bPendingEquip = false;
	CurrentState = EWeaponState::Idle;

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

	CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
}

void AHordeWeapon::Destroyed()
{
	Super::Destroyed();

	StopSimulatingWeaponFire();
}

//////////////////////////////////////////////////////////////////////////
// Inventory /////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void AHordeWeapon::OnEquip(const AHordeWeapon* LastWeapon)
{
	AttachMeshToPawn();

	bPendingEquip = true;
	DetermineWeaponState();

	// Only play animation if last weapon is valid
	if (LastWeapon)
	{
		float Duration = PlayWeaponAnimation(EquipAnimation);
		if (Duration <= 0.0f)
		{
			// failsafe
			Duration = 0.5f;
		}
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AHordeWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}

	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}

	//AHordeCharacter::NotifyEquipWeapon.Broadcast(MyPawn, this);
}

void AHordeWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;

	// Determine the state so that the can reload checks will work
	DetermineWeaponState();

	if (MyPawn)
	{
		// try to reload empty clip
		if (MyPawn->IsLocallyControlled() &&
			CurrentAmmoInClip <= 0 &&
			CanReload())
		{
			StartReload();
		}

		SetReticleWidgetVisibility(true);
		//OnAmmoChanged.Broadcast(MyPawn->GetCurrentAmmo(GetAmmoType()), MyPawn->GetMaxAmmo(GetAmmoType()), CurrentAmmoInClip, WeaponConfig.AmmoPerClip);
	}
}

void AHordeWeapon::OnUnEquip()
{
	bIsEquipped = false;
	StopFire();

	if (bPendingReload)
	{
		StopWeaponAnimation(ReloadAnimation);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		StopWeaponAnimation(EquipAnimation);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	//AShooterCharacter::NotifyUnEquipWeapon.Broadcast(MyPawn, this);

	DetermineWeaponState();
}

void AHordeWeapon::OnEnterInventory(AHordeCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);

	// Init Reticle
	if(ReticleWidgetClass)
	{
		if (NewOwner->IsLocallyControlled())
		{
			ReticleWidget = CreateWidget<UUserWidget>(GetWorld(), ReticleWidgetClass);
			if(ReticleWidget)
			{
				ReticleWidget->AddToViewport();
				ReticleWidget->SetVisibility(ESlateVisibility::Hidden);	
			}
		}
	}
}

void AHordeWeapon::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (Role == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
}

void AHordeWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide mesh
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if (MyPawn->IsLocallyControlled())
		{
			USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
			BaseMeshComp->SetHiddenInGame(false);
			BaseMeshComp->AttachToComponent(PawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AHordeWeapon::DetachMeshFromPawn()
{
	BaseMeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	BaseMeshComp->SetHiddenInGame(true);
}

void AHordeWeapon::AttachMeshToPawnPrevEquipSocket()
{
	if (MyPawn)
	{
		// Remove and hide mesh
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		FName AttachPoint = MyPawn->GetWeaponEquipAttachPoint();
		if (MyPawn->IsLocallyControlled() == true)
		{
			USkeletalMeshComponent* PawnMesh = MyPawn->GetMesh();
			BaseMeshComp->SetHiddenInGame(false);
			BaseMeshComp->AttachToComponent(PawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame(false);
		}
	}
}

void AHordeWeapon::OnEquipToPlayerBack()
{
	AttachMeshToPawnPrevEquipSocket();
}

void AHordeWeapon::OnUnEquipFromPlayerBack()
{
	DetachMeshFromPawn();
}

//////////////////////////////////////////////////////////////////////////
// Input /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void AHordeWeapon::StartFire()
{
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

		float AnimDuration = PlayWeaponAnimation(ReloadAnimation);
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
		StopWeaponAnimation(ReloadAnimation);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input - Server Side ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

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

bool AHordeWeapon::ServerStopReload_Validate()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Replication & Client-Side Effects /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void AHordeWeapon::OnRep_MyPawn()
{
	if (MyPawn)
	{
		OnEnterInventory(MyPawn);
	}
	else
	{
		OnLeaveInventory();
	}
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
	//TODO: REMOVe
	bool burst = ((!WeaponConfig.BurstMode) || (BurstFireCounter >= WeaponConfig.NumOfBurstShots));
	if (Role == ROLE_Authority && CurrentState != EWeaponState::Firing && burst)
	{
		return;
	}

	if (MuzzleFX)
	{
		if (!bLoopedMuzzleFX || MuzzlePSC == NULL)
		{
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, BarrelMeshComp, MuzzleAttachPoint);
		}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		PlayWeaponAnimation(FireAnimation);
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

	AHordePlayerController* PC = (MyPawn != NULL) ? Cast<AHordePlayerController>(MyPawn->Controller) : NULL;
	if (PC != NULL && PC->IsLocalController())
	{
		if (FireCameraShake != NULL)
		{
			PC->ClientPlayCameraShake(FireCameraShake, 1);
		}
		if (FireForceFeedback != NULL)// && PC->IsVibrationEnabled())
		{
			PC->ClientPlayForceFeedback(FireForceFeedback, false, false, "Weapon");
		}
	}
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
		StopWeaponAnimation(FireAnimation);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = NULL;

		PlayWeaponSound(FireFinishSound);
	}
}

//////////////////////////////////////////////////////////////////////////
// Weapon Usage //////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

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
			// Shotgun Mode
			if(WeaponConfig.ShotgunMode)
			{
				for (int i = 0; i < WeaponConfig.NumOfShotgunShots; i++)
				{
					FireWeapon();
				}
			}
			else
			{
				FireWeapon();
			}
			
			UseAmmo();

			// update firing FX on remote clients if function was called on server
			BurstCounter++;

			// BurstFire
			if (WeaponConfig.BurstMode)
			{
				BurstInProgress = true;
				BurstFireCounter++;
				if (BurstFireCounter < WeaponConfig.NumOfBurstShots && CurrentAmmoInClip > 0)
				{
					GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AHordeWeapon::HandleFiring, WeaponConfig.TimeBetweenBurstShots, false);
				}
				else {
					BurstInProgress = false;
					BurstFireCounter = 0;
				}
			}
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	// Out of ammo
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
			//BurstFireCounter = 0;
		}

		// setup refire timer
		bool burst = ((!WeaponConfig.BurstMode) || (BurstFireCounter == 0));
		bRefiring = (CurrentState == EWeaponState::Firing && WeaponConfig.TimeBetweenShots > 0.0f && burst);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AHordeWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
			//BurstFireCounter = 0;
		}

		if (!BurstInProgress)
		{
			DetermineWeaponState();
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

	if (!HasInfiniteAmmo() && HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	OnAmmoChanged.Broadcast(MyPawn->GetCurrentAmmo(GetAmmoType()), MyPawn->GetMaxAmmo(GetAmmoType()), CurrentAmmoInClip, WeaponConfig.AmmoPerClip);
}

void AHordeWeapon::ReloadWeapon()
{
	int32 ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	
	if (HasInfiniteClip())
	{
		ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	}

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += MyPawn->ReceiveAmmo(GetAmmoType(), ClipDelta);
	}

	if (HasInfiniteClip())
	{
		//CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	}

	OnAmmoChanged.Broadcast(MyPawn->GetCurrentAmmo(GetAmmoType()), MyPawn->GetMaxAmmo(GetAmmoType()), CurrentAmmoInClip, WeaponConfig.AmmoPerClip);
}

void AHordeWeapon::DetermineWeaponState()
{
	EWeaponState::Type NewState = EWeaponState::Idle;

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
	else if ((bPendingReload == false) && (bWantsToFire == true || BurstInProgress) && (CanFire() == true))
	{
		NewState = EWeaponState::Firing;
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

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing) //&& !WeaponConfig.BurstMode
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
	bool burst = (BurstFireCounter == 0);
	if (burst)
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
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage helpers //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

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

FVector AHordeWeapon::GetAdjustedAim() const
{
	AActor* MyOwner = GetOwner();
	FVector FinalAim = FVector::ZeroVector;
	if (MyOwner)
	{
		FVector CamLoc;
		FRotator CamRot;
		MyOwner->GetActorEyesViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	return FinalAim;
}

FVector AHordeWeapon::GetCameraDamageStartLocation(const FVector & AimDir) const
{
	AActor* MyOwner = GetOwner();
	FVector OutStartTrace = FVector::ZeroVector;
	if (MyOwner)
	{
		// Use player's camera
		FRotator UnusedRot;
		MyOwner->GetActorEyesViewPoint(OutStartTrace, UnusedRot);
		
		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((MyOwner->GetActorLocation() - OutStartTrace) | AimDir);
	}

	return OutStartTrace;
}

FVector AHordeWeapon::GetMuzzleLocation() const
{
	//USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	USkeletalMeshComponent* UseMesh = BarrelMeshComp;
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector AHordeWeapon::GetMuzzleDirection() const
{
	USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult AHordeWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	return Hit;
}

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
// Control ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool AHordeWeapon::CanFire() const
{
	bool bCanFire = true; //MyPawn && MyPawn->CanFire();
	bool bStateOKToFire = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanFire == true) && (bStateOKToFire == true) && (bPendingReload == false));
}

bool AHordeWeapon::CanReload() const
{
	bool bCanReload = true; //(!MyPawn || MyPawn->CanReload());
	bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && ((MyPawn->GetCurrentAmmo(GetAmmoType()) - CurrentAmmoInClip) > 0 || HasInfiniteClip());
	bool bStateOKToReload = ((CurrentState == EWeaponState::Idle) || (CurrentState == EWeaponState::Firing));
	return ((bCanReload == true) && (bGotAmmo == true) && (bStateOKToReload == true));
}

//////////////////////////////////////////////////////////////////////////
// Reading Data  /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool AHordeWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool AHordeWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

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
	return 0;
}

USkeletalMeshComponent * AHordeWeapon::GetWeaponMesh() const
{
	return BaseMeshComp;
}


void AHordeWeapon::SetWeaponBaseMesh(USkeletalMesh * BaseMesh)
{
	BaseMeshComp->SetSkeletalMesh(BaseMesh);
}

void AHordeWeapon::SetWeaponBarrelMesh(USkeletalMesh * BarrelMesh)
{
	BarrelMeshComp->SetSkeletalMesh(BarrelMesh);
}

void AHordeWeapon::SetWeaponStockMesh(USkeletalMesh * StockMesh)
{
	StockMeshComp->SetSkeletalMesh(StockMesh);
}

void AHordeWeapon::SetWeaponGripMesh(USkeletalMesh * GripMesh)
{
	GripMeshComp->SetSkeletalMesh(GripMesh);
}

FWeaponData * AHordeWeapon::GetWeaponConfig()
{
	return &WeaponConfig;
}

void AHordeWeapon::SetWeaponConfig(FWeaponData * Config)
{
	WeaponConfig = *Config;
}

FPartDeltaData AHordeWeapon::GetWeaponStats()
{
	return FPartDeltaData();
}

void AHordeWeapon::SetWeaponDeltaStats(FPartDeltaData * Config)
{
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

float AHordeWeapon::GetEquipDuration() const
{
	return EquipDuration;
}

void AHordeWeapon::SetCurrentSpread(float spread)
{
}

void AHordeWeapon::SetReticleWidgetVisibility(bool hidden)
{
	if (ReticleWidget)
	{
		if (hidden)
		{
			ReticleWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ReticleWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AHordeWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordeWeapon, MyPawn);

	DOREPLIFETIME_CONDITION(AHordeWeapon, CurrentAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AHordeWeapon, CurrentAmmoInClip, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(AHordeWeapon, BurstCounter, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AHordeWeapon, bPendingReload, COND_SkipOwner);
}

