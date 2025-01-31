// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HordePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API AHordePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);
	
};
