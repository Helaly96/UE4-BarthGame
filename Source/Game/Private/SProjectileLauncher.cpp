// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileLauncher.h"
#include "Engine/World.h"
#include"Components/SkeletalMeshComponent.h"

void ASProjectileLauncher::Fire()
{

	AActor* MyOwner = GetOwner();

	if (MyOwner->Role == ROLE_Authority)
	{
		if (MyOwner)
		{
			UE_LOG(LogTemp, Warning, TEXT("RPG Fire"));
			FVector EyeLocation;
			FRotator EyeRotation;

			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FVector MuzzeLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			FRotator MuzzleRotation = MeshComp->GetSocketRotation(MuzzleSocketName);
			FActorSpawnParameters params;
			params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzeLocation, EyeRotation, params);

		}
	}	

	else
	{
		ServerFire();
	}

}