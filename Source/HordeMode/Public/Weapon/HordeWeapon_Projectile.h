// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HordeWeapon.h"
#include "HordeWeapon_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API AHordeWeapon_Projectile : public AHordeWeapon
{
	GENERATED_BODY()
	
protected:

	virtual void Fire() override;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;
	
};
