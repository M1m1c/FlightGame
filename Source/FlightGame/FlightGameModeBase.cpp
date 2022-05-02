// Copyright Epic Games, Inc. All Rights Reserved.


#include "FlightGameModeBase.h"
#include "FlightGameState.h"
#include "TargetingMaster.h"

void AFlightGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ATargetingMaster* targetingMaster = GetWorld()->SpawnActor<ATargetingMaster>();
	auto gamestate = AFlightGameState::Get(this);
	gamestate->TargetingMaster = targetingMaster;
	//targetingMaster->Initalise();
	gamestate->OnGameStateInitalisation.Broadcast();

}
