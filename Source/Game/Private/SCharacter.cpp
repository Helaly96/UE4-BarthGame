// Fill out your copyright notice in the Description page of Project Settings.




#include "../public/SCharacter.h"
#include "SWeapon.h"
//to use input component
#include "Components/InputComponent.h"
//to use camera component
#include "Camera/CameraComponent.h"

//to use the spring arm component
#include "GameFramework/SpringArmComponent.h"

//To access the movement component
#include"GameFramework/PawnMovementComponent.h"
#include "Game.h"
#include "HealthComponent.h"
//To control the character movement
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include"Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "Engine/GameInstance.h"
// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCameraComponent"));
	//If this camera component is placed on a pawn, should it use the view/control rotation of the pawn where possible?


	SpringComp = CreateDefaultSubobject<USpringArmComponent >(TEXT("SpringArmComponent Created"));
	HComponent = CreateDefaultSubobject<UHealthComponent >(TEXT("HealthComp"));
	Name = CreateDefaultSubobject<UTextRenderComponent >(TEXT("TextRendererForName"));
	FPCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCameraComponent"));
	SpringComp->SetupAttachment(RootComponent);
	Name->SetupAttachment(RootComponent);
	FPCameraComp->SetupAttachment(GetMesh(), "spine_01");

	CameraComp->SetupAttachment(SpringComp);
	SpringComp->bUsePawnControlRotation = true;

	CameraComp->SetIsReplicated(true);

	//to make unreal make the player crouch
	//The pawn has a movement component which has paramters such as it can crouch, jump , fly
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	is_Sprinting = false;

	this->DefaultFov = CameraComp->FieldOfView;
	ZoomFov = 40;

	LookUpModifier = 2;

	Current_Camera_State = 0;

	


	//Rules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);

}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("%f Speed is"), this->GetVelocity().Size());
	target_fov = Want_To_Zoom ? ZoomFov : DefaultFov;

	//Lerps between 2 values, given delta time and speed 
	if (CameraComp)
	{
		current_fov = FMath::FInterpTo(CameraComp->FieldOfView, target_fov, DeltaTime, 20);
		CameraComp->SetFieldOfView(current_fov);
	}



}

void ASCharacter::BeginSlide()
{
	UE_LOG(LogTemp, Warning, TEXT("StartedCrouching"));
	this->is_Sliding = true;
}

void ASCharacter::BeginCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("StartedCrouching"));
	bIsCrouching = true;
	Crouch();
}


void ASCharacter::EndCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("EndCrouching"));
	UnCrouch();
	bIsCrouching = false;
}

void ASCharacter::StartFire()
{
	if (Current_Weapon)
	{
		Current_Weapon->StartFire();

	}
}

void ASCharacter::StopFire()
{
	if (Current_Weapon)
	{
		Current_Weapon->EndFire();

	}
}



void ASCharacter::MoveForward(float value)
{
	//Add movement input along the given world direction vector 
	AddMovementInput(GetActorForwardVector() * value);
}

void ASCharacter::BeginJump()
{
	//Start the jumping action
	Jump();

}

void ASCharacter::BeginSprint()
{
	if (!is_Sprinting)
	{
		is_Sprinting = true;
		UE_LOG(LogTemp, Warning, TEXT("StartRunning"));
		GetCharacterMovement()->MaxWalkSpeed *= Speed_Modifier;
	}
	
	
}

void ASCharacter::EndSprint()
{
	UE_LOG(LogTemp, Warning, TEXT("StartedCrouching"));
	GetCharacterMovement()->MaxWalkSpeed /= Speed_Modifier;
	is_Sprinting = false;

}

void ASCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind the axis to a funcution
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	//These are 2 built in funcutions in unreal engine
	//if you remember them from blueprints days, they both take val as an argument

	//remember to invert the value in the editor for the lookup, mouse Y Variable
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::LookUpMultiplier);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	//Binding the Crouch button
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	//Binding Jump Action
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::BeginJump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::EndSprint);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("SwitchGun", IE_Released, this, &ASCharacter::SwitchGun);
	PlayerInputComponent->BindAction("ChangeCameraMode", IE_Released, this, &ASCharacter::ChangeCamera);
	
}

void ASCharacter::LookUpMultiplier(float val)
{
	ASCharacter::AddControllerPitchInput(val * LookUpModifier);
}




float ASCharacter::GetRemoteViewPitch() const
{
	float ClampedPitch = (RemoteViewPitch * 360.f / 255.f);
	ClampedPitch = ClampedPitch > 90.f ? ClampedPitch - 360.f : ClampedPitch;
	return FMath::Clamp<float>(ClampedPitch, -90.f, 90.f);
}


