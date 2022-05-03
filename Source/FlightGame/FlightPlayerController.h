// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlightPlayerController.generated.h"

/**
 *
 */
UCLASS()
class FLIGHTGAME_API AFlightPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:

	class UTargetingWidget* targetingWidget_Instance;

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTargetingWidget> TargetingWidget_Class;
};
