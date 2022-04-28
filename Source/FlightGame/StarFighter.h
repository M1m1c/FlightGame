// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "StarFighter.generated.h"

USTRUCT()
struct FFlightMove
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	float throttle = .0f;
	UPROPERTY()
	float roll = .0f;
	UPROPERTY()
	float pitch = .0f;
	UPROPERTY()
	float yaw = .0f;

	UPROPERTY()
	float boostTimer = 0.f;
	UPROPERTY()
	float currentBoostAdditive = 0.f;

	UPROPERTY()
	bool bUpdateRollVel = false;
	UPROPERTY()
	bool bUpdatePitchVel = false;
	UPROPERTY()
	bool bUpdateYawVel = false;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float TimeStamp;
};

USTRUCT()
struct FStarFighterState
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY()
	FTransform transform;

	UPROPERTY()
	float moveVelocity = .0f;
	UPROPERTY()
	float rollVelocity = .0f;
	UPROPERTY()
	float pitchVelocity = .0f;
	UPROPERTY()
	float yawVelocity = .0f;

	UPROPERTY()
	FFlightMove previousMove;
};


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

	void SimulateMove(const FFlightMove& move);

	void UpdateShipMovement(const FFlightMove& move);

	void UpdateShipRotation(const FFlightMove& move);

	float GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float input, bool bAccCondition);

	float GetRotVelocity(float DeltaTime, float currentVel, bool bAccCondition);

	void UpdateCameraOffset(float DeltaTime);
	void UpdateCameraRotation(float DeltaTime);

	void ReadThrottle(float value);

	void ReadRoll(float value);

	void ReadPitch(float value);

	void ReadYaw(float value);

	void ReadCameraHorizontal(float value);

	void ReadCameraVertical(float value);

	void ToggleCameraFreeLook();

	void ReadBoostInput();

	void ClearUsedMoves(FFlightMove previousMove);

	FFlightMove CreateNewMove(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_SendMove(FFlightMove NewMove);

	UFUNCTION()
	void OnRep_ServerState();

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FStarFighterState ServerState;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CameraHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* MeshHolder;

	TArray<FFlightMove> UnusedMoves;

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

	float boostTimer = 1.f;
	float maxBoostTime = 1.f;
	float currentBoostAdditive = 0.f;
	float maxBoostVelocityAdditive = 0.01f;

	FTransform CameraHolderDefaultTransform;
	FTransform CameraArmDefaultTransform;
	float CameraRotationSpeed = 100.f;
	float CameraHorizontal = 0.f;
	float CameraVertical = 0.f;
	float CameraResetStartTimerMax = 0.4f;
	float CameraResetStartTimer = 0.f;
	float CameraResetTime = 0.f;
	float CameraResetSpeed = 0.2f;
	bool bFreeCameraLook = false;
	bool bShouldResetCamera = false;
	bool bNoHorizontalCamInput = false;
	bool bNoVerticalCamInput = false;

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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	float GetBoostTimer();

	UFUNCTION(BlueprintCallable)
	float GetMaxBoostTime();

};
