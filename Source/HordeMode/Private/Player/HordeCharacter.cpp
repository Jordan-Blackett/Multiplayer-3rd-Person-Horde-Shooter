// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "HordeMode.h"
#include "HordeWeapon.h"
#include "HordeHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "Public/TimerManager.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "AI/HordeAICharacter.h"
#include "HordeLoot.h"
#include "HordeLootWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HordeWeapon_HitScan.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Loot/HordeWeaponPartDataAsset.h"

// Sets default values
AHordeCharacter::AHordeCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<UHordeHealthComponent>(TEXT("HealthComp"));

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	WeaponAttachSocketName = "WeaponSocket";
	WeaponAttachEquipSocketName = "BackEquipSocket";

	MaxPistolAmmo = 600;
	MaxAssaultRifleAmmo = 400;
	MaxShotgunAmmo = 100;
	MaxSniperAmmo = 50;
	MaxExplosiveAmmo = 25;

	CurrentAssaultRifleAmmo = MaxAssaultRifleAmmo;
}

// Called when the game starts or when spawned
void AHordeCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		DefaultFOV = CameraComp->FieldOfView;

		HealthComp->OnHealthChanged.AddDynamic(this, &AHordeCharacter::OnHealthChanged);
		//HealthComp->OnHealthChanged.RemoveDynamic(this, &AHordeCharacter::OnHealthChanged);

		GetWorld()->GetTimerManager().SetTimer(TraceLineTimerHandle, this, &AHordeCharacter::InspectActor, 0.1f, true);
	}

	// Server Only
	if (Role == ROLE_Authority)
	{
		InitialiseDefaultInventory();
	}
}

// Called to bind functionality to input
void AHordeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHordeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHordeCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AHordeCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AHordeCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AHordeCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AHordeCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AHordeCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AHordeCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHordeCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AHordeCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Released, this, &AHordeCharacter::OnReload);

	PlayerInputComponent->BindAction("WeaponSlot1", IE_Pressed, this, &AHordeCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("WeaponSlot2", IE_Pressed, this, &AHordeCharacter::OnPrevWeapon);
	PlayerInputComponent->BindAction("WeaponSlot3", IE_Pressed, this, &AHordeCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("WeaponSlot4", IE_Pressed, this, &AHordeCharacter::OnPrevWeapon);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AHordeCharacter::OnNextWeapon);
	PlayerInputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AHordeCharacter::OnPrevWeapon);

	PlayerInputComponent->BindAction("PickUpLoot", IE_Pressed, this, &AHordeCharacter::PickUpInspectedLoot);
	PlayerInputComponent->BindAction("PickUpLoot", IE_Released, this, &AHordeCharacter::PickUpInspectedLoot);
}

void AHordeCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AHordeCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

//////////////////////////////////////////////////////////////////////////
//

void AHordeCharacter::BeginCrouch()
{
	Crouch();
}

void AHordeCharacter::EndCrouch()
{
	UnCrouch();
}

void AHordeCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void AHordeCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void AHordeCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AHordeCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void AHordeCharacter::OnReload()
{
	//AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	//if (MyPC && MyPC->IsGameInputAllowed())
	//{
		if (CurrentWeapon)
		{
			CurrentWeapon->StartReload();
		}
	//}
}

FRotator AHordeCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

bool AHordeCharacter::GetIsAiming() const
{
	return bWantsToZoom;
}

int32 AHordeCharacter::ReceiveAmmo(EAmmoType AmmoType, int32 AmmoCount)
{
	int32* TempAmmo = new int();
	switch (AmmoType)
	{
	case EAmmoType::EPistol:
		TempAmmo = &CurrentPistolAmmo;
		break;
	case EAmmoType::EAssaultRifle:
		TempAmmo = &CurrentAssaultRifleAmmo;
		break;
	case EAmmoType::EShotgun:
		TempAmmo = &CurrentShotgunAmmo;
		break;
	case EAmmoType::ESniper:
		TempAmmo = &CurrentShotgunAmmo;
		break;
	case EAmmoType::EExplosive:
		TempAmmo = &CurrentShotgunAmmo;
		break;
	default:
		TempAmmo = 0;
		break;
	}

	int32 AmmoDelta = FMath::Max(0, AmmoCount - *TempAmmo);
	*TempAmmo = FMath::Max(0, *TempAmmo - AmmoCount);
	return AmmoCount - AmmoDelta;
}

