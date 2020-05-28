// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
class UHealthComponent; class USoundCue;
#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"
UCLASS()
class GAME_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

	FVector GetNextPathPoint();

	//next point in Navigation path
	FVector NextPathPoint;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly)
	UHealthComponent* HComp;

	UPROPERTY(EditDefaultsOnly)
		float MovementForce;

	UPROPERTY(EditDefaultsOnly)
		bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly)
		float RequiredDistanceToTarget;

	UMaterialInstanceDynamic * MatInst;

	UFUNCTION()
	void HandleTakeDamage(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* ExplosionEffect;

	bool bExplodedAlready = false;

	UPROPERTY(EditDefaultsOnly)
		float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly)
		float ExplosionDamage;

	UPROPERTY(VisibleDefaultsOnly)
	USphereComponent* SphereComp;

	bool StartedSelfDestruct;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* SelfDestructSound;
	UPROPERTY(EditDefaultsOnly)
	USoundCue* ExplosionSound;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	
		
	


};
