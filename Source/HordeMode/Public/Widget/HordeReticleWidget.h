// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HordeReticleWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDEMODE_API UHordeReticleWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Reticle")
	void SetReticleDefault();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Reticle")
	void SetReticleAlly();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Reticle")
	void SetReticleEnemy();
};
