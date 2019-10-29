// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeItemPoolDataAsset.h"
#include "HordeLootTableComponent.h"
#include "HordeWeaponPartDataAsset.h"

int UHordeItemPoolsDataAsset::SelectLoot(UHordeLootTableComponent * LootTable)
{
	float LootRoll = FMath::FRand();
	float ProbabilityTemp = 0;
	uint32 arrayNum = ItemPools.Num();
	for (uint32 index = 0; index < arrayNum; ++index)
	{
		ProbabilityTemp += (ItemPools[index].Probability / 100);
		if (LootRoll <= ProbabilityTemp)
		{
			if (ItemPools[index].ItemPool)
			{
				return ItemPools[index].ItemPool.GetDefaultObject()->SelectLoot(LootTable);
			}
		}
	}

	return 1;
}

void UHordeItemPoolsDataAsset::PostEditChangeChainProperty(FPropertyChangedChainEvent & PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	//FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	////if (PropertyName == GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, ItemPool))
	////{
	float totalTickets = 0.0f;
	uint32 arrayNum = ItemPools.Num();
	for (uint32 Idx = 0; Idx < arrayNum; ++Idx)
	{
		if (ItemPools[Idx].WeightModifier > 0.0f)
			totalTickets += (ItemPools[Idx].Weight * ItemPools[Idx].WeightModifier);
		else
			totalTickets += ItemPools[Idx].Weight;
	}

	float WeightTicket;
	for (uint32 Idx = 0; Idx < arrayNum; ++Idx)
	{
		WeightTicket = 0.0f;
		if (ItemPools[Idx].WeightModifier > 0.0f)
			WeightTicket += (ItemPools[Idx].Weight * ItemPools[Idx].WeightModifier);
		else
			WeightTicket += ItemPools[Idx].Weight;
		ItemPools[Idx].Probability = (WeightTicket / totalTickets) * 100.0f;
	}
	////}
}

TSubclassOf<UHordeWeaponDataAsset>* UHordeItemPoolWeaponPartDataAsset::SelectLootParts(UHordeLootTableComponent * LootTable)
{
	float LootRoll = FMath::FRand();
	float ProbabilityTemp = 0;
	uint32 arrayNum = ItemPools.Num();
	for (uint32 index = 0; index < arrayNum; ++index)
	{
		ProbabilityTemp += (ItemPools[index].Probability / 100);
		if (LootRoll <= ProbabilityTemp)
		{
			if (ItemPools[index].WeaponPart)
			{
				return &ItemPools[index].WeaponPart;
			}
		}
	}

	return nullptr;
}

