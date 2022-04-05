// Fill out your copyright notice in the Description page of Project Settings.


#include "StarFighter.h"

// Sets default values
AStarFighter::AStarFighter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = root;

}

// Called when the game starts or when spawned
void AStarFighter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStarFighter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector location = GetActorLocation();

	location += GetActorForwardVector() * 100.0f * DeltaTime;

	SetActorLocation(location);
}

// Called to bind functionality to input
void AStarFighter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

