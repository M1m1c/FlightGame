// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingWidget.h"

#include "Runtime/UMG/Public/UMG.h"

void UTargetingWidget::Initalise(APlayerController* owner)
{
	owningPlayer = owner;
}

void UTargetingWidget::SetTarget(AActor* target)
{
	currentTarget = target;
	if (NameTextBlock)
	{
		NameTextBlock->SetText(FText::FromString(currentTarget->GetName()));
	}
}

void UTargetingWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (!owningPlayer) { return; }
	if (!currentTarget) { return; }

	
	FVector2D screenPos;	
	int32 sizeX;
	int32 sizeY;

	owningPlayer->ProjectWorldLocationToScreen(currentTarget->GetActorLocation(), screenPos, true);	
	owningPlayer->GetViewportSize(sizeX, sizeY);

	auto clampedX = FMath::Clamp((int32)screenPos.X, offset, sizeX-offset);
	auto clampedY = FMath::Clamp((int32)screenPos.Y, offset, sizeY-offset);
	screenPos = FVector2D(clampedX, clampedY);

	SetPositionInViewport(screenPos);
}
