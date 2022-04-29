// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetableComp.h"
#include <FlightGame/FlightGameState.h>

// Sets default values for this component's properties
UTargetableComp::UTargetableComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetableComp::BeginPlay()
{
	Super::BeginPlay();

	auto gameState = GetWorld()->GetGameState();
	auto flightGameState = Cast<AFlightGameState>(gameState);
	if (!flightGameState) { return; }
	flightGameState->AddToTargetsArray(GetOwner());
}


// Called every frame
void UTargetableComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

