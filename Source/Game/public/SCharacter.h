// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/TextRenderComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/EngineTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent; class USpringArmComponent; class ASWeapon; class UHealthComponent;
UCLASS()
class GAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSwitchGun();

	UFUNCTION(Server, Reliable)
	void ServerChangeName(const FString& Name);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
		UTextRenderComponent* Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	FString PlayerName;

	void TakeNameFromTerminal(FString Name);

	void LookUpMultiplier(float val);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float LookUpModifier;
protected:
	// Called when the game starts or when spawned

	//Vars

	//make the next item visible anywhere (so we can access it from blueprints), but read only in the blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	UCameraComponent* FPCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpringArmComponent* SpringComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool is_Sprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Speed_Modifier=1.5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool is_Sliding = false;

	float DefaultFov;
	float ZoomFov;
	float current_fov;
	float target_fov;
	bool Want_To_Zoom;

	UPROPERTY(Replicated)
	ASWeapon * Current_Weapon;

	UPROPERTY(Replicated)
	ASWeapon* MainWeapon;

	UPROPERTY(Replicated)
	ASWeapon* SecoundaryWeapon;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASWeapon> MainWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASWeapon> SecoundaryWeaponClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UHealthComponent* HComponent;

	


	bool bIsCrouching;


	//Funcutions
	void MoveForward(float value);
	void MoveRight(float value);

	void BeginCrouch();
	void EndCrouch();

	void BeginJump();

	void BeginSprint();
	void EndSprint();

	void BeginSlide();

	void BeginZoom();
	void EndZoom();

	void StartFire();
	void StopFire();
	void SwitchGun();

	// 0 if Third , 1 If first
	UPROPERTY(Replicated)
	int Current_Camera_State;

	UFUNCTION(BlueprintCallable)
	void ChangeCamera();

	UFUNCTION(Server, Reliable)
	void ServerChangeCamera();

	void ChangeName(FString Name);

	UCameraComponent* TempCamera;

	UFUNCTION()
	void OnHealthChange(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FAttachmentTransformRules * Rules;
	
	UPROPERTY(Replicated,BlueprintReadOnly)
	bool DiedBefore = false;

public:	
	// Called to bind functionality to input
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FVector GetPawnViewLocation() const override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	float GetRemoteViewPitch() const;
};
