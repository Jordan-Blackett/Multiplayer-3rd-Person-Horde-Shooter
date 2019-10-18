// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLoot.h"
#include "HordeMode.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Public/TimerManager.h"
#include "Engine/Engine.h"

// Sets default values
AHordeLoot::AHordeLoot()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionObjectType(COLLISION_LOOT);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetSphereRadius(200.0f);
	RootComponent = SphereComp;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetCollisionObjectType(ECC_WorldStatic);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	BoxComp->SetupAttachment(SphereComp);

	LineTraceComp = CreateDefaultSubobject<UBoxComponent>(TEXT("LineTraceComp"));
	LineTraceComp->SetCollisionObjectType(COLLISION_LOOT);
	LineTraceComp->SetCollisionResponseToAllChannels(ECR_Block);
	LineTraceComp->SetupAttachment(SphereComp);

	SphereComp->SetHiddenInGame(false);
	BoxComp->SetHiddenInGame(false);
	LineTraceComp->SetHiddenInGame(false);

	//MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//MeshComp->SetupAttachment(SphereComp);

	//ImpulseRange = 200.0f;
	//ImpulseHeight = 700.0f;
}

// Called when the game starts or when spawned
void AHordeLoot::BeginPlay()
{
	Super::BeginPlay();

	//SphereComp->AddImpulse(FVector(FMath::RandRange(-ImpulseRange, ImpulseRange), FMath::RandRange(-ImpulseRange, ImpulseRange), ImpulseHeight), NAME_None, true);

	if (Role == ROLE_Authority)
	{
	}
}

void AHordeLoot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{
		if (OtherActor)
		{
			//FTimerDelegate TimerDel;
			//TimerDel.BindUFunction(this, FName("TraceLine"), OtherActor);
			//GetWorld()->GetTimerManager().SetTimer(TraceLineTimerHandle, TimerDel, 0.5f, true);
		}
	}
}

void AHordeLoot::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{
		if (OtherActor)
		{
			GetWorld()->GetTimerManager().ClearTimer(TraceLineTimerHandle);
		}
	}
}

void AHordeLoot::TraceLine(AActor* OtherActor)
{
	//// Aim Dir
	//FVector AimDir = FVector::ZeroVector;
	//FVector CamLoc;
	//FRotator CamRot;
	//OtherActor->GetActorEyesViewPoint(CamLoc, CamRot);
	//AimDir = CamRot.Vector();

	//// Start Trace 
	//FVector StartTrace = FVector::ZeroVector;
	//// Use player's camera
	//FRotator UnusedRot;
	//OtherActor->GetActorEyesViewPoint(StartTrace, UnusedRot);
	//// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
	//StartTrace = StartTrace + AimDir * ((OtherActor->GetActorLocation() - StartTrace) | AimDir);

	//const FVector EndTrace = StartTrace + (AimDir * 1000);

	//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::White, false, 0.5f, 0, 2.0f);

	//// Perform trace to retrieve hit info
	//FCollisionQueryParams TraceParams(FName(TEXT("Loot_Trace")), true);
	//TraceParams.bTraceAsyncScene = true;

	//FHitResult Hit(ForceInit);
	//if (ActorLineTraceSingle(Hit, StartTrace, EndTrace, ECC_WorldStatic, TraceParams))
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *Hit.GetComponent()->GetName()));
	//	if (Hit.GetComponent() == BoxComp)
	//	{
	//		Destroy();
	//	}
	//}
}