void AHordeCharacter::GiveAmmo(EAmmoType AmmoType, int32 AmmoCount)
{
	//const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	//AddAmount = FMath::Min(AddAmount, MissingAmmo);
	//CurrentAmmo += AddAmount;
}

int32 AHordeCharacter::GetMaxAmmo(EAmmoType AmmoType)
{
	switch (AmmoType)
	{
	case EAmmoType::EPistol:
		return MaxPistolAmmo;
		break;
	case EAmmoType::EAssaultRifle:
		return MaxAssaultRifleAmmo;
		break;
	case EAmmoType::EShotgun:
		return MaxShotgunAmmo;
		break;
	case EAmmoType::ESniper:
		return MaxSniperAmmo;
		break;
	case EAmmoType::EExplosive:
		return MaxExplosiveAmmo;
		break;
	default:
		return 0;
		break;
	}
}

int32 AHordeCharacter::GetCurrentAmmo(EAmmoType AmmoType)
{
	switch (AmmoType)
	{
	case EAmmoType::EPistol:
		return CurrentPistolAmmo;
		break;
	case EAmmoType::EAssaultRifle:
		return CurrentAssaultRifleAmmo;
		break;
	case EAmmoType::EShotgun:
		return CurrentShotgunAmmo;
		break;
	case EAmmoType::ESniper:
		return CurrentSniperAmmo;
		break;
	case EAmmoType::EExplosive:
		return CurrentExplosiveAmmo;
		break;
	default:
		return 0;
		break;
	}
}

void AHordeCharacter::OnHealthChanged(UHordeHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		// Die!
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

void AHordeCharacter::OnAmmoChangedDelegate(int32 ammo, int32 maxAmmo, int32 ammoInClip, int32 ammoPerClip)
{
	OnCurrentWeaponAmmoChanged.Broadcast(ammo, maxAmmo, ammoInClip, ammoPerClip);
}

// Called every frame
void AHordeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// TODO: is local

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

	if (CurrentWeapon)
	{
		// [0 - 1]
		float MovementSpread = GetVelocity().Size() / GetCharacterMovement()->GetMaxSpeed();
		CurrentWeapon->SetCurrentSpread(MovementSpread);

		// Weapon Stats Card
		FPartDeltaData WeaponStatCard = CurrentWeapon->GetWeaponStats();
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Damage: %s"), *FString::SanitizeFloat(WeaponStatCard.Damage)));
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Accuracy: %s"), *FString::SanitizeFloat(WeaponStatCard.Accuracy)));
		//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Damage: %s"), *FString::SanitizeFloat(WeaponStatCard.Damage)));
	}

}

FVector AHordeCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void AHordeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AHordeCharacter, Inventory, COND_OwnerOnly);

	DOREPLIFETIME(AHordeCharacter, CurrentWeapon);
	DOREPLIFETIME(AHordeCharacter, bWantsToZoom);
	DOREPLIFETIME(AHordeCharacter, bDied);
}

//////////////////////////////////////////////////////////////////////////
// Inventory

