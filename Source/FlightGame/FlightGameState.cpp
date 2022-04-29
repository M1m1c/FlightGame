// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightGameState.h"

void AFlightGameState::AddToTargetsArray(AActor* targetToAdd)
{
	totalTargetsInLevel.Add(targetToAdd);
}

TArray<AActor*> AFlightGameState::GetTargetsArray()
{
	TArray<AActor*> newArray(totalTargetsInLevel);
	return newArray;
}