void ASCharacter::SwitchGun()
{
	if (Role == ROLE_Authority)
	{
		if (Current_Weapon == MainWeapon)
		{
			Current_Weapon = SecoundaryWeapon;
			MainWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "BackGun");
			SecoundaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		}
		else
		{
			Current_Weapon = MainWeapon;
			SecoundaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "BackGun");
			MainWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
		}
	}
	else
	{
		ServerSwitchGun();
	}

}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Spawn a Default Player
	FActorSpawnParameters params;

	//always spawn it, even if it's colliding with something
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	MainWeapon = GetWorld()->SpawnActor<ASWeapon>(MainWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, params);
	SecoundaryWeapon = GetWorld()->SpawnActor<ASWeapon>(SecoundaryWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, params);
	HComponent->ChangeHelath.AddDynamic(this, &ASCharacter::OnHealthChange);

	//if you are the server (Means that the Game running on that PC is the server)
	if (Role == ROLE_Authority)
	{
		if (MainWeapon)
		{
			MainWeapon->SetOwner(this);
			MainWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
			Current_Weapon = MainWeapon;
		}

		if (SecoundaryWeapon)
		{
			SecoundaryWeapon->SetOwner(this);
			SecoundaryWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "BackGun");
		}

	}

	
}

void ASCharacter::BeginZoom()
{
	Want_To_Zoom = true;
}

void ASCharacter::EndZoom()
{
	Want_To_Zoom = false;
}

void ASCharacter::OnHealthChange(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	UE_LOG(LogTemp, Warning, TEXT("Health is %f"), Health);

	if (Health <= 0 && !DiedBefore)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnteredDeath"));
		GetMovementComponent()->StopMovementImmediately();
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		DiedBefore = true;
		SetLifeSpan(10.0f);
	}

}



//This is called when GetEyeLocation is called :3

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}
	else
	{
		return Super::GetPawnViewLocation();
	}

}

void ASCharacter::ServerSwitchGun_Implementation()
{
	SwitchGun();

}

void ASCharacter::ChangeCamera()
{
	if (Role < ROLE_Authority)
	{
		ServerChangeCamera();
		return;
	}
		//We are in Currently Third Person
		if (Current_Camera_State == 0)
		{
			CameraComp->SetActive(false);
			TempCamera = CameraComp;
			CameraComp = FPCameraComp;
			SpringComp->bUsePawnControlRotation = false;
			CameraComp->bUsePawnControlRotation = true;
			CameraComp->SetActive(true);
			Current_Camera_State = 1;
		}

		//We are Currently in First person
		else
		{
			CameraComp->SetActive(false);
			CameraComp = TempCamera;
			CameraComp->SetActive(true);
			CameraComp->bUsePawnControlRotation = false;
			SpringComp->bUsePawnControlRotation = true;
			Current_Camera_State = 0;
		}
	}



void ASCharacter::ServerChangeCamera_Implementation()
{
	ChangeCamera();
}


bool ASCharacter::ServerSwitchGun_Validate()
{
	return true;

}



//what to replicate when a server connects to us

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASCharacter, Current_Weapon);
	DOREPLIFETIME(ASCharacter, DiedBefore);
	DOREPLIFETIME(ASCharacter, SecoundaryWeapon);
	DOREPLIFETIME(ASCharacter, MainWeapon);
	DOREPLIFETIME(ASCharacter, Name);
	DOREPLIFETIME(ASCharacter, PlayerName);
	DOREPLIFETIME(ASCharacter, CameraComp);
	DOREPLIFETIME(ASCharacter, Current_Camera_State);
	DOREPLIFETIME(ASCharacter, SpringComp);
	//DOREPLIFETIME(ASCharacter, FPCameraComp);

}


void ASCharacter::TakeNameFromTerminal(FString Name)
{
	if (Role == ROLE_Authority)
	{
		PlayerName = Name;
	}
	else
	{
		ServerChangeName(Name);
	}
}

void ASCharacter::ServerChangeName_Implementation(const FString& Name)
{
	
	PlayerName = Name;
}



/* How the health system is handled*/

/*
  an event  gets fired whenever the owner of the health componen takes a damage,

	MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeAnyDamage);
	so when the owner takes damage ,  the event takeAnyDamge  gets called , and the health component responds by caalling TakeANyDamge
	fn

	which then brodcasts its own made event:
		ChangeHelath.Broadcast(this,Health,Damage, DamageType , InstigatedBy, DamageCauser);
		which is defined here
		DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);
		then in the character cpp we add a funcution to respond to the change health event
		HComponent->ChangeHelath.AddDynamic(this, &ASCharacter::OnHealthChange);

		which calls the fn OnHealthChange

*/