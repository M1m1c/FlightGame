// Fill out your copyright notice in the Description page of Project Settings.


#include "StarFighter.h"
#include "Net/UnrealNetwork.h"
#include <Runtime/Engine/Classes/GameFramework/SpringArmComponent.h>
#include <Runtime/Engine/Classes/Camera/CameraComponent.h>

// Sets default values
AStarFighter::AStarFighter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 60.0f;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

	CameraHolder = CreateDefaultSubobject<USceneComponent>(TEXT("CameraHolder"));
	CameraHolder->SetupAttachment(root);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(CameraHolder);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	MeshHolder = CreateDefaultSubobject<USceneComponent>(TEXT("MeshHolder"));
	MeshHolder->SetupAttachment(root);

}


// Called when the game starts or when spawned
void AStarFighter::BeginPlay()
{
	Super::BeginPlay();
	defaultCameraPos = CameraComp->GetRelativeLocation();

	CameraHolderDefaultTransform = CameraHolder->GetRelativeTransform();
	CameraArmDefaultTransform = CameraBoom->GetRelativeTransform();
}


// Called every frame
void AStarFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!HasAuthority() && IsLocallyControlled())
	{
		auto flightMove = CreateNewMove(DeltaTime);
		UnusedMoves.Add(flightMove);
		SimulateMove(flightMove);
		Server_SendMove(flightMove);
	}
	else if (HasAuthority() && IsLocallyControlled())
	{
		auto flightMove = CreateNewMove(DeltaTime);
		Server_SendMove(flightMove);
	}
	else if (GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
	{
		SimulateMove(ServerState.previousMove);
	}


	if (!IsLocallyControlled()) { return; }
	UpdateCameraOffset(DeltaTime);
	UpdateCameraRotation(DeltaTime);
}


FFlightMove AStarFighter::CreateNewMove(float DeltaTime)
{
	FFlightMove move;
	move.DeltaTime = DeltaTime;
	move.throttle = throttle;
	move.roll = roll;
	move.pitch = pitch;
	move.yaw = yaw;
	move.bUpdateRollVel = bUpdateRollVel;
	move.bUpdatePitchVel = bUpdatePitchVel;
	move.bUpdateYawVel = bUpdateYawVel;
	move.boostTimer = boostTimer;
	move.currentBoostAdditive = currentBoostAdditive;
	move.TimeStamp = GetWorld()->GetTimeSeconds();//ARaceGameState::Get(this)->GetServerWorldTimeSeconds();
	return move;
}

void AStarFighter::SimulateMove(const FFlightMove& move)
{
	UpdateShipMovement(move);

	UpdateShipRotation(move);
}


void AStarFighter::UpdateShipMovement(const FFlightMove& move)
{
	auto newVelocity = GetLinearVelocityChange(
		move.DeltaTime,
		move.throttle < 0.f ? -velAccelerationSpeed : velAccelerationSpeed,
		velDecelerationSpeed,
		FMath::IsNearlyZero(move.throttle) == false);

	boostTimer = move.boostTimer;
	//currentBoostAdditive = move.currentBoostAdditive;
	auto bIsBoosting = boostTimer < maxBoostTime;
	if (bIsBoosting) 
	{ 
		boostTimer = FMath::Clamp(boostTimer + move.DeltaTime, 0.f, maxBoostTime); 
		
		auto alpha = FMath::GetMappedRangeValueClamped(FVector2D(0.f, maxBoostTime), FVector2D(0.f, 1.f), boostTimer);
		currentBoostAdditive = FMath::Lerp(maxBoostTime, 0.f, alpha);
	}

	auto clampMaxValue = bIsBoosting ? 1.f + currentBoostAdditive : 1.f;
	auto velocityChange = moveVelocity + newVelocity + (bIsBoosting ? currentBoostAdditive : 0.f);
	moveVelocity = FMath::Clamp(velocityChange, 0.2f, clampMaxValue);


	FVector location = GetActorLocation();
	location += GetActorForwardVector() * (MaxSpeed * move.DeltaTime * moveVelocity);
	SetActorLocation(location);
}

