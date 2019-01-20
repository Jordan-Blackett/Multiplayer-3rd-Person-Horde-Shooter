// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/HordeWeapon.h"
#include "HordeWeapon_HitScan.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API AHordeWeapon_HitScan : public AHordeWeapon
{
	GENERATED_BODY()
	
	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;
	
	
};