void AHordeCharacter::InitialiseDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// Add weapons to inventory
	int32 NumWeapons = DefaultInventory.Num();
	for (int32 i = 0; i < NumWeapons; i++)
	{
		if (DefaultInventory[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AHordeWeapon* NewWeapon = GetWorld()->SpawnActor<AHordeWeapon>(DefaultInventory[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// Equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void AHordeCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	// Remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AHordeWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void AHordeCharacter::AddWeapon(AHordeWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AHordeCharacter::RemoveWeapon(AHordeWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

void AHordeCharacter::EquipWeapon(AHordeWeapon* Weapon, bool NewLootWeapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, NewLootWeapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

AHordeWeapon* AHordeCharacter::FindWeapon(TSubclassOf<AHordeWeapon> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return NULL;
}

void AHordeCharacter::ServerEquipWeapon_Implementation(AHordeWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool AHordeCharacter::ServerEquipWeapon_Validate(AHordeWeapon* Weapon)
{
	return true;
}

void AHordeCharacter::SetCurrentWeapon(AHordeWeapon* NewWeapon, bool NewPickedUpWeapon, AHordeWeapon* CurrentWeapon1)
{
	// Server Only

	// Unequip Current Weapon
	if (CurrentWeapon1)
	{
		if (!NewPickedUpWeapon)
		{
			if (PreviousWeapon)
			{
				// Hide on back weapon
				PreviousWeapon->OnUnEquip();
				PreviousWeapon->OnUnEquipFromPlayerBack();
			}
			PreviousWeapon = CurrentWeapon1;

			// Hide current weapon and place it on the back
			CurrentWeapon1->OnUnEquip();
			CurrentWeapon1->OnEquipToPlayerBack();
		}
		else
		{
			// Replace current weapon with pickedup weapon
			CurrentWeapon1->OnUnEquip();
			CurrentWeapon1->OnUnEquipFromPlayerBack();
		}
		
		CurrentWeapon1->SetReticleWidgetVisibility(false);
		CurrentWeapon1->OnAmmoChanged.RemoveDynamic(this, &AHordeCharacter::OnAmmoChangedDelegate);
	}

	if (Role == ROLE_Authority)
	{
		// Trigger Client Weapon Swap (OnRep)
		CurrentWeapon = NewWeapon;
	}

	// Equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		
		NewWeapon->OnEquip(CurrentWeapon1); // TODO: why send last weapon

		NewWeapon->SetReticleWidgetVisibility(true);
		NewWeapon->OnAmmoChanged.AddDynamic(this, &AHordeCharacter::OnAmmoChangedDelegate);
		OnCurrentWeaponAmmoChanged.Broadcast(GetCurrentAmmo(NewWeapon->GetAmmoType1()), NewWeapon->GetMaxAmmo(), NewWeapon->GetCurrentAmmoInClip(), NewWeapon->GetAmmoPerClip());
	}
}

void AHordeCharacter::OnRep_CurrentWeapon(AHordeWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, false, LastWeapon);
}

//////////////////////////////////////////////////////////////////////////
// Actor Pickup

void AHordeCharacter::InspectActor()
{
	// Aim Dir
	FVector AimDir = FVector::ZeroVector;
	FVector CamLoc;
	FRotator CamRot;
	GetActorEyesViewPoint(CamLoc, CamRot);
	AimDir = CamRot.Vector();

	// Start Trace 
	FVector StartTrace = FVector::ZeroVector;
	FRotator UnusedRot;
	GetActorEyesViewPoint(StartTrace, UnusedRot);
	StartTrace = StartTrace + AimDir * ((GetActorLocation() - StartTrace) | AimDir);

	const FVector EndTrace = StartTrace + (AimDir * 15000);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::White, false, 0.5f, 0, 2.0f);

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(FName(TEXT("Loot_Trace")), true);
	TraceParams.bTraceAsyncScene = true;

	FHitResult Hit(ForceInit);
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Pawn, TraceParams))
	{
		// Enemy
		AHordeAICharacter* InspectedAICharacter = Cast<AHordeAICharacter>(Hit.GetActor());
		if (InspectedAICharacter) {
			if (!EnemyHealthHUD)
			{
				ShowEnemyHealthHUD(InspectedAICharacter->GetHealthComp());
				EnemyHealthHUD = true;
			}
		} else {
			if (EnemyHealthHUD)
			{
				HideEnemyHealthHUD();
				EnemyHealthHUD = false;
			}
		}
	}

	//if (GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_LOOT, TraceParams))
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *Hit.GetComponent()->GetName()));

	//	InspectedLoot = Cast<AHordeLoot>(Hit.GetActor());
	//	if (InspectedLoot)
	//	{
	//		// Weapon
	//		if (Cast<AHordeLootWeapon>(InspectedLoot)->GetWeaponClass() != nullptr)
	//		{
	//			//Cast<AHordeLootWeapon>(InspectedLoot)->getdelta

	//			// --- Weapon Stat Card ---
	//		/*		cahnge stats
	//			visualable true*/


	//			//AHordeWeapon* NewWeapon = Cast<AHordeLootWeapon>(InspectedLoot)->GetWeaponClass();
	//			//if (NewWeapon)
	//			//{
	//			//	NewWeapon->GetWeaponStats();
	//			//	FPartDeltaData WeaponStatCard = NewWeapon->GetWeaponStats();
	//			//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Damage: %f"), WeaponStatCard.Damage));
	//			//}
	//		}

	//		// Enemy
	//		{
	//		
	//		}
	//	}


		//if (InspectedLoot)
		//{
		//	//InspectedLoot->Destroy();
		//	//test -- on equip
		//}
		//else
		//{
		//	InspectedLoot = nullptr;
		//}
	//}
	//else
	//{
	//	InspectedLoot = nullptr;
	//}
}

