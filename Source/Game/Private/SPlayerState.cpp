// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
	bHasTeamSet = false;
}
void ASPlayerState::AddScore(int ScoreDelta)
{
	Score += ScoreDelta;
}