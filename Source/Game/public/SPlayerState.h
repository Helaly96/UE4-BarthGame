// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable)
		void AddScore(int ScoreDelta);



public:
	ASPlayerState();

	UPROPERTY(BlueprintReadOnly)
	bool bHasTeamSet;
	UPROPERTY(BlueprintReadOnly)
	int TeamNo;
	
};
