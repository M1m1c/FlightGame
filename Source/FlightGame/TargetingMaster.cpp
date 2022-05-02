// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingMaster.h"


//void ATargetingMaster::Initalise()
//{
//	if (!this) { return; }
//	UE_LOG(LogTemp, Warning, TEXT("initialised: %s"), *this->GetName());
//}

ATargetingMaster::ATargetingMaster()
{
	bReplicates = true;
}

void ATargetingMaster::AddToTargetsArray(AActor* targetToAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *targetToAdd->GetName());
	if (!this) { return; }
	totalTargetsInLevel.Add(targetToAdd);

	OnUpdateTargets.Broadcast(totalTargetsInLevel);
	UE_LOG(LogTemp, Warning, TEXT("target count: %d"), totalTargetsInLevel.Num());
}

TArray<AActor*> ATargetingMaster::GetTargetsArray()
{
	TArray<AActor*> newArray(totalTargetsInLevel);
	return newArray;
}