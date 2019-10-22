// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HordeItemPoolDataAsset.generated.h"

class UHordeItemPoolDataAsset;

UENUM(BlueprintType)
enum class EWeights : uint8 {
	ENone,
	ECommon,
	EUncommon,
	ERare,
	EVeryRare,
	ELegendary
};

USTRUCT()
struct FItemPoolData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
	float Probability;

	UPROPERTY(EditDefaultsOnly)
	EWeights Weight;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHordeItemPoolDataAsset> ItemPool;
};

USTRUCT()
struct FListItemPoolData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FItemPoolData> ItemPools;
};

/**
 * 
 */
UCLASS(Blueprintable)
class HORDEMODE_API UHordeItemPoolDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	//UPROPERTY(VisibleAnywhere, Category = "Test")
	//float Probability;

	//UPROPERTY(EditDefaultsOnly)
	//float Weight;

	UPROPERTY(EditAnywhere, Category = "Pools")
	FListItemPoolData Pools;

	//UPROPERTY(EditDefaultsOnly)
	//UHordeItemPoolDataAsset* ItemPool;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UHordeItemPoolDataAsset> tertr;

	void PostEditChangeProperty(struct FPropertyChangedEvent& e)
	{
		Super::PostEditChangeProperty(e);

		FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, Pools)) {
			/* Because you are inside the class, you should see the value already changed */

			//TransmissionSetup.ItemPools[0].Probability = 10.f;
			//Probability = Weight;

			//if (MyBool) doThings(); // This is how you access MyBool.
			//else undoThings();

			///* if you want to use bool property */
			//UBoolProperty* prop = static_cast<UBoolProperty*>(e.Property);
			//if (prop->GetPropertyValue())
			//	dothings()
			//else
			//	undothings()
		}
	}

	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
	{
		Super::PostEditChangeChainProperty(PropertyChangedEvent);

		//FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
		//if (PropertyName == GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, TransmissionSetup)) {
		//
		uint32 arrayNum = Pools.ItemPools.Num();
		for (uint32 Idx = 0; Idx < arrayNum; ++Idx)
		{
			//GetArrayIndex()
			Pools.ItemPools[Idx].Probability = 15.f;
		}
	}
};

UCLASS(Blueprintable)
class HORDEMODE_API UHordeListDefinitionDataAsset : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Pools")
	FListItemPoolData TransmissionSetup;
};


//UCLASS(Blueprintable)
//class HORDEMODE_API UHordeWeaponPoolDataAsset : public UHordeItemPoolDataAsset
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditDefaultsOnly, Category = "Pools")
//	int Pools;
//
//};

