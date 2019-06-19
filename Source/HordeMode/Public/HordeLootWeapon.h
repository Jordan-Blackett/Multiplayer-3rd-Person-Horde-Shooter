// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/HordeLoot.h"
#include "HordeLootWeapon.generated.h"

class AHordeWeapon;

/**
 * 
 */
UCLASS()
class HORDEMODE_API AHordeLootWeapon : public AHordeLoot
{
	GENERATED_BODY()
	
public:
	AHordeLootWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	TSubclassOf<AHordeWeapon> WeaponClass;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	TSubclassOf<AHordeWeapon> GetWeaponClass();
};
