// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include"Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include"Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Game.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include"TimerManager.h"


// contatin info of a scan weapon line trace

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonCameraComponent"));
	RootComponent = MeshComp;
	MuzzleSocketName = "MuzzleSocket";

	SetReplicates(true);
	NetUpdateFrequency = 66;
	MinNetUpdateFrequency = 33;

}



void ASWeapon::PlayFireEffects(FVector TraceEnd)


{

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);

	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation("MuzzleSocket");
	if (TracerEffect)
	{
		UParticleSystemComponent* SmokeTrail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (SmokeTrail)
		{
		
				SmokeTrail->SetVectorParameter("Target", TraceEnd);

		}

	}

	APawn* PawnOwner = Cast<APawn>(MyOwner);

	if (PawnOwner)
	{
		APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());

		if (PC)
		{
			PC->ClientPlayCameraShake(ShakeCamera);

		}
		
	}
}

void ASWeapon::StartFire()
{
	
	//GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire,1,true,0);
	Fire();
}

void ASWeapon::EndFire()
{
	//GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void ASWeapon::Fire()
{

	
	if (Role < ROLE_Authority)
	{
		ServerFire();
	}


	MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		params.AddIgnoredActor(MyOwner);
		params.bTraceComplex = true;
		params.bReturnPhysicalMaterial=true;

		FVector Direction = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (Direction * 10000);
		FHitResult Hit;
		
		EPhysicalSurface Surface = EPhysicalSurface::SurfaceType11;

		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd , COLLISION_WEAPON, params))
		{
			AActor * Hitted = Hit.GetActor();

			TraceEnd = Hit.ImpactPoint;

			Surface = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			HitScanTrace.Surface = Surface;
			if (Surface == FleshTypeVulnerable)
			{
				UGameplayStatics::ApplyPointDamage(Hitted, 20*2, Direction, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			}
			else
			{
				UGameplayStatics::ApplyPointDamage(Hitted, 20, Direction, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			}

		}

		PlayImpactEffect(Surface, MeshComp->GetSocketLocation(MuzzleSocketName), TraceEnd);

		PlayFireEffects(TraceEnd);

		if (Role == ROLE_Authority)
		{
			
			HitScanTrace.TraceTo = TraceEnd;
			HitScanTrace.Surface = Surface;
			
		}
		
	}
}

//(REQUIRED)implementation of the server fire fn
void ASWeapon::ServerFire_Implementation()
{
	Fire();
}

//(REQUIRED)validation
bool ASWeapon::ServerFire_Validate()
{

	return true;
}

void ASWeapon::HRS()
{
	//Play Cosmetic Effects
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffect(HitScanTrace.Surface, MeshComp->GetSocketLocation(MuzzleSocketName) , HitScanTrace.TraceTo);
}

void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(ASWeapon, HitScanTrace);
	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
}

void ASWeapon::PlayImpactEffect(EPhysicalSurface Surfacetype, FVector TraceFrom , FVector TraceEnd)
{
	UParticleSystem* Selected_Particle;
	switch (Surfacetype)
	{
	case FleshTypeDefault:
		Selected_Particle = FleshImpactEffect;
		break;

	case FleshTypeVulnerable:
		Selected_Particle = HeadShotImpactEffect;
		break;

	default:
		Selected_Particle = DefaultImpactEffect;
		break;
	}
	FVector ShotDirection = TraceEnd - TraceFrom;
	ShotDirection.Normalize();

	if (Selected_Particle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Selected_Particle, TraceEnd, ShotDirection.Rotation());
}



/*
What's happening in HitScanTrace.
when we are the server.

Our Character Starts to Fire(), and since we have the role Authotiy, we continue with the funcution that applies damage and
then makes the damage, and in the process we change HitScanTrace.

Changin the HitTrace Causes the replication to happen.
so the server sees a change in HitTrace1 for Actor 1, so it plays the fire effects.
and Same For Actor2, the other player sees it, because it's called by the server.
*/