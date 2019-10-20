// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLootTable.h"
#include "Engine/World.h"
#include "HordeWeapon.h"
#include "HordeWeapon_HitScan.h"
#include "HordeLootWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "HordeWeaponPartDataAsset.h"

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
	if (SelectedParts.SelectedBase.WeaponPartData)
	{
		TSubclassOf<AHordeWeapon> NewWeaponBase = SelectedParts.SelectedBase.WeaponPartData->GetDefaultObject<UHordeWeaponPartBaseDataAsset>()->GetWeaponBaseWeapon();
		if (NewWeaponBase)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			// Spawn Weapon
			AHordeWeapon* NewWeapon = GetWorld()->SpawnActor<AHordeWeapon>(NewWeaponBase, SpawnInfo);

			// Spawn LootWeapon
			float RollX = FMath::FRand();
			float RollY = FMath::FRand();
			FVector NewLocation = FVector(1450.f * RollX, 1450.f * RollY, 50.f);
			AHordeLootWeapon* NewLootWeapon = GetWorld()->SpawnActor<AHordeLootWeapon>(AHordeLootWeapon::StaticClass(), NewLocation, FRotator::ZeroRotator, SpawnInfo);

			if (NewWeapon && NewLootWeapon)
			{
				// Base
				USkeletalMesh* BaseMesh = SelectedParts.SelectedBase.WeaponPartData->GetDefaultObject<UHordeWeaponPartBaseDataAsset>()->GetWeaponPartMesh();
				if (BaseMesh) {
					NewWeapon->SetWeaponBaseMesh(BaseMesh);
					NewLootWeapon->SetWeaponBaseMesh(BaseMesh);
				}

				NewWeapon->GetWeaponConfig();
				FPartDeltaData WeaponDelta;

				// Barrel
				if (SelectedParts.SelectedBarrelPart.WeaponPartData) {
					USkeletalMesh* BarrelMesh = SelectedParts.SelectedBarrelPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
					if (BarrelMesh) {
						NewWeapon->SetWeaponBarrelMesh(BarrelMesh);
						NewLootWeapon->SetWeaponBarrelMesh(BarrelMesh);
					}

					FPartDeltaData* BarrelStatDelta = SelectedParts.SelectedBarrelPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
					if (BarrelStatDelta)
					{
						WeaponDelta += *BarrelStatDelta;
					}
				}

				// Stock
				if (SelectedParts.SelectedStockPart.WeaponPartData) {
					USkeletalMesh* StockMesh = SelectedParts.SelectedStockPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
					if (StockMesh) {
						NewWeapon->SetWeaponStockMesh(StockMesh);
						NewLootWeapon->SetWeaponStockMesh(StockMesh);
					}

					FPartDeltaData* StockStatDelta = SelectedParts.SelectedStockPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
					if (StockStatDelta)
					{
						WeaponDelta += *StockStatDelta;
					}
				}

				// Grip
				if (SelectedParts.SelectedGripPart.WeaponPartData) {
					USkeletalMesh* GripMesh = SelectedParts.SelectedGripPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
					if (GripMesh) {
						NewWeapon->SetWeaponGripMesh(GripMesh);
						NewLootWeapon->SetWeaponGripMesh(GripMesh);
					}

					FPartDeltaData* GripStatDelta = SelectedParts.SelectedGripPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
					if (GripStatDelta)
					{
						WeaponDelta += *GripStatDelta;
					}
				}

				// Stats
				NewWeapon->SetWeaponDeltaStats(&WeaponDelta);

				//
				NewLootWeapon->SetWeaponClass(NewWeapon);
			}
			else
			{
				// Delete
			}
		}
	}
}