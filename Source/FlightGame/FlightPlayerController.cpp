// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightPlayerController.h"

#include "TargetingWidget.h"
#include "StarFighter.h"

//#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/UserWidget.h"

void AFlightPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(TargetingWidget_Class)) { return; }
	if (!IsLocalPlayerController()) { return; }
	targetingWidget_Instance = CreateWidget<UTargetingWidget>(this, TargetingWidget_Class);
	targetingWidget_Instance->AddToViewport();
	targetingWidget_Instance->Initalise(this);

	auto pawn = GetPawn();
	if (!ensure(pawn)) { return; }

	auto starFighter = Cast<AStarFighter>(pawn);
	if (!starFighter) { return; }
	starFighter->OnChangeTarget.AddDynamic(targetingWidget_Instance, &UTargetingWidget::SetTarget);
}
