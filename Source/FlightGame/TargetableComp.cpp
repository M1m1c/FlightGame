// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetableComp.h"
#include <FlightGame/FlightGameState.h>
#include "TargetingMaster.h"

UTargetableComp::UTargetableComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//register addmyself so that it gets called when the state is finished setting up
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
	flightGameState->OnGameStateInitalisation.RemoveDynamic(this, &UTargetableComp::AddMyselfAsTarget);
}


