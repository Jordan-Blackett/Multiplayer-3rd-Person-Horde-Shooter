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
//
USTRUCT()
struct FWeaponPartData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	float Probability;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
	UStaticMesh* GripMesh;

	/** defaults */
	FWeaponPartData()
	{
		Probability = 0.0f;
		GripMesh = nullptr;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORDEMODE_API UHordeLootTable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHordeLootTable();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Parts")
	TArray<FWeaponPartData> WeaponPartsBarrels;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
