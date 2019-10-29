// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HordeLootTableComponent.generated.h"

class UHordeItemPoolsDataAsset;
class UHordeWeaponDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORDEMODE_API UHordeLootTableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHordeLootTableComponent();

	UPROPERTY(EditDefaultsOnly, Category = "LootTable")
	TSubclassOf<UHordeItemPoolsDataAsset> LootTable;

public:
	UFUNCTION(BlueprintCallable, Category = "LootTable")
	void GenerateLoot(FVector LootSpawnPoint, int Level);

	void testfunction(TArray<TSubclassOf<UHordeWeaponDataAsset>*> parts);

private:

	//void ContructWeapon(FSelectedWeaponParts& SelectedParts);
};
