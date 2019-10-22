// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HordeImpactEffect.generated.h"

class USoundCue;
enum EPhysicalSurface;

UCLASS()
class HORDEMODE_API AHordeImpactEffect : public AActor
{
	GENERATED_BODY()

	AHordeImpactEffect();

public:
	
	/** default impact FX used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	UParticleSystem* DefaultFX;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category = Visual)
	UParticleSystem* FleshFX;

	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	USoundCue* DefaultSound;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category = Sound)
	USoundCue* FleshSound;

	/** default decal when material specific override doesn't exist */
	//UPROPERTY(EditDefaultsOnly, Category = Defaults)
	//struct FDecalData DefaultDecal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
	FHitResult SurfaceHit;

	/** spawn effect */
	virtual void PostInitializeComponents() override;

protected:

	/** get FX for material type */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** get sound for material type */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;
};
