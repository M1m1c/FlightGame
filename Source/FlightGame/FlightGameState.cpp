// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightGameState.h"

AFlightGameState* AFlightGameState::Get(UObject* WorldContext)
{
	UWorld* World = WorldContext->GetWorld();
	return World->GetGameState<AFlightGameState>();
}
