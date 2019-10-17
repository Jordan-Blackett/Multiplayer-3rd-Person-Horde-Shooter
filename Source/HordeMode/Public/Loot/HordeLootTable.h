// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HordeLootTable.generated.h"

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
struct FLootTableWeaponData : public FLootTableData
{
	GENERATED_USTRUCT_BODY()

public:
	// Weapon Stats
	float test;
};


USTRUCT()
struct FWeaponBaseData : public FLootTableWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* BaseMesh;
};

USTRUCT()
struct FWeaponPartData : public FLootTableWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* PartMesh;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* BaseMesh;
};

USTRUCT()
struct FWeaponPoolData : public FLootTableWeaponData
{
	GENERATED_USTRUCT_BODY()

	//UPROPERTY(EditDefaultsOnly)
	//TArray<FWeaponBaseData> WeaponBase;

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

	//FWeaponBaseData SelectedBase;
	FWeaponPartData SelectedBase;
	FWeaponPartData SelectedBarrelPart;
	FWeaponPartData SelectedStockPart;
	FWeaponPartData SelectedGripPart;

	FSelectedWeaponParts() { ; }
	FSelectedWeaponParts(FWeaponPoolData& Pool)
	{
		SelectedBase = SelectLootFromWeaponPartsPool(Pool.WeaponPartsBase);
		SelectedBarrelPart = SelectLootFromWeaponPartsPool(Pool.WeaponPartsBarrels);
		SelectedStockPart = SelectLootFromWeaponPartsPool(Pool.WeaponPartsStocks);
		SelectedGripPart = SelectLootFromWeaponPartsPool(Pool.WeaponPartsGrip);
	}

	template <typename T>
	T SelectLootFromWeaponPartsPool(TArray<T>& Pool)
	{
		float Roll = FMath::FRand();
		for (T Part : Pool)
		{
			if (Roll <= Part.Probability)
			{
				return Part;
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


public:
	UFUNCTION(BlueprintCallable, Category = "LootTable")
	void GenerateLoot();

private:
	void SetWeaponPoolProbability(FWeaponPoolData& Pool);
	
	template <typename T>
	void SetWeaponPartsPoolProbality(TArray<T>& Pool);

	void ContructWeapon(FSelectedWeaponParts& SelectedParts);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
