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
	bAlwaysRelevant = true;
}

void ATargetingMaster::AddToTargetsArray(AActor* targetToAdd)
{
	if (!this) { return; }
	if (totalTargetsInLevel.Find(targetToAdd)) { return; }
	totalTargetsInLevel.Add(targetToAdd);
	OnUpdateTargets.Broadcast(totalTargetsInLevel);

	UE_LOG(LogTemp, Warning, TEXT("Added to target array %s"), *targetToAdd->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Target count: %d"), totalTargetsInLevel.Num());
}

TArray<AActor*> ATargetingMaster::GetTargetsArray()
{
	TArray<AActor*> newArray(totalTargetsInLevel);
	return newArray;
}