void UHordeItemPoolWeaponPartDataAsset::PostEditChangeChainProperty(FPropertyChangedChainEvent & PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	////FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	//////if (PropertyName == GET_MEMBER_NAME_CHECKED(UHordeItemPoolDataAsset, ItemPool))
	//////{
	float totalTickets = 0.0f;
	uint32 arrayNum = ItemPools.Num();
	for (uint32 Idx = 0; Idx < arrayNum; ++Idx)
	{
		if (ItemPools[Idx].WeightModifier > 0.0f)
			totalTickets += (ItemPools[Idx].Weight * ItemPools[Idx].WeightModifier);
		else
			totalTickets += ItemPools[Idx].Weight;
	}

	float WeightTicket;
	for (uint32 Idx = 0; Idx < arrayNum; ++Idx)
	{
		WeightTicket = 0.0f;
		if (ItemPools[Idx].WeightModifier > 0.0f)
			WeightTicket += (ItemPools[Idx].Weight * ItemPools[Idx].WeightModifier);
		else
			WeightTicket += ItemPools[Idx].Weight;
		ItemPools[Idx].Probability = (WeightTicket / totalTickets) * 100.0f;
	}
	//////}
}
//
//{
//	
//	//uint32 arrayNum = Parts.Num();
//	//for (uint32 index = 0; index < arrayNum; ++index)
//	//{
//	//	if (Parts[index])
//	//	{
//	//		// Spawn LootWeapon
//	//		FActorSpawnParameters SpawnInfo;
//	//		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	//		float RollX = FMath::FRand();
//	//		float RollY = FMath::FRand();
//	//		FVector NewLocation = FVector(1450.f * RollX, 1450.f * RollY, 50.f);
//
//			//UWorld * World = GEngine->GetWorld();			
//			//GEngine->GetWorldFromContextObject(WorldContextObject);
//			//AHordeLootWeapon* NewLootWeapon = GetWorld()->SpawnActor<AHordeLootWeapon>(AHordeLootWeapon::StaticClass(), NewLocation, FRotator::ZeroRotator, SpawnInfo);
//			//AHordeLootWeapon* Projectile = Cast<AHordeLootWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTM));
//			//if (NewLootWeapon)
//			//{
//			//	// Base
//			//	USkeletalMesh* BaseMesh = Parts[index]->GetDefaultObject()->GetWeaponPartMesh();
//			//	if (BaseMesh) {
//			//		NewLootWeapon->SetWeaponBaseMesh(BaseMesh);
//			//	}
//
//				//NewWeapon->GetWeaponConfig();
//				//FPartDeltaData WeaponDelta;
//
//				// Barrel
//				//if (SelectedParts.SelectedBarrelPart.WeaponPartData) {
//				//	USkeletalMesh* BarrelMesh = SelectedParts.SelectedBarrelPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
//				//	if (BarrelMesh) {
//				//		NewWeapon->SetWeaponBarrelMesh(BarrelMesh);
//				//		NewLootWeapon->SetWeaponBarrelMesh(BarrelMesh);
//				//	}
//
//				//	FPartDeltaData* BarrelStatDelta = SelectedParts.SelectedBarrelPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
//				//	if (BarrelStatDelta)
//				//	{
//				//		WeaponDelta += *BarrelStatDelta;
//				//	}
//				//}
//
//
//
//
//	//		}
//	//	}
//	//}
//
//	//if (SelectedParts.SelectedBase.WeaponPartData)
//	//{
//		//TSubclassOf<AHordeWeapon> NewWeaponBase = SelectedParts.SelectedBase.WeaponPartData->GetDefaultObject<UHordeWeaponPartBaseDataAsset>()->GetWeaponBaseWeapon();
//		//if (NewWeaponBase)
//		//{
//		//	FActorSpawnParameters SpawnInfo;
//		//	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//		//	// Spawn Weapon
//		//	AHordeWeapon* NewWeapon = GetWorld()->SpawnActor<AHordeWeapon>(NewWeaponBase, SpawnInfo);
//
//
//		//	//AHordeWeapon* Projectile = Cast<AHordeWeapon>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, NewWeaponBase, NewLocation));
//
//
//		//		// Base
//		//		USkeletalMesh* BaseMesh = SelectedParts.SelectedBase.WeaponPartData->GetDefaultObject<UHordeWeaponPartBaseDataAsset>()->GetWeaponPartMesh();
//		//		if (BaseMesh) {
//		//			NewWeapon->SetWeaponBaseMesh(BaseMesh);
//		//			NewLootWeapon->SetWeaponBaseMesh(BaseMesh);
//		//		}
//
//		//		NewWeapon->GetWeaponConfig();
//		//		FPartDeltaData WeaponDelta;
//
//		//		// Barrel
//		//		if (SelectedParts.SelectedBarrelPart.WeaponPartData) {
//		//			USkeletalMesh* BarrelMesh = SelectedParts.SelectedBarrelPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
//		//			if (BarrelMesh) {
//		//				NewWeapon->SetWeaponBarrelMesh(BarrelMesh);
//		//				NewLootWeapon->SetWeaponBarrelMesh(BarrelMesh);
//		//			}
//
//		//			FPartDeltaData* BarrelStatDelta = SelectedParts.SelectedBarrelPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
//		//			if (BarrelStatDelta)
//		//			{
//		//				WeaponDelta += *BarrelStatDelta;
//		//			}
//		//		}
//
//		//		// Stock
//		//		if (SelectedParts.SelectedStockPart.WeaponPartData) {
//		//			USkeletalMesh* StockMesh = SelectedParts.SelectedStockPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
//		//			if (StockMesh) {
//		//				NewWeapon->SetWeaponStockMesh(StockMesh);
//		//				NewLootWeapon->SetWeaponStockMesh(StockMesh);
//		//			}
//
//		//			FPartDeltaData* StockStatDelta = SelectedParts.SelectedStockPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
//		//			if (StockStatDelta)
//		//			{
//		//				WeaponDelta += *StockStatDelta;
//		//			}
//		//		}
//
//		//		// Grip
//		//		if (SelectedParts.SelectedGripPart.WeaponPartData) {
//		//			USkeletalMesh* GripMesh = SelectedParts.SelectedGripPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetWeaponPartMesh();
//		//			if (GripMesh) {
//		//				NewWeapon->SetWeaponGripMesh(GripMesh);
//		//				NewLootWeapon->SetWeaponGripMesh(GripMesh);
//		//			}
//
//		//			FPartDeltaData* GripStatDelta = SelectedParts.SelectedGripPart.WeaponPartData->GetDefaultObject<UHordeWeaponPartDataAsset>()->GetPartDeltaData();
//		//			if (GripStatDelta)
//		//			{
//		//				WeaponDelta += *GripStatDelta;
//		//			}
//		//		}
//
//		//		// Stats
//		//		NewWeapon->SetWeaponDeltaStats(&WeaponDelta);
//
//		//		//
//		//		NewLootWeapon->SetWeaponClass(NewWeapon);
//		//	}
//		//	else
//		//	{
//		//		// Delete
//		//	}
//		//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//	return 1;
//}

int UHordeItemPoolWeaponDataAsset::SelectLoot(UHordeLootTableComponent* LootTable)
{
	TArray<TSubclassOf<UHordeWeaponDataAsset>*> SelectedParts;

	uint32 arrayNum = ItemPools.Num();
	for (uint32 index = 0; index < arrayNum; ++index)
	{
		if (ItemPools[index].ItemPool)
		{
			TSubclassOf<UHordeWeaponDataAsset>* part = Cast<UHordeItemPoolWeaponPartDataAsset>(ItemPools[index].ItemPool.GetDefaultObject())->SelectLootParts(LootTable);
			SelectedParts.Add(part);
		}
	}

	return ConstructWeapon(LootTable, SelectedParts);
}

int UHordeItemPoolWeaponDataAsset::ConstructWeapon(UHordeLootTableComponent* LootTable, TArray<TSubclassOf<UHordeWeaponDataAsset>*> Parts)
{
	LootTable->testfunction(Parts);
	return 0;
}
