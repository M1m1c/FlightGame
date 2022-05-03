// Copyright Epic Games, Inc. All Rights Reserved.


#include "FlightGameModeBase.h"
#include "FlightGameState.h"


void AFlightGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	auto gamestate = AFlightGameState::Get(this);
	gamestate->Multicast_InitialiseState();
}
