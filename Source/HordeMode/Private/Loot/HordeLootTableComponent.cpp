// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeLootTableComponent.h"
#include "HordeItemPoolDataAsset.h"
#include "Engine/World.h"
#include "HordeWeaponPartDataAsset.h"
#include "HordeLootWeapon.h"

// Sets default values for this component's properties
UHordeLootTableComponent::UHordeLootTableComponent()
{
	// ...
}

void UHordeLootTableComponent::GenerateLoot(FVector LootSpawnPoint, int Level)
{
	float ProbabilityTemp = 0;
	uint32 arrayNum = LootTable.GetDefaultObject()->ItemPools.Num();
	for (uint32 index = 0; index < arrayNum; ++index)
	{
		float LootRoll = FMath::FRand();
		ProbabilityTemp += (LootTable.GetDefaultObject()->ItemPools[index].Probability / 100);
		if (LootRoll <= ProbabilityTemp)
		{
			if (LootTable.GetDefaultObject()->ItemPools[index].ItemPool)
			{
				Cast<UHordeItemPoolsDataAsset>(LootTable.GetDefaultObject()->ItemPools[index].ItemPool.GetDefaultObject())->SelectLoot(this);
			}
		}
	}
}

void UHordeLootTableComponent::testfunction(TArray<TSubclassOf<UHordeWeaponDataAsset>*> parts)
{
	// Spawn LootWeapon
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	float RollX = FMath::FRand();
	float RollY = FMath::FRand();
	FVector NewLocation = FVector(1450.f * RollX, 1450.f * RollY, 50.f);
	AHordeLootWeapon* NewLootWeapon = GetWorld()->SpawnActor<AHordeLootWeapon>(AHordeLootWeapon::StaticClass(), NewLocation, FRotator::ZeroRotator, SpawnInfo);
	if (NewLootWeapon)
	{
		uint32 PartsNum = parts.Num();
		// Base
		if (PartsNum > 0)
		{
			USkeletalMesh* BaseMesh = parts[0]->GetDefaultObject()->GetWeaponPartMesh();
			if (BaseMesh) {
				NewLootWeapon->SetWeaponBaseMesh(BaseMesh);
			}

			NewLootWeapon->SetWeaponClass(Cast<UHordeWeaponPartBaseDataAsset>(parts[0]->GetDefaultObject())->GetWeaponBaseWeapon());
		}

		FPartDeltaData WeaponDelta;

		// Barrel
		if (PartsNum > 1)
		{
			USkeletalMesh* BarrelMesh = parts[1]->GetDefaultObject()->GetWeaponPartMesh();
			if (BarrelMesh) {
				NewLootWeapon->SetWeaponBarrelMesh(BarrelMesh);
			}

			//parts[1]->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
			FPartDeltaData* BarrelStatDelta = Cast<UHordeWeaponPartDataAsset>(parts[1]->GetDefaultObject())->GetPartDeltaData();
			if (BarrelStatDelta) {
				WeaponDelta += *BarrelStatDelta;
			}
		}

		// Stock
		if (PartsNum > 2) {
			USkeletalMesh* StockMesh = parts[2]->GetDefaultObject()->GetWeaponPartMesh();
			if (StockMesh) {
				NewLootWeapon->SetWeaponStockMesh(StockMesh);
			}

			FPartDeltaData* StockStatDelta = Cast<UHordeWeaponPartDataAsset>(parts[2]->GetDefaultObject())->GetPartDeltaData();
			if (StockStatDelta) {
				WeaponDelta += *StockStatDelta;
			}
		}

		// Grip
		if (PartsNum > 3) {
			USkeletalMesh* GripMesh = parts[3]->GetDefaultObject()->GetWeaponPartMesh();
			if (GripMesh) {
				NewLootWeapon->SetWeaponGripMesh(GripMesh);
			}

			FPartDeltaData* GripStatDelta = Cast<UHordeWeaponPartDataAsset>(parts[3]->GetDefaultObject())->GetPartDeltaData();
			if (GripStatDelta) {
				WeaponDelta += *GripStatDelta;
			}
		}

		// Stats
		//NewLootWeapon->SetWeaponDeltaStats(&WeaponDelta);

		//return 1;
	}
}
