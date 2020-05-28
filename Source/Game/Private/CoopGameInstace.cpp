// Fill out your copyright notice in the Description page of Project Settings.


#include "CoopGameInstace.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "SCharacter.h"

UCoopGameInstace::UCoopGameInstace(const FObjectInitializer& ObjectInitializer)
{

}

void UCoopGameInstace::Init()
{

}

void UCoopGameInstace::Host()
{
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, "Hosting");
	UWorld* World = GetWorld();

	//means that we are the server
	World->ServerTravel("/Game/Levels/NewMap?listen");
}
void  UCoopGameInstace::Join(const FString& address)
{
	UEngine* Engine = GetEngine();
	Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, "Hosting");
	APlayerController* PC = GetFirstLocalPlayerController();

	if (PC)
	{
		PC->ClientTravel(address, ETravelType::TRAVEL_Absolute);

	}
}

void  UCoopGameInstace::ChangeName(FString Name)
{
	APlayerController* PC = GetFirstLocalPlayerController();
	if (PC)
	{
		APawn* PawnActor = PC->GetPawn();
		if (PawnActor)
		{
			ASCharacter* Player = Cast<ASCharacter,APawn>(PawnActor);

			if (Player)
			{
				Player->TakeNameFromTerminal(Name);
			}
			
		}
	
	}
}
