// Fill out your copyright notice in the Description page of Project Settings.

#include "HordeGameMode.h"
#include "Engine/World.h"
#include "HordeHealthComponent.h"
#include "HordeGameState.h"
#include "HordePlayerState.h"
#include "TimerManager.h"



AHordeGameMode::AHordeGameMode()
{
	TimeBetweenWaves = 2.0f;

	GameStateClass = AHordeGameState::StaticClass();
	PlayerStateClass = AHordePlayerState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AHordeGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AHordeGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void AHordeGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}

void AHordeGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AHordeGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::WaitingToStart);

	RestartDeadPlayers();
}

void AHordeGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UHordeHealthComponent* HealthComp = Cast<UHordeHealthComponent>(TestPawn->GetComponentByClass(UHordeHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);

		PrepareForNextWave();
	}
}

void AHordeGameMode::CheckAnyPlayerAlive()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHordeHealthComponent* HealthComp = Cast<UHordeHealthComponent>(MyPawn->GetComponentByClass(UHordeHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				// A player is still alive.
				return;
			}
		}
	}

	// No player alive
	GameOver();
}

void AHordeGameMode::GameOver()
{
	EndWave();

	// @TODO: Finish up the match, present 'game over' to players.

	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("GAME OVER! Players Died"));
}

void AHordeGameMode::SetWaveState(EWaveState NewState)
{
	AHordeGameState* GS = GetGameState<AHordeGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void AHordeGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn() == nullptr)
		{
			RestartPlayer(PC);
		}
	}
}

void AHordeGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AHordeGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckAnyPlayerAlive();
}

void AHordeGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}