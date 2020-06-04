// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled ,AActor*, KillerActor, AActor*, KilledActor,AController *,KillerController);

struct FCurrentScore;
/**
 * 
 */
UCLASS()
class GAME_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnNewBot();


	void SpawnBotTimerElapsed();

	void StartWave();
	void PrepareForNextWave();
	void EndWave();


	void SetMatchScore(FCurrentScore Score);
	void AssignTeams();


	//bots to spawn in a current wave
	float NoOfBots;

	//Number of survived waves
	int WaveCount;

	FTimerHandle TimerHandle_SpawnBot;

	UPROPERTY(EditDefaultsOnly)
	float TimeBetweenWaves;

public:
	void RestartDeadPlayers();
	APlayerController* PlayerToRevive;
	virtual void Tick(float DeltaSeconds) override;
	void Revive();
	ASGameMode();
	virtual void StartPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnActorKilled OnActorKilled;

	int next_assignable_team;
};
