// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetingMaster.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetsChangedInWorld, TArray<AActor*>, targetsInWorld);

UCLASS()
class FLIGHTGAME_API ATargetingMaster : public AActor
{
	GENERATED_BODY()

		ATargetingMaster();
		UPROPERTY(Replicated)
		TArray<AActor*> totalTargetsInLevel;
public:

	UPROPERTY(BlueprintAssignable)
	FTargetsChangedInWorld OnUpdateTargets;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_AddToTargetsArray(AActor* targetToAdd);

	TArray<AActor*> GetTargetsArray();
	
};
