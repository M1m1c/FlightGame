// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComp.h"
#include "FlightGameState.h"
#include "TargetingMaster.h"

UTargetingComp::UTargetingComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//register initalise so that it gets called when the state is finished setting up
void UTargetingComp::BeginPlay()
{
	Super::BeginPlay();

	auto gameState = AFlightGameState::Get(this);
	if (!ensure(gameState)) { return; }
	gameState->OnGameStateInitalisation.AddDynamic(this, &UTargetingComp::Initalise);
}

void UTargetingComp::Initalise()
{
	auto gameState = AFlightGameState::Get(this);
	if (!ensure(gameState)) { return; }
	if (!ensure(gameState->TargetingMaster)) { return; }
	gameState->TargetingMaster->OnUpdateTargets.AddDynamic(this, &UTargetingComp::UpdateTargetArray);
	availableTargets = gameState->TargetingMaster->GetTargetsArray();
	gameState->OnGameStateInitalisation.RemoveDynamic(this, &UTargetingComp::Initalise);
}

void UTargetingComp::UpdateTargetArray(TArray<AActor*> newTargetList)
{
	availableTargets = newTargetList;
	UE_LOG(LogTemp, Warning, TEXT("@UpdateTargetArray target count: %d"), availableTargets.Num());
}

