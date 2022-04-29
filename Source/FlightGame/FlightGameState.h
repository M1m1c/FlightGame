// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FlightGameState.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTGAME_API AFlightGameState : public AGameStateBase
{
	GENERATED_BODY()

		TArray<AActor*> totalTargetsInLevel;
public:
	void AddToTargetsArray(AActor* targetToAdd);
	
	TArray<AActor*> GetTargetsArray();
};
