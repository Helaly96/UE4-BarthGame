// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "SGameMode.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	bIsDead = false;
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	//Only make the server react to damage
	//Clients shouldn't be incharge of making damage

	//Componens don't have roles but actors do
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			UE_LOG(LogTemp, Warning, TEXT("Called once"));
			//Subscribe to the player actor
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
		}
	}
	
	

	DefaultHealth = Health;
	
}


void UHealthComponent::TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage < 0 || bIsDead)
	{

	}
	else
	{
		Health = Health - Damage;
		//UE_LOG(LogTemp, Warning, TEXT(" Damage is %f"),Damage);
		//UE_LOG(LogTemp, Warning, TEXT(" Health is %f"), Health);
		Health = FMath::Clamp(Health, 0.0f, DefaultHealth);
	}

	bIsDead = Health <= 0;
	//UE_LOG(LogTemp, Warning, TEXT("TookDamage"));
	ChangeHelath.Broadcast(this,Health,Damage, DamageType , InstigatedBy, DamageCauser);
	

	if (Health <= 0)
	{
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser , InstigatedBy);
		}

	}
}


//On Rep for floats, we can have old value as parameters
void UHealthComponent::OnRepHealth(float OldHealth)
{

	ChangeHelath.Broadcast(this, Health, OldHealth-Health , nullptr, nullptr, nullptr);
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHealthComponent, Health);
	//DOREPLIFETIME(ASCharacter, SecoundaryWeapon);
}
