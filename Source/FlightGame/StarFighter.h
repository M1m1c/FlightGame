// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StarFighter.generated.h"

UCLASS()
class FLIGHTGAME_API AStarFighter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AStarFighter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	float GetLinearVelocityChange(float deltaTime, float accelSpeed, float decelSpeed, bool changeCondition);

	float GetPropotionalVelocityChange(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition);

	void UpdateShipMovement(float DeltaTime);

	void UpdateShipRotation(float DeltaTime);

	float GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float& input, bool bAccCondition);

	float GetRotVelocity(float DeltaTime, float currentVel, bool bAccCondition);

	void UpdateCameraOffset(float DeltaTime);

	void ReadThrottle(float value);

	void ReadRoll(float value);

	void ReadPitch(float value);

	void ReadYaw(float value);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(EditDefaultsOnly)
	float MaxSpeed = 10000.0f;

	UPROPERTY(BlueprintReadOnly)
	float moveVelocity = .0f;
	float velDecelerationSpeed = 0.01f;
	float velAccelerationSpeed = 0.3f;
	float turnDecelerationSpeed = 3.f;
	float turnAccelerationSpeed = 4.f;

	float throttle = .0f;
	float roll = .0f;
	float pitch = .0f;
	float yaw = .0f;
	float rollSpeed = 100.f;
	float pitchSpeed = 75.f;
	float yawSpeed = 25.f;

	UPROPERTY(BlueprintReadOnly)
	float rollVelocity = .0f;
	UPROPERTY(BlueprintReadOnly)
	float pitchVelocity = .0f;
	UPROPERTY(BlueprintReadOnly)
	float yawVelocity = .0f;

	bool bUpdateRollVel = false;
	bool bUpdatePitchVel = false;
	bool bUpdateYawVel = false;

	FVector defaultCameraPos;
	float maxOffset = 100.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable)
	void Server_ReadThrottle(float value);

	UFUNCTION(Server, Reliable)
	void Server_ReadRoll(float value);

	UFUNCTION(Server, Reliable)
	void Server_ReadPitch(float value);

	UFUNCTION(Server, Reliable)
	void Server_ReadYaw(float value);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
