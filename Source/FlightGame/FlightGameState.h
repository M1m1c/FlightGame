// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FlightGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInitialiseGameState);

UCLASS()
class FLIGHTGAME_API AFlightGameState : public AGameStateBase
{
	GENERATED_BODY()		
protected:
	bool bInitalised = false;

public:

	void InitialiseState();

	FInitialiseGameState OnGameStateInitalisation;

	UFUNCTION()
	static AFlightGameState* Get(UObject* WorldContext);

	UPROPERTY()
	class ATargetingMaster* TargetingMaster;

	bool GetIsInitalised();
};
