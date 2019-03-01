// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HordePickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class AHordePowerupActor;

UCLASS()
class HORDEMODE_API AHordePickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHordePickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<AHordePowerupActor> PowerUpClass;

	AHordePowerupActor* PowerUpInstance;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float CooldownDuration;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	FTransform PowerUpTransform;

	FTimerHandle TimerHandle_RespawnTimer;

	void Respawn();

public:	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};