void AStarFighter::UpdateShipRotation(const FFlightMove& move)
{

	auto updatedPitch = GetUpdatedRotAxis(move.DeltaTime, pitchSpeed, pitchVelocity, move.pitch, move.bUpdatePitchVel);

	auto updatedRoll = GetUpdatedRotAxis(move.DeltaTime, rollSpeed, rollVelocity, move.roll, move.bUpdateRollVel);

	auto updatedYaw = GetUpdatedRotAxis(move.DeltaTime, yawSpeed, yawVelocity, move.yaw, move.bUpdateYawVel);

	FRotator newRotation = FRotator(updatedPitch, updatedYaw, updatedRoll) * move.DeltaTime;

	FQuat quatRotation = FQuat(newRotation);

	AddActorLocalRotation(quatRotation, false, 0, ETeleportType::None);
}

float AStarFighter::GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float input, bool bAccCondition)
{
	velocity = GetRotVelocity(DeltaTime, velocity, bAccCondition);
	if (FMath::IsNearlyZero(velocity)) { input = 0.f; }
	return input * speed * velocity;
}

float AStarFighter::GetRotVelocity(float DeltaTime, float currentVel, bool bAccCondition)
{
	auto velocityChange = GetPropotionalVelocityChange(
		DeltaTime,
		currentVel,
		turnAccelerationSpeed,
		turnDecelerationSpeed,
		bAccCondition);
	return FMath::Clamp(currentVel + velocityChange, 0.f, 1.f);
}

void AStarFighter::UpdateCameraOffset(float DeltaTime)
{

	auto yVel = ((rollVelocity * pitchVelocity) + yawVelocity);
	auto yInput = FMath::Clamp(FMath::Abs(roll * pitch) + FMath::Abs(yaw), 0.f, 1.f);
	auto yOffSet = (FMath::Clamp(yVel * yInput, 0.f, 1.f) * maxOffset * moveVelocity);
	auto xOffSet = (pitchVelocity * FMath::Abs(pitch) * maxOffset * moveVelocity);

	auto bRollNegative = (roll < 0.f && bUpdateRollVel == true);
	auto bYawNegative = (yaw < 0.f && bUpdateYawVel == true);

	auto offsetVec = FVector(
		defaultCameraPos.X,
		((bRollNegative || bYawNegative) ? -yOffSet : yOffSet) + defaultCameraPos.Y,
		(pitch < 0.f ? -xOffSet : xOffSet) + defaultCameraPos.Z);

	auto currentCamPos = CameraComp->GetRelativeLocation();
	CameraComp->SetRelativeLocation(FMath::Lerp(currentCamPos, offsetVec, DeltaTime));
}

void AStarFighter::UpdateCameraRotation(float DeltaTime)
{
	if (CameraResetTime == 1.f && !bFreeCameraLook) { return; }
	FRotator newArmRotation = FRotator(CameraVertical * CameraRotationSpeed,0.f, 0.f) * DeltaTime;
	FQuat quatArmRotation = FQuat(newArmRotation);
	CameraBoom->AddRelativeRotation(quatArmRotation);

	FRotator newHolderRotation = FRotator(0.f, CameraHorizontal * CameraRotationSpeed, 0.f) * DeltaTime;
	FQuat quatHolderRotation = FQuat(newHolderRotation);
	CameraHolder->AddRelativeRotation(quatHolderRotation);

	if (bFreeCameraLook) { return; }

	bShouldResetCamera = bNoHorizontalCamInput && bNoVerticalCamInput;

	if (bShouldResetCamera)
	{
		CameraResetStartTimer = FMath::Clamp(CameraResetStartTimer - DeltaTime, 0.f, CameraResetStartTimerMax);
	}

	if (FMath::IsNearlyZero(CameraResetStartTimer))
	{
		auto currentArmRot = CameraBoom->GetRelativeRotation();
		CameraBoom->SetRelativeRotation(
			FQuat::Slerp(FQuat(currentArmRot),
				CameraArmDefaultTransform.GetRotation(),
				CameraResetTime));

		auto currentHolderRot = CameraHolder->GetRelativeRotation();
		CameraHolder->SetRelativeRotation(
			FQuat::Slerp(FQuat(currentHolderRot),
				CameraHolderDefaultTransform.GetRotation(),
				CameraResetTime));

		CameraResetTime = FMath::Clamp(CameraResetTime + (DeltaTime * CameraResetSpeed), 0.f, 1.f);
	}
}

float AStarFighter::GetLinearVelocityChange(float deltaTime, float accelSpeed, float decelSpeed, bool changeCondition)
{
	auto velocityChange = (changeCondition ? deltaTime * accelSpeed : -(deltaTime * decelSpeed));
	return velocityChange;
}

