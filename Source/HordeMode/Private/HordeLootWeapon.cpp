// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLootWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HordeWeapon.h"

AHordeLootWeapon::AHordeLootWeapon()
{
	GripMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GripMeshComp"));
	GripMeshComp->SetupAttachment(SphereComp);
	
	BaseMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BaseMeshComp"));
	BaseMeshComp->SetupAttachment(GripMeshComp, "BaseSocket");

	BarrelMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BarrelMeshComp"));
	BarrelMeshComp->SetupAttachment(BaseMeshComp, "BarrelSocket");

	StockMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StockMeshComp"));
	StockMeshComp->SetupAttachment(BaseMeshComp, "StockSocket");

	ItemStatCardComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemStatCardComp"));
	ItemStatCardComp->SetupAttachment(SphereComp);
}

void AHordeLootWeapon::AddImpluse(FVector impluse)
{
	SphereComp->AddImpulse(impluse * SphereComp->GetMass());
}

//void AHordeLootWeapon::BeginPlay()
//{
//	AHordeLoot::BeginPlay();
//}

void AHordeLootWeapon::NotifyActorBeginOverlap(AActor * OtherActor)
{
	AHordeLoot::NotifyActorBeginOverlap(OtherActor);

	if (Role == ROLE_Authority)
	{

		//Cast<AHordeCharacter>(OtherActor);


		//TraceLine(OtherActor);

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

		//DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::White, false, 0.1f, 0, 1.0f);

		//FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		//RV_TraceParams.bTraceComplex = true;
		//RV_TraceParams.bTraceAsyncScene = true;
		//RV_TraceParams.bReturnPhysicalMaterial = false;

		//// Perform trace to retrieve hit info
		//FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, Instigator);
		//TraceParams.bTraceAsyncScene = true;
		//TraceParams.bReturnPhysicalMaterial = true;

		//FHitResult Hit(ForceInit);
		//GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

		//OtherActor
		
		
		//Destroy();
	}
}

TSubclassOf<AHordeWeapon> AHordeLootWeapon::GetWeaponClass()
{
	return WeaponClass;
}

void AHordeLootWeapon::SetWeaponClass(TSubclassOf<AHordeWeapon> NewWeaponClass)
{
	WeaponClass = NewWeaponClass;
}

void AHordeLootWeapon::SetWeaponBaseMesh(USkeletalMesh * BaseMesh)
{
	BaseMeshComp->SetSkeletalMesh(BaseMesh);
}

void AHordeLootWeapon::SetWeaponBarrelMesh(USkeletalMesh * BarrelMesh)
{
	BarrelMeshComp->SetSkeletalMesh(BarrelMesh);
}

void AHordeLootWeapon::SetWeaponStockMesh(USkeletalMesh * StockMesh)
{
	StockMeshComp->SetSkeletalMesh(StockMesh);
}

void AHordeLootWeapon::SetWeaponGripMesh(USkeletalMesh * GripMesh)
{
	GripMeshComp->SetSkeletalMesh(GripMesh);
}
