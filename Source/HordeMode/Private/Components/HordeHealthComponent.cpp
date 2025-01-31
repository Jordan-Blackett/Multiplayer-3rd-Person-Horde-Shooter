// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "HordeGameMode.h"
#include "Player/HordeCharacter.h"

// Sets default values for this component's properties
UHordeHealthComponent::UHordeHealthComponent()
{
	DefaultHealth = 100;
	bIsDead = false;

	TeamNum = 255;
	bCanDamageSelf = false;

	SetIsReplicated(true);
}

// Called when the game starts
void UHordeHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHordeHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}

void UHordeHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;

	// Hitmarker?


	// HUD
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UHordeHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	//UE_LOG(LogTemp, Log, TEXT("Damage %s"), *FString::SanitizeFloat(Damage));
	//if (IsFriendly(DamagedActor, DamageCauser))
	//{
	//	return;
	//}

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		AHordeGameMode* GM = Cast<AHordeGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}

	// Hitmarker
	Cast<AHordeCharacter>(DamageCauser)->ClientHitMarker();
}

//void UHordeHealthComponent::FloatingDamageText(float Damage, bool DamageType)
//{
//	if (FloatingDamageTextWidgetTemplate != nullptr)
//	{
//		UParagonWidget_FloatingDamageText* FloatingText = CreateWidget<UParagonWidget_FloatingDamageText>(GetWorld(), FloatingDamageTextWidgetTemplate);
//		if (FloatingText) {
//			FloatingText->SetDamageValue(FMath::RoundToInt(Damage));
//			FloatingText->DamageType(DamageType);
//
//			// Get hit position to screen
//			APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
//			//const APlayerController* const PlayerController = Cast<const APlayerController>(GetController());
//
//			if (PlayerController)
//			{
//				FVector2D ScreenLocation; //PlayerController->GetPawn()->GetActorLocation()
//				PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenLocation);
//
//				int32 ScreenWidth = 0;
//				int32 ScreenHeight = 0;
//				PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);
//
//				FloatingText->SetInitialScreenLocation(ScreenLocation);
//			}
//
//			FloatingText->Init();
//			FloatingText->AddToViewport();
//		}
//	}
//}

void UHordeHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

bool UHordeHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// Assume Friendly
		return true;
	}

	UHordeHealthComponent* HealthCompA = Cast<UHordeHealthComponent>(ActorA->GetComponentByClass(UHordeHealthComponent::StaticClass()));
	UHordeHealthComponent* HealthCompB = Cast<UHordeHealthComponent>(ActorB->GetComponentByClass(UHordeHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// Assume friendly
		return true;
	}

	if (HealthCompA == HealthCompB)
	{
		return !HealthCompA->bCanDamageSelf;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}

float UHordeHealthComponent::GetHealth() const
{
	return Health;
}

void UHordeHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHordeHealthComponent, Health);
}