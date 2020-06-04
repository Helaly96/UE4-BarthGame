// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "SPlayerState.h"
ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2;
	WaveCount = 0;
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	next_assignable_team = 0;
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

void ASGameMode::AssignTeams()
{
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		APlayerController* PC = it->Get();
		ASPlayerState* PS = PC->GetPlayerState<ASPlayerState>();
		if (ensureAlways(PS))
		{
			if (PS->bHasTeamSet == false)
			{
				if (next_assignable_team == 0)
				{
					PS->TeamNo = 0;
					next_assignable_team = 1;
					PS->bHasTeamSet = true;
				}
				else
				{
					PS->TeamNo = 1;
					next_assignable_team = 0;
					PS->bHasTeamSet = true;
				}
					
			}
		}

	}
}

void ASGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	RestartDeadPlayers();
	AssignTeams();

}


void ASGameMode::SetMatchScore(FCurrentScore Score)
{
	ASGameState* CurrentGameState = GetGameState <ASGameState>();

	if (ensureAlways(CurrentGameState))
	{
		CurrentGameState->OnRep_ChangeScore();
	}

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
	//PrepareForNextWave();
	FCurrentScore GameScore;
	SetMatchScore(GameScore);

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