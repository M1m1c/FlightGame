// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetableComp.h"
#include <FlightGame/FlightGameState.h>
#include "TargetingMaster.h"

UTargetableComp::UTargetableComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UTargetableComp::BeginPlay()
{
	Super::BeginPlay();

	auto flightGameState = AFlightGameState::Get(this);
	if (!ensure(flightGameState)) { return; }
	flightGameState->OnGameStateInitalisation.AddDynamic(this, &UTargetableComp::AddMyselfAsTarget);

	if (!flightGameState->TargetingMaster) { return; }
	flightGameState->TargetingMaster->AddToTargetsArray(GetOwner());
}

void UTargetableComp::AddMyselfAsTarget()
{

	auto flightGameState = AFlightGameState::Get(this);
	if (!ensure(flightGameState)) { return; }
	flightGameState->TargetingMaster->AddToTargetsArray(GetOwner());
}


