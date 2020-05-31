// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "Engine/World.h"


ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2;
	WaveCount = 0;
}

void ASGameMode::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* PC = it->Get();
		if (PC && (PC->GetPawn() == nullptr) )
		{
			PlayerToRevive = PC;
			FTimerHandle Handler;
			Revive();
			//GetWorldTimerManager().SetTimer(Handler, this, &ASGameMode::Revive, 2.0f, false, 5.0f);
		}
	}
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RestartDeadPlayers();

}

void ASGameMode::Revive()
{
	if (PlayerToRevive)
	{
		RestartPlayer(PlayerToRevive);
	}
	
}

void ASGameMode::StartWave()
{
	NoOfBots = 2 * WaveCount;
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBot, this, &ASGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnBot);
	WaveCount++;
	PrepareForNextWave();
}

void ASGameMode::StartPlay()
{
	Super::StartPlay();
	PrepareForNextWave();
}

void ASGameMode::PrepareForNextWave()
{
	FTimerHandle TimerHandle_NextWaveStart;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, 1.0f, false, 0.0f);
}


void ASGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();
	NoOfBots--;
	if (NoOfBots <= 0)
	{
		EndWave();
	}
}