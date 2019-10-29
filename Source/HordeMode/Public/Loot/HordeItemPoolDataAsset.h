// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "HordeItemPoolDataAsset.generated.h"

class UHordeItemPoolDataAsset;
class UHordeItemPoolWeaponPartDataAsset;
class UHordeLootTableComponent;
class UHordeWeaponDataAsset;

UENUM(BlueprintType)
enum class EWeights : uint8 {
	ENone,
	ECommon,
	EUncommon,
	ERare,
	EVeryRare,
	ELegendary
};

FORCEINLINE const float& operator+=(float& currentTickets, const EWeights& weight)
{
	float tickets = 0;
	switch (weight) {
		case EWeights::ENone: tickets = 0.0f; break;
		case EWeights::ECommon: tickets = 100.0f; break;
		case EWeights::EUncommon: tickets = 10.0f; break;
		case EWeights::ERare: tickets = 1.0f; break;
		case EWeights::EVeryRare: tickets = 0.1f; break;
		case EWeights::ELegendary: tickets = 0.01f; break;
	}
	return currentTickets += tickets;
}

FORCEINLINE const float operator*(const EWeights& weight, float WeightModifier)
{
	float tickets = 0;
	switch (weight) {
	case EWeights::ENone: tickets = 0.0f; break;
	case EWeights::ECommon: tickets = 100.0f; break;
	case EWeights::EUncommon: tickets = 10.0f; break;
	case EWeights::ERare: tickets = 1.0f; break;
	case EWeights::EVeryRare: tickets = 0.1f; break;
	case EWeights::ELegendary: tickets = 0.01f; break;
	}
	WeightModifier *= tickets;
	return WeightModifier;
}

USTRUCT()
struct FItemPoolData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	float Probability;

	UPROPERTY(EditDefaultsOnly)
	EWeights Weight;

	UPROPERTY(EditDefaultsOnly)
	float WeightModifier;
};

USTRUCT()
struct FItemPoolPoolData : public FItemPoolData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHordeItemPoolDataAsset> ItemPool;
};

USTRUCT()
struct FItemPoolWeaponTypePoolData : public FItemPoolData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHordeWeaponDataAsset> WeaponPart;
};

/**
 * 
 */
UCLASS()
class HORDEMODE_API UHordeItemPoolDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual int SelectLoot(UHordeLootTableComponent* LootTable) { return 1; };

};

UCLASS(Blueprintable)
class HORDEMODE_API UHordeItemPoolsDataAsset : public UHordeItemPoolDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ItemPools")
	TArray<FItemPoolPoolData> ItemPools;

	virtual int SelectLoot(UHordeLootTableComponent* LootTable) override; //AHordeLoot

private:

	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent);


};

UCLASS(Blueprintable)
class HORDEMODE_API UHordeItemPoolWeaponPartDataAsset : public UHordeItemPoolDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "ItemPools")
	TArray<FItemPoolWeaponTypePoolData> ItemPools;

	TSubclassOf<UHordeWeaponDataAsset>* SelectLootParts(UHordeLootTableComponent* LootTable);

private:

	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent);

};

UCLASS(Blueprintable)
class HORDEMODE_API UHordeItemPoolWeaponDataAsset : public UHordeItemPoolsDataAsset
{
	GENERATED_BODY()

public:
	virtual int SelectLoot(UHordeLootTableComponent* LootTable) override;

private:
	int ConstructWeapon(UHordeLootTableComponent* LootTable, TArray<TSubclassOf<UHordeWeaponDataAsset>*> Parts);

};
