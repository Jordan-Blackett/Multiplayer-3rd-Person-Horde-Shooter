// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeWeapon_Projectile.h"
#include "Components/SkeletalMeshComponent.h"


//void AHordeWeapon_Projectile::Fire()
//{
//	AActor* MyOwner = GetOwner();
//	if (MyOwner && ProjectileClass)
//	{
//		FVector EyeLocation;
//		FRotator EyeRotation;
//		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
//
//		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
//		//FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);
//
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
//	}
//
//}
