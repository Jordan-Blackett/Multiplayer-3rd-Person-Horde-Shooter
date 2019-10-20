// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/HordeWeapon.h"
#include "Weapon/HordeWeapon_HitScan.h"
#include "Weapon/HordeWeapon_Projectile.h"
#include "HordeWeaponPartDataAsset.generated.h"

class USkeletalMesh;

/**
 * 
 */
UCLASS(Blueprintable)
class HORDEMODE_API UHordeWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* PartMesh;
	
public:
	USkeletalMesh* GetWeaponPartMesh() { return PartMesh; }
};

UCLASS(Blueprintable)
class HORDEMODE_API UHordeWeaponPartBaseDataAsset : public UHordeWeaponDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHordeWeapon> BaseWeapon;

public:
	TSubclassOf<AHordeWeapon> GetWeaponBaseWeapon() { return BaseWeapon; }
};

USTRUCT()
struct FPartDeltaData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	float Damage;

	UPROPERTY(EditDefaultsOnly)
	float Handling;

	UPROPERTY(EditDefaultsOnly)
	float FireRate;

	FORCEINLINE struct FPartDeltaData& operator+=(const FPartDeltaData& other)
	{
		Damage += other.Damage;
		return *this;
	}
};

UCLASS(Blueprintable)
class HORDEMODE_API UHordeWeaponPartDataAsset : public UHordeWeaponDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FPartDeltaData WeaponDelta;

public:
	FPartDeltaData* GetPartDeltaData() { return &WeaponDelta; }
};