void AHordeCharacter::PickUpInspectedLoot()
{
	if (Role != ROLE_Authority)
	{
		ServerInspectedLoot();
		return;
	}

	// Set timer
	if (!GetWorld()->GetTimerManager().IsTimerActive(HoldKeyTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(HoldKeyTimerHandle, this, &AHordeCharacter::EquipInspectedLoot, 1.0f, false);
		return;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(HoldKeyTimerHandle);
	}

	if (InspectedLoot)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("PICKED UP LOOT")));

		// Pick up

		// Weapon
		if (Cast<AHordeLoot>(InspectedLoot))
		{
			//getinputkeytimedown
		}

		InspectedLoot->Destroy();
		InspectedLoot = nullptr;
	}
}

void AHordeCharacter::ServerInspectedLoot_Implementation()
{
	PickUpInspectedLoot();
}

bool AHordeCharacter::ServerInspectedLoot_Validate()
{
	return true;
}

void AHordeCharacter::EquipInspectedLoot()
{
	if (InspectedLoot)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("EQUIP LOOT")));

		// Weapon
		if (Cast<AHordeLootWeapon>(InspectedLoot)->GetWeaponClass() != nullptr)
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);

			// Spawn new weapon
			//AHordeWeapon* NewWeapon = Cast<AHordeLootWeapon>(InspectedLoot)->GetWeaponClass();
			//if (NewWeapon)
			//{	
			//	NewWeapon->OnEnterInventory(this);

			//	// Equip new weapon
			//	EquipWeapon(NewWeapon, true);

			//	// Drop current weapon
			//	MulticastThrowWeaponLoot(InspectedLoot);

			//	// Replace current weapon inventory
			//	Inventory[CurrentWeaponIdx] = NewWeapon;
			//}
		}

		// TODO: Vacuum

		InspectedLoot->Destroy();
		InspectedLoot = nullptr;
	}
}

void AHordeCharacter::ThrowWeaponLoot(AHordeLoot* WeaponLoot)
{
	if (WeaponLoot) {
		FVector newLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f); // TODO: variables
		//DrawDebugSphere(GetWorld(), newLocation, 1.0f, 5.0f, FColor::Red, false, 10.0f);
		WeaponLoot->SetActorLocation(newLocation);
		Cast<AHordeLootWeapon>(WeaponLoot)->AddImpluse(GetActorForwardVector() * 500.0f);
	}
}

void AHordeCharacter::MulticastThrowWeaponLoot_Implementation(AHordeLoot* WeaponLoot)
{
	ThrowWeaponLoot(WeaponLoot);
}

bool AHordeCharacter::MulticastThrowWeaponLoot_Validate(AHordeLoot* WeaponLoot)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Input handlers

void AHordeCharacter::OnEquipWeapon0()
{
}

void AHordeCharacter::OnEquipWeapon1()
{

}

void AHordeCharacter::OnEquipWeapon2()
{

}

void AHordeCharacter::OnEquipWeapon3()
{

}

void AHordeCharacter::OnNextWeapon()
{
	//AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	//if (MyPC) // && MyPC->IsGameInputAllowed())
	//{
		if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
		{
			const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
			AHordeWeapon* NextWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
			EquipWeapon(NextWeapon);
		}
	//}
}

void AHordeCharacter::OnPrevWeapon()
{
	//AShooterPlayerController* MyPC = Cast<AShooterPlayerController>(Controller);
	//if (MyPC && MyPC->IsGameInputAllowed())
	//{
	//	if (Inventory.Num() >= 2 && (CurrentWeapon == NULL || CurrentWeapon->GetCurrentState() != EWeaponState::Equipping))
	//	{
	//		const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
	//		AShooterWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
	//		EquipWeapon(PrevWeapon);
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////
// Reading data

FName AHordeCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachSocketName;
}

FName AHordeCharacter::GetWeaponEquipAttachPoint() const
{
	return WeaponAttachEquipSocketName;
}

AHordeWeapon * AHordeCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}
