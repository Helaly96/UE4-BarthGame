// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CoopGameInstace.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UCoopGameInstace : public UGameInstance
{
	GENERATED_BODY()

	public:
		UCoopGameInstace(const FObjectInitializer& ObjectInitializer);
		virtual void Init();
		FString Name;
		//Console
		UFUNCTION(Exec)
			void Host();

		UFUNCTION(Exec)
			void Join(const FString& address);

		UFUNCTION(Exec)
			void ChangeName(FString Name);
		
		
	
};
