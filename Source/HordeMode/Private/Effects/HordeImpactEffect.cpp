// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeImpactEffect.h"


// Sets default values
AHordeImpactEffect::AHordeImpactEffect()
{
	//bAutoDestroyWhenFinished = true;
}

//void AShooterImpactEffect::PostInitializeComponents()
//{
	//Super::PostInitializeComponents();

	//UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	//EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	//// Show particles
	//UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	//if (ImpactFX)
	//{
	//	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	//}

	//// play sound
	//USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	//if (ImpactSound)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	//}

	//if (DefaultDecal.DecalMaterial)
	//{
	//	FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
	//	RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

	//	UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
	//		SurfaceHit.Component.Get(), SurfaceHit.BoneName,
	//		SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
	//		DefaultDecal.LifeSpan);
	//}
//}

//UParticleSystem* AHordeImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
//{
//	UParticleSystem* ImpactFX = NULL;
//
//	switch (SurfaceType)
//	{
//	//case HORDE_SURFACE_Flesh:		ImpactFX = FleshFX; break;
//	default:						ImpactFX = DefaultFX; break;
//	}
//
//	return ImpactFX;
//}

//USoundCue* AHordeImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
//{
//	USoundCue* ImpactSound = NULL;
//
//	switch (SurfaceType)
//	{
//	//case HORDE_SURFACE_Flesh:		ImpactSound = FleshSound; break;
//	default:						ImpactSound = DefaultSound; break;
//	}
//
//	return ImpactSound;
//}


