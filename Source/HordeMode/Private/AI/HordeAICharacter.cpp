// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeAICharacter.h"
#include "HordeHealthComponent.h"


// Sets default values
AHordeAICharacter::AHordeAICharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<UHordeHealthComponent>(TEXT("HealthComp"));
}

UHordeHealthComponent* AHordeAICharacter::GetHealthComp()
{
	return HealthComp;
}

// Called when the game starts or when spawned
void AHordeAICharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->GetHealth();
}
