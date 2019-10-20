// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HordeLootTable.generated.h"

//#include "HordeWeaponPartStruct.h"

//UENUM(BlueprintType)
//enum class EAmmoType : uint8 {
//	ENone,
//	EPistol,
//	EAssaultRifle,
//	EShotgun,
//	ESniper,
//	EExplosive,
//};

class UHordeLootTable;
class AHordeWeapon;
class USkeletalMesh;
class UHordeWeaponDataAsset;


USTRUCT()
struct FLootTableData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	float Probability;

	UPROPERTY(EditDefaultsOnly)
	float ExtraProbability;

	FLootTableData()
	{
		Probability = 1.0f;
		ExtraProbability = 0.0f;
	}
};

USTRUCT()
struct FWeaponPartData : public FLootTableData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHordeWeaponDataAsset> WeaponPartData;
};

USTRUCT()
struct FWeaponPoolData : public FLootTableData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FWeaponPartData> WeaponPartsBase;

	UPROPERTY(EditDefaultsOnly)
	TArray<FWeaponPartData> WeaponPartsBarrels;

	UPROPERTY(EditDefaultsOnly)
	TArray<FWeaponPartData> WeaponPartsStocks;

	UPROPERTY(EditDefaultsOnly)
	TArray<FWeaponPartData> WeaponPartsGrip;

	UPROPERTY(EditDefaultsOnly)
	TArray<FWeaponPartData> WeaponPartsSight;

	UPROPERTY(EditDefaultsOnly)
	TArray<FWeaponPartData> WeaponPartsAccessories;
};

USTRUCT()
struct FRarityData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Probability;

	UPROPERTY(EditDefaultsOnly)
	float ExtraProbability;

	UPROPERTY(EditDefaultsOnly)
	float StatDelta;
};

USTRUCT()
struct FSelectedWeaponParts {

	GENERATED_USTRUCT_BODY()

	FWeaponPartData SelectedBase;
	FWeaponPartData SelectedBarrelPart;
	FWeaponPartData SelectedStockPart;
	FWeaponPartData SelectedGripPart;

	FSelectedWeaponParts() { ; }
	FSelectedWeaponParts(FWeaponPoolData& Pool)
	{
		if (Pool.WeaponPartsBase.Num() > 0)
			SelectedBase = SelectLootFromWeaponPartsPool(Pool.WeaponPartsBase);
		if (Pool.WeaponPartsBarrels.Num() > 0)
			SelectedBarrelPart = SelectLootFromWeaponPartsPool(Pool.WeaponPartsBarrels);
		if (Pool.WeaponPartsStocks.Num() > 0)
			SelectedStockPart = SelectLootFromWeaponPartsPool(Pool.WeaponPartsStocks);
		if (Pool.WeaponPartsGrip.Num() > 0)
			SelectedGripPart = SelectLootFromWeaponPartsPool(Pool.WeaponPartsGrip);
	}

	FWeaponPartData SelectLootFromWeaponPartsPool(TArray<FWeaponPartData>& Pool)
	{
		float Roll = FMath::FRand();

		uint8 Len = Pool.Num();
		for (uint8 i = 0; i < Len; ++i)
		{
			if (Roll <= Pool[i].Probability)
			{
				return Pool[i];
			}
		}

		return Pool[0];
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORDEMODE_API UHordeLootTable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHordeLootTable();

	UPROPERTY(EditDefaultsOnly, Category = "Rarity")
	TArray<FRarityData> RarityPool;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parts")
	FWeaponPoolData WeaponAssaultRiflePool;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parts")
	FWeaponPoolData WeaponSMGPool;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Base")
	TSubclassOf<AHordeWeapon> AssaultRifleBaseWeapon;

public:
	UFUNCTION(BlueprintCallable, Category = "LootTable")
	void GenerateLoot();

private:
	void SetWeaponPoolProbability(FWeaponPoolData& Pool);
	
	void SetWeaponPartsPoolProbality(TArray<FWeaponPartData>& Pool);

	void ContructWeapon(FSelectedWeaponParts& SelectedParts);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
