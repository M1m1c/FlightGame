// Fill out your copyright notice in the Description page of Project Settings.


#include "StarFighter.h"
#include <Runtime/Engine/Classes/GameFramework/SpringArmComponent.h>
#include <Runtime/Engine/Classes/Camera/CameraComponent.h>

// Sets default values
AStarFighter::AStarFighter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

	auto newVelocity = GetLinearVelocityChange(
		DeltaTime,
		throttle < 0.f ? -velAccelerationSpeed : velAccelerationSpeed,
		velDecelerationSpeed,
		FMath::IsNearlyZero(throttle) == false);

	moveVelocity = FMath::Clamp(moveVelocity + newVelocity, 0.2f, 1.f);
	//UE_LOG(LogTemp, Warning, TEXT("%f velocity"), moveVelocity);

	FVector location = GetActorLocation();
	location += GetActorForwardVector() * (MaxSpeed * DeltaTime * moveVelocity);
	SetActorLocation(location);

	auto pitchVel = GetPropotionalVelocityChange(
		DeltaTime,
		pitchVelocity,
		turnAccelerationSpeed,
		turnDecelerationSpeed,
		bUpdatePitchVel);
	pitchVelocity = FMath::Clamp(pitchVelocity + pitchVel, 0.f, 1.f);
	auto updatedPitch = pitch * pitchSpeed * pitchVelocity;

	auto rollVel = GetPropotionalVelocityChange(
		DeltaTime,
		rollVelocity,
		turnAccelerationSpeed,
		turnDecelerationSpeed,
		bUpdateRollVel);
	rollVelocity = FMath::Clamp(rollVelocity + rollVel, 0.f, 1.f);
	auto updatedRoll = roll * rollSpeed * rollVelocity;

	auto yawVel = GetPropotionalVelocityChange(
		DeltaTime,
		yawVelocity,
		turnAccelerationSpeed,
		turnDecelerationSpeed,
		bUpdateYawVel);
	yawVelocity = FMath::Clamp(yawVelocity + yawVel, 0.f, 1.f);
	auto updatedYaw = yaw * yawSpeed * yawVelocity;


	FRotator newRotation = FRotator(updatedPitch, updatedYaw, updatedRoll) * DeltaTime;

	FQuat quatRotation = FQuat(newRotation);

	AddActorLocalRotation(quatRotation, false, 0, ETeleportType::None);

	auto currentCamPos = CameraComp->GetRelativeLocation();
	//TODO Only do y offset whn rolling and pitching or when yawing
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
	InputComponent->BindAxis("Throttle", this, &AStarFighter::ReadThrottle);
	InputComponent->BindAxis("Roll", this, &AStarFighter::ReadRoll);
	InputComponent->BindAxis("Pitch", this, &AStarFighter::ReadPitch);
	InputComponent->BindAxis("Yaw", this, &AStarFighter::ReadYaw);
}

void AStarFighter::ReadThrottle(float value)
{
	throttle = value;
}

void AStarFighter::ReadRoll(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdateRollVel = false;
		return;
	}
	//TODO if differnce between value and roll is greater than 0.3 then reset velocity
	/*if ((roll > 0.f && value < 0.f) || (roll < 0.f && value > 0.f))
	{
		rollVelocity = 0.f;
	}*/
	roll = value;
	bUpdateRollVel = true;
}

void AStarFighter::ReadPitch(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdatePitchVel = false;
		return;
	}
	/*if ((pitch > 0.f && value < 0.f) || (pitch < 0.f && value > 0.f))
	{
		pitchVelocity = 0.f;
	}*/
	pitch = -value;
	bUpdatePitchVel = true;
}

void AStarFighter::ReadYaw(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdateYawVel = false;
		return;
	}
	yaw = value;
	bUpdateYawVel = true;
}



