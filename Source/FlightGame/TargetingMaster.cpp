// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingMaster.h"

#include "Net/UnrealNetwork.h"


ATargetingMaster::ATargetingMaster()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ATargetingMaster::Multicast_AddToTargetsArray_Implementation(AActor* targetToAdd)
{
	if (!this) { return; }
	if (!targetToAdd) { return; }
	if (!totalTargetsInLevel.Find(targetToAdd)) { return; }
	totalTargetsInLevel.Add(targetToAdd);
	OnUpdateTargets.Broadcast(GetTargetsArray());

	UE_LOG(LogTemp, Warning, TEXT("Added to target array %s"), *targetToAdd->GetName());
}

TArray<AActor*> ATargetingMaster::GetTargetsArray()
{
	TArray<AActor*> newArray(totalTargetsInLevel);
	return newArray;
}


void ATargetingMaster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATargetingMaster, totalTargetsInLevel);
}