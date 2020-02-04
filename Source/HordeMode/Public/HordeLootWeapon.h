// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/HordeLoot.h"
#include "HordeLootWeapon.generated.h"

class AHordeWeapon;
class USkeletalMesh;
class UWidgetComponent;

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
	//virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* BaseMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* BarrelMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* StockMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* GripMeshComp;

	// TODO: move to lootitem.h
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* ItemStatCardComp;

	UPROPERTY()
	TSubclassOf<AHordeWeapon> WeaponClass;

	//UPROPERTY()
	//TSubclassOf<AHordeWeapon> WeaponDelta;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
	TSubclassOf<AHordeWeapon> GetWeaponClass();
	void SetWeaponClass(TSubclassOf<AHordeWeapon> NewWeaponClass);

	//////////////////////////////////////////////////////////////////////////
	//  Weapon Construction
	void SetWeaponBaseMesh(USkeletalMesh* BaseMesh);
	void SetWeaponBarrelMesh(USkeletalMesh* BarrelMesh);
	void SetWeaponStockMesh(USkeletalMesh* StockMesh);
	void SetWeaponGripMesh(USkeletalMesh* GripMesh);
};
