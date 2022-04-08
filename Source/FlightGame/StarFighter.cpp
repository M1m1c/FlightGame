// Fill out your copyright notice in the Description page of Project Settings.


#include "StarFighter.h"
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

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(root);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

}

// Called when the game starts or when spawned
void AStarFighter::BeginPlay()
{
	Super::BeginPlay();
	defaultCameraPos = CameraComp->GetRelativeLocation();
}

// Called every frame
void AStarFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateShipMovement(DeltaTime);

	UpdateShipRotation(DeltaTime);

	if (!IsLocallyControlled()) { return; }
	UpdateCameraOffset(DeltaTime);

}

void AStarFighter::UpdateShipMovement(float DeltaTime)
{
	auto newVelocity = GetLinearVelocityChange(
		DeltaTime,
		throttle < 0.f ? -velAccelerationSpeed : velAccelerationSpeed,
		velDecelerationSpeed,
		FMath::IsNearlyZero(throttle) == false);

	moveVelocity = FMath::Clamp(moveVelocity + newVelocity, 0.2f, 1.f);

	FVector location = GetActorLocation();
	location += GetActorForwardVector() * (MaxSpeed * DeltaTime * moveVelocity);
	SetActorLocation(location);
}

void AStarFighter::UpdateShipRotation(float DeltaTime)
{

	auto updatedPitch = GetUpdatedRotAxis(DeltaTime, pitchSpeed, pitchVelocity, pitch, bUpdatePitchVel);

	auto updatedRoll = GetUpdatedRotAxis(DeltaTime, rollSpeed, rollVelocity, roll, bUpdateRollVel);

	auto updatedYaw = GetUpdatedRotAxis(DeltaTime, yawSpeed, yawVelocity, yaw, bUpdateYawVel);

	FRotator newRotation = FRotator(updatedPitch, updatedYaw, updatedRoll) * DeltaTime;

	FQuat quatRotation = FQuat(newRotation);

	AddActorLocalRotation(quatRotation, false, 0, ETeleportType::None);
}

float AStarFighter::GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float& input, bool bAccCondition)
{
	velocity = GetRotVelocity(DeltaTime, velocity, bAccCondition);
	if (FMath::IsNearlyZero(velocity)) { input = 0.f; }
	return input * speed * velocity;
}

float AStarFighter::GetRotVelocity(float DeltaTime, float currentVel,bool bAccCondition)
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

float AStarFighter::GetLinearVelocityChange(float deltaTime, float accelSpeed, float decelSpeed, bool changeCondition)
{
	auto velocityChange = (changeCondition ? deltaTime * accelSpeed : -(deltaTime * decelSpeed));
	return velocityChange;
}

float AStarFighter::GetPropotionalVelocityChange(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition)
{
	auto proportionalDec = -(deltaTime + (deltaTime * (decelSpeed * currentVelocity)));
	auto deceleration = moveVelocity > .0f ? proportionalDec : -deltaTime;
	auto velocityChange = (changeCondition ? deltaTime * accelSpeed : deceleration);
	return velocityChange;
}

// Called to bind functionality to input
void AStarFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("Throttle", this, &AStarFighter::Server_ReadThrottle);
	InputComponent->BindAxis("Throttle", this, &AStarFighter::ReadThrottle);

	InputComponent->BindAxis("Roll", this, &AStarFighter::Server_ReadRoll);
	InputComponent->BindAxis("Roll", this, &AStarFighter::ReadRoll);

	InputComponent->BindAxis("Pitch", this, &AStarFighter::Server_ReadPitch);
	InputComponent->BindAxis("Pitch", this, &AStarFighter::ReadPitch);

	InputComponent->BindAxis("Yaw", this, &AStarFighter::Server_ReadYaw);
	InputComponent->BindAxis("Yaw", this, &AStarFighter::ReadYaw);
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

void AStarFighter::Server_ReadThrottle_Implementation(float value)
{
	ReadThrottle(value);
}

void AStarFighter::Server_ReadRoll_Implementation(float value)
{
	ReadRoll(value);
}

void AStarFighter::Server_ReadPitch_Implementation(float value)
{
	ReadPitch(value);
}

void AStarFighter::Server_ReadYaw_Implementation(float value)
{
	ReadYaw(value);
}
