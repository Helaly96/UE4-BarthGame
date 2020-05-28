// Fill out your copyright notice in the Description page of Project Settings.
class UParticleSystem; class USkeletalMeshComponent; class UDamageType; class UCameraShake;

#pragma once
/*Weapon Structs*/
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

//Here we defined a struct that will be shared across the servers, which contains the surfaces and the end point of the bullet
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TEnumAsByte <EPhysicalSurface> Surface;
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class GAME_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

	UFUNCTION(BlueprintCallable)
		virtual void Fire();

	void StartFire();
	void EndFire();
	
	//we call a fn every time this is replicated(Replication=Changed by the server and the server changes it for the other clients)
	UPROPERTY(ReplicatedUsing = HRS)
	FHitScanTrace  HitScanTrace;

	UFUNCTION()
	void HRS();

	//when server fire is called, it will be pushed to server
	//Reliable it will eventually go to server (For gameplay reliable components)
	//confirm it's done, required for server
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();


protected:
	// Called when the game starts or when spawned

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName MuzzleSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* HeadShotImpactEffect;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UParticleSystem* TracerEffect;
	//we didn't make an instance, because unreal wants it that way lol
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UDamageType> DamageType;

	void PlayFireEffects(FVector TraceEnd);

	AActor* MyOwner;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCameraShake> ShakeCamera;
	
	FTimerHandle TimerHandle_TimeBetweenShots;
	float LastTimeFired;
	
	void PlayImpactEffect(EPhysicalSurface Surfacetype, FVector TraceFrom, FVector TraceEnd);


};
