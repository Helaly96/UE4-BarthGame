// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"

void ASGameState::OnRep_ChangeScore()
{
	ScoreChanged(MatchScore);
}

void ASGameState::SetScore(FCurrentScore MScore)
{
	if (Role == ROLE_Authority)
	{
		MatchScore = MScore;
		OnRep_ChangeScore();
	}
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASGameState, MatchScore);
	//DOREPLIFETIME(ASCharacter, FPCameraComp);

}