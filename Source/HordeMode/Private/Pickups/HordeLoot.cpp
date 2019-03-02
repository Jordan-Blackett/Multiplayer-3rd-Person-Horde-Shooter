// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLoot.h"
#include "HordeMode.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AHordeLoot::AHordeLoot()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionObjectType(COLLISION_LOOT);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	BoxComp->SetupAttachment(SphereComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(SphereComp);
}

// Called when the game starts or when spawned
void AHordeLoot::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->AddImpulse(FVector(FMath::RandRange(-200, 200), FMath::RandRange(-200, 200), 700.0f), NAME_None, true);

	if (Role == ROLE_Authority)
	{
	}
}

void AHordeLoot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{
		Destroy();
	}
}
