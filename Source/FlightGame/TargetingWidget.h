// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TargetingWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTGAME_API UTargetingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	class APlayerController* owningPlayer;
	AActor* currentTarget;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* NameTextBlock;

	
	int32 offset = 25;

public:
	void Initalise(APlayerController* owner);
	
	UFUNCTION()
	void SetTarget(AActor* target);

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
};
