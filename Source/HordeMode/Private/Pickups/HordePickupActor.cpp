// Fill out your copyright notice in the Description page of Project Settings.

#include "HordePickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "HordePowerupActor.h"
#include "TimerManager.h"

// Sets default values
AHordePickupActor::AHordePickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10.0f;

	PowerUpTransform.SetLocation(FVector(0, 0, 50));
	PowerUpTransform.SetScale3D(FVector(0, 0, 0));

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AHordePickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority)
	{
		Respawn();
	}
}

void AHordePickupActor::Respawn()
{
	if (PowerUpClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s. Please update your Blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	FTransform PowerupTransform = GetTransform() + PowerUpTransform;
	PowerUpInstance = GetWorld()->SpawnActor<AHordePowerupActor>(PowerUpClass, PowerupTransform, SpawnParams);
}

void AHordePickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority && PowerUpInstance)
	{
		PowerUpInstance->ActivatePowerup(OtherActor);
		PowerUpInstance = nullptr;

		// Set Timer to respawn powerup
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AHordePickupActor::Respawn, CooldownDuration);
	}
}
