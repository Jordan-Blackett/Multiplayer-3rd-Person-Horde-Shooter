// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HordeLoot.generated.h"

class USphereComponent;
class UBoxComponent;

UCLASS()
class HORDEMODE_API AHordeLoot : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHordeLoot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	float ImpulseRange;

	UPROPERTY(EditDefaultsOnly, Category = "Loot")
	float ImpulseHeight;



public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
};
