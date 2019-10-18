// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLootTable.h"
#include "Engine/World.h"
#include "HordeWeapon.h"
#include "HordeWeapon_HitScan.h"
#include "HordeLootWeapon.h"
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

	SetWeaponPartsPoolProbality(Pool.WeaponPartsBase);
	SetWeaponPartsPoolProbality(Pool.WeaponPartsBarrels);
	SetWeaponPartsPoolProbality(Pool.WeaponPartsStocks);
	SetWeaponPartsPoolProbality(Pool.WeaponPartsGrip);
}

void UHordeLootTable::SetWeaponPartsPoolProbality(TArray<FWeaponPartData>& Pool)
{
	uint8 Len = Pool.Num();
	float PartProbability = 1.0f / Len;
	for (uint8 i = 0; i < Len; ++i)
	{
		Pool[i].Probability = PartProbability * i;
	}
}

void UHordeLootTable::ContructWeapon(FSelectedWeaponParts& SelectedParts)
{
	AHordeWeapon* NewWeapon = NewObject<AHordeWeapon>();
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

	float RollX = FMath::FRand();
	float RollY = FMath::FRand();
	FVector NewLocation = FVector(1450.f * RollX, 1450.f * RollY, 50.f);
	FActorSpawnParameters SpawnInfo;
	AHordeLootWeapon* NewLootWeapon = GetWorld()->SpawnActor<AHordeLootWeapon>(AHordeLootWeapon::StaticClass(), NewLocation, FRotator::ZeroRotator, SpawnInfo);
	if (NewLootWeapon && NewWeapon)
	{
		NewLootWeapon->SetWeaponClass(NewWeapon);

		if (SelectedParts.SelectedBase.BaseMesh)
			NewLootWeapon->SetWeaponBaseMesh(SelectedParts.SelectedBase.BaseMesh);

		if (SelectedParts.SelectedBarrelPart.PartMesh)
			NewLootWeapon->SetWeaponBarrelMesh(SelectedParts.SelectedBarrelPart.PartMesh);

		if (SelectedParts.SelectedStockPart.PartMesh)
			NewLootWeapon->SetWeaponStockMesh(SelectedParts.SelectedStockPart.PartMesh);

		if (SelectedParts.SelectedGripPart.PartMesh)
			NewLootWeapon->SetWeaponGripMesh(SelectedParts.SelectedGripPart.PartMesh);
	}
}