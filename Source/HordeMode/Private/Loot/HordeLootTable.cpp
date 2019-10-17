// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLootTable.h"
#include "Engine/World.h"
#include "HordeWeapon.h"
#include "HordeWeapon_HitScan.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHordeLootTable::UHordeLootTable()
{

}

// Called when the game starts
void UHordeLootTable::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// Generate Probability

	// Assault Rifles
	SetWeaponPoolProbability(WeaponAssaultRiflePool);	
}

void UHordeLootTable::GenerateLoot()
{
	float Roll = FMath::FRand();

	if (Roll <= WeaponAssaultRiflePool.Probability)
	{
		if (WeaponAssaultRiflePool.WeaponPartsBase.Num() > 0)
		{
			FSelectedWeaponParts SelectedParts(WeaponAssaultRiflePool);
			ContructWeapon(SelectedParts);
		}
		else {
			// Spawn Default Assault
		}
	}


	//SpawnLoot(targetLoction, struct);
}

void UHordeLootTable::SetWeaponPoolProbability(FWeaponPoolData & Pool)
{
	Pool.Probability = 1;

	//SetWeaponPartsPoolProbality(Pool.WeaponBase);
	//SetWeaponPartsPoolProbality(Pool.WeaponPartsBarrels);
}

template <typename T>
void UHordeLootTable::SetWeaponPartsPoolProbality(TArray<T>& Pool)
{
	float PartProbability = 1 / Pool.Num();
	for (T Parts : Pool)
	{
		Parts.Probability = PartProbability;
	}
}

void UHordeLootTable::ContructWeapon(FSelectedWeaponParts& SelectedParts)
{
	// Stats

	FVector NewLocation = FVector(0.f, 0.f, 50.f);
	FActorSpawnParameters SpawnInfo;
	AHordeWeapon* NewWeapon = GetWorld()->SpawnActor<AHordeWeapon>(AHordeWeapon::StaticClass(), NewLocation, FRotator::ZeroRotator, SpawnInfo);
	if (NewWeapon)
	{
		if (SelectedParts.SelectedBase.BaseMesh)
			NewWeapon->SetWeaponBaseMesh(SelectedParts.SelectedBase.BaseMesh);

		if (SelectedParts.SelectedBarrelPart.PartMesh)
			NewWeapon->SetWeaponBarrelMesh(SelectedParts.SelectedBarrelPart.PartMesh);

		if (SelectedParts.SelectedStockPart.PartMesh)
			NewWeapon->SetWeaponStockMesh(SelectedParts.SelectedStockPart.PartMesh);

		if (SelectedParts.SelectedGripPart.PartMesh)
			NewWeapon->SetWeaponGripMesh(SelectedParts.SelectedGripPart.PartMesh);
	}
}