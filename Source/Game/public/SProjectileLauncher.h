// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SProjectileLauncher.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ASProjectileLauncher : public ASWeapon
{
	GENERATED_BODY()


	public:
		virtual void Fire();

		UPROPERTY(EditDefaultsOnly)
			TSubclassOf<AActor> ProjectileClass;
	
};
