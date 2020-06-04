// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SGameState.generated.h"

USTRUCT(BlueprintType)
struct FCurrentScore
{
	GENERATED_BODY()
public:

	FCurrentScore()
	{
		BlueScore   = 0;
		OrangeScore = 0;
	}

	UPROPERTY(BlueprintReadWrite)
		int BlueScore;
	UPROPERTY(BlueprintReadWrite)
		int OrangeScore;
};

UCLASS()
class GAME_API ASGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_ChangeScore)
	FCurrentScore MatchScore;
	
	UFUNCTION()
	void OnRep_ChangeScore();


	void SetScore(FCurrentScore MScore);

	UFUNCTION(BlueprintImplementableEvent)
	void ScoreChanged(FCurrentScore MScore);
};
