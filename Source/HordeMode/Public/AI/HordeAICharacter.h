// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HordeAICharacter.generated.h"

class UHordeHealthComponent;

UCLASS()
class HORDEMODE_API AHordeAICharacter : public ACharacter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHordeAICharacter();

public:

	UHordeHealthComponent* GetHealthComp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHordeHealthComponent* HealthComp;


};
