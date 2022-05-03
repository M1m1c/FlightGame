// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightGameState.h"
#include "TargetingMaster.h"

void AFlightGameState::Multicast_InitialiseState_Implementation()
{
	ATargetingMaster* targetingMaster = GetWorld()->SpawnActor<ATargetingMaster>();
	TargetingMaster = targetingMaster;
	OnGameStateInitalisation.Broadcast();
	bInitalised = true;
}

AFlightGameState* AFlightGameState::Get(UObject* WorldContext)
{
	UWorld* World = WorldContext->GetWorld();
	return World->GetGameState<AFlightGameState>();
}

bool AFlightGameState::GetIsInitalised()
{
	return bInitalised;
}
