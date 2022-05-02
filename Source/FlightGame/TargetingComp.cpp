// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComp.h"
#include "FlightGameState.h"
#include "TargetingMaster.h"

UTargetingComp::UTargetingComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UTargetingComp::BeginPlay()
{
	Super::BeginPlay();

	auto gameState = AFlightGameState::Get(this);
	if (!ensure(gameState)) { return; }

	//gameState->OnGameStateInitalisation.AddDynamic(this, &UTargetingComp::Initalise);
	gameState->TargetingMaster->OnUpdateTargets.AddDynamic(this, &UTargetingComp::UpdateTargetArray);
	availableTargets = gameState->TargetingMaster->GetTargetsArray();
	UE_LOG(LogTemp, Warning, TEXT("@TargetingComp target count: %d"), availableTargets.Num());
}

void UTargetingComp::Initalise()
{
	auto gameState = AFlightGameState::Get(this);
	if (!ensure(gameState)) { return; }

	gameState->TargetingMaster->OnUpdateTargets.AddDynamic(this, &UTargetingComp::UpdateTargetArray);
	availableTargets = gameState->TargetingMaster->GetTargetsArray();
	UE_LOG(LogTemp, Warning, TEXT("@TargetingComp target count: %d"), availableTargets.Num());
}

void UTargetingComp::UpdateTargetArray(TArray<AActor*> newTargetList)
{
	availableTargets = newTargetList;
	UE_LOG(LogTemp, Warning, TEXT("@TargetingComp target count: %d"), availableTargets.Num());
}

