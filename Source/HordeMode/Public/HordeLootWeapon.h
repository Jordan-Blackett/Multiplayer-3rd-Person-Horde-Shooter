// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/HordeLoot.h"
#include "HordeLootWeapon.generated.h"

class AHordeWeapon;
class USkeletalMesh;

/**
 * 
 */
UCLASS()
class HORDEMODE_API AHordeLootWeapon : public AHordeLoot
{
	GENERATED_BODY()
	
public:
	AHordeLootWeapon();

	void AddImpluse(FVector force);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* BaseMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BarrelMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StockMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GripMeshComp;

	UPROPERTY()
	AHordeWeapon* WeaponClass;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	AHordeWeapon* GetWeaponClass();
	void SetWeaponClass(AHordeWeapon* NewWeaponClass);

	//////////////////////////////////////////////////////////////////////////
	//  Weapon Construction - 
	void SetWeaponBaseMesh(USkeletalMesh* BaseMesh);
	void SetWeaponBarrelMesh(UStaticMesh* BarrelMesh);
	void SetWeaponStockMesh(UStaticMesh* StockMesh);
	void SetWeaponGripMesh(UStaticMesh* GripMesh);
};
