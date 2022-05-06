// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComp.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTGAME_API UTargetingComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UTargetingComp();

	AActor* GetTargetFromIndex(int32 index);

	int GetTargetCount();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void Initalise();

	UFUNCTION()
	void UpdateTargetArray(TArray<AActor*> newTargetList);

	UPROPERTY(Replicated)
	TArray<AActor*> availableTargets;
};
