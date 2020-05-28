// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthComponent.h"
#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "..\..\Source\Game\public\SCharacter.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"

// Sets default values
ASTrackerBot::ASTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	HComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));

	ExplosionRadius = 200;
	ExplosionDamage = 100;


	//Make the root component our mesh
	RootComponent = MeshComp;

	//not make it not affect the nav mesh
	MeshComp->SetCanEverAffectNavigation(false);
	//because we will apply forces to it
	MeshComp->SetSimulatePhysics(true);
	MovementForce = 2000;
	RequiredDistanceToTarget = 100;

	bUseVelocityChange = true;

	//Subscribe to on Health Change event
	HComp->ChangeHelath.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);
	StartedSelfDestruct = false;


	SphereComp->SetSphereRadius(ExplosionRadius);
	SphereComp->SetupAttachment(RootComponent);
	//SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	//Find initial point
	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();

	}
	
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority)
	{
		float Distance_To_Target = (GetActorLocation() - NextPathPoint).Size();
		if (!(Distance_To_Target <= RequiredDistanceToTarget))
		{
			FVector GetDirection = NextPathPoint - GetActorLocation();
			GetDirection.Normalize();
			GetDirection = GetDirection * MovementForce;
			MeshComp->AddForce(GetDirection, NAME_None, bUseVelocityChange);
		}
		else
		{
			NextPathPoint = GetNextPathPoint();
		}
	}
	
}

FVector  ASTrackerBot::GetNextPathPoint()
{
	ACharacter * PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath * NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath)
	{
		if (NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
		else
		{
			return GetActorLocation();
		}
	}
	else

	{
		return GetActorLocation();
	}
	
}
	
void ASTrackerBot::HandleTakeDamage(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	//Explode
	UE_LOG(LogTemp, Warning, TEXT("Took Damage"));

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateDynamicMaterialInstance(0, MeshComp->GetMaterial(0), NAME_None);

	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);

	}

	if (Health <= 0)
	{
		SelfDestruct();
	}
	else
	{
		Health -= HealthDelta;
	}
}

void ASTrackerBot::SelfDestruct()
{
	if ( !bExplodedAlready)
	{
		bExplodedAlready = true;

		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), GetActorRotation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

		if (Role == ROLE_Authority)
		{
			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(this);
			UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
			SetLifeSpan(0.2f);
		}
		
		
		

	}
	else
	{
		return;
	}
	
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!StartedSelfDestruct)
	{
		ASCharacter* Player = Cast <ASCharacter>(OtherActor);
		if (Player)
		{
			UE_LOG(LogTemp, Warning, TEXT("KillSelf"));

			//We overlap with players
			if (Role == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, 0.5f, true, 0.0f);
			}
			StartedSelfDestruct = true;
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound,RootComponent,NAME_None);
		}
		
	}

}

void ASTrackerBot::DamageSelf()
{
	//Apply damage to self
								//Damage to, Value, Instigator,        damage causer,Damgaetype
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}