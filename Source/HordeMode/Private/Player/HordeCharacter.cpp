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
}

// Called when the game starts or when spawned
void AHordeCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	HealthComp->OnHealthChanged.AddDynamic(this, &AHordeCharacter::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		// Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<AHordeWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwningPawn(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
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
}

void AHordeCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void AHordeCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

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

// Called every frame
void AHordeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
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

	DOREPLIFETIME(AHordeCharacter, CurrentWeapon);
	DOREPLIFETIME(AHordeCharacter, bDied);
}