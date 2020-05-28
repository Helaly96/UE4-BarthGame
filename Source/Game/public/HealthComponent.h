// Fill out your copyright notice in the Description page of Project Settings.
//Delegates are simply events dispathcers, the first argument is the event name, and what will it take as params follows next, then each event will be defind. and we will broadcast it which means the event happened
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties 
	UHealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature ChangeHelath;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRepHealth,EditDefaultsOnly,BlueprintReadOnly)
	float Health;

	UFUNCTION()
	void OnRepHealth(float OldHealth);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultHealth;


	//This must be added;
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


		
};
