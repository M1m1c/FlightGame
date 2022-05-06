// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComp.h"
#include "FlightGameState.h"
#include "TargetingMaster.h"

#include "Net/UnrealNetwork.h"

UTargetingComp::UTargetingComp()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

AActor* UTargetingComp::GetTargetFromIndex(int32 index)
{
	AActor* retval = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("@GetTargetFromIndex target count: %d"), availableTargets.Num());
	if (availableTargets.IsValidIndex(index)) { retval = availableTargets[index]; }
	return retval;
}

int UTargetingComp::GetTargetCount()
{
	return availableTargets.Num();
}


//register initalise so that it gets called when the state is finished setting up
//Only gets called on server
void UTargetingComp::BeginPlay()
{
	Super::BeginPlay();

	auto gameState = AFlightGameState::Get(this);
	if (!ensure(gameState)) { return; }
	gameState->OnGameStateInitalisation.AddDynamic(this, &UTargetingComp::Initalise);

	if (!gameState->GetIsInitalised()) { return; }
	Initalise();
}

//Only gets called on server
void UTargetingComp::Initalise()
{
	auto gameState = AFlightGameState::Get(this);
	if (!ensure(gameState)) { return; }
	if (!ensure(gameState->TargetingMaster)) { return; }
	gameState->TargetingMaster->OnUpdateTargets.AddDynamic(this, &UTargetingComp::UpdateTargetArray);
	availableTargets = gameState->TargetingMaster->GetTargetsArray();
	gameState->OnGameStateInitalisation.RemoveDynamic(this, &UTargetingComp::Initalise);
	UE_LOG(LogTemp, Warning, TEXT("@Initalise target count: %d"), availableTargets.Num());
}

//Only gets called on server
void UTargetingComp::UpdateTargetArray(TArray<AActor*> newTargetList)
{
	if (newTargetList.Num() == 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("new target list was empty"));
		return; 
	}
	availableTargets = newTargetList;
	UE_LOG(LogTemp, Warning, TEXT("@UpdateTargetArray target count: %d"), availableTargets.Num());
}

void UTargetingComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTargetingComp, availableTargets);
}