float AStarFighter::GetPropotionalVelocityChange(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition)
{
	auto proportionalDec = -(deltaTime + (deltaTime * (decelSpeed * currentVelocity)));
	auto deceleration = currentVelocity > .0f ? proportionalDec : -deltaTime;
	auto velocityChange = (changeCondition ? deltaTime * accelSpeed : deceleration);
	return velocityChange;
}

// Called to bind functionality to input
void AStarFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("Throttle", this, &AStarFighter::ReadThrottle);
	InputComponent->BindAxis("Roll", this, &AStarFighter::ReadRoll);
	InputComponent->BindAxis("Pitch", this, &AStarFighter::ReadPitch);
	InputComponent->BindAxis("Yaw", this, &AStarFighter::ReadYaw);
	InputComponent->BindAxis("CameraHorizontal", this, &AStarFighter::ReadCameraHorizontal);
	InputComponent->BindAxis("CameraVertical", this, &AStarFighter::ReadCameraVertical);
	InputComponent->BindAction("FreeCameraLook", IE_Pressed, this, &AStarFighter::ToggleCameraFreeLook);
	InputComponent->BindAction("BoostInput", IE_Pressed, this, &AStarFighter::ReadBoostInput);
}

float AStarFighter::GetBoostTimer()
{
	return boostTimer;
}

float AStarFighter::GetMaxBoostTime()
{
	return maxBoostTime;
}

void AStarFighter::ReadThrottle(float value)
{
	throttle = FMath::Clamp(value, -1.f, 1.f);;
}

void AStarFighter::ReadRoll(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdateRollVel = false;
		return;
	}
	roll = FMath::Clamp(value, -1.f, 1.f);;
	bUpdateRollVel = true;
}

void AStarFighter::ReadPitch(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdatePitchVel = false;
		return;
	}
	pitch = -FMath::Clamp(value, -1.f, 1.f);;
	bUpdatePitchVel = true;
}

void AStarFighter::ReadYaw(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdateYawVel = false;
		return;
	}
	yaw = FMath::Clamp(value, -1.f, 1.f);;
	bUpdateYawVel = true;
}


void AStarFighter::ReadCameraHorizontal(float value)
{
	bNoHorizontalCamInput = FMath::IsNearlyZero(value);
	CameraResetStartTimer = bNoHorizontalCamInput ? CameraResetStartTimer : CameraResetStartTimerMax;
	CameraResetTime = bNoHorizontalCamInput ? CameraResetTime : 0.f;
	CameraHorizontal = value;
}

void AStarFighter::ReadCameraVertical(float value)
{
	bNoVerticalCamInput = FMath::IsNearlyZero(value);
	CameraResetStartTimer = bNoVerticalCamInput ? CameraResetStartTimer : CameraResetStartTimerMax;
	CameraResetTime = bNoVerticalCamInput ? CameraResetTime : 0.f;
	CameraVertical = value;
}

void AStarFighter::ToggleCameraFreeLook()
{
	bFreeCameraLook = !bFreeCameraLook;
}

void AStarFighter::ReadBoostInput()
{
	if (boostTimer != maxBoostTime) { return; }
	if (moveVelocity < 0.65f) { return; }
	OnBoosted();
	currentBoostAdditive = maxBoostVelocityAdditive;
	boostTimer = 0.f;
}

void AStarFighter::ClearUsedMoves(FFlightMove previousMove)
{
	TArray<FFlightMove> newMoves;

	for (const FFlightMove& move : UnusedMoves)
	{
		if (move.TimeStamp > previousMove.TimeStamp)
		{
			newMoves.Add(move);
		}
	}
	UnusedMoves = newMoves;
}


void AStarFighter::Server_SendMove_Implementation(FFlightMove NewMove)
{
	SimulateMove(NewMove);
	ServerState.previousMove = NewMove;
	ServerState.transform = GetActorTransform();
	ServerState.moveVelocity = moveVelocity;
	ServerState.rollVelocity = rollVelocity;
	ServerState.pitchVelocity = pitchVelocity;
	ServerState.yawVelocity = yawVelocity;
}

//Updates position and simulate moves that have been missed
void AStarFighter::OnRep_ServerState()
{
	SetActorTransform(ServerState.transform);
	moveVelocity = ServerState.moveVelocity;
	rollVelocity = ServerState.rollVelocity;
	pitchVelocity = ServerState.pitchVelocity;
	yawVelocity = ServerState.yawVelocity;
	ClearUsedMoves(ServerState.previousMove);

	for (const FFlightMove& move : UnusedMoves)
	{
		SimulateMove(move);
	}
}


void AStarFighter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStarFighter, ServerState);
}