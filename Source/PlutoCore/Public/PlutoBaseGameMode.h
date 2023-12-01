// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlutoBaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PLUTOCORE_API APlutoBaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	void RestartGameplay(bool Won);

private:
	void ResetLevel();

public:
	UPROPERTY(BlueprintReadOnly)
		int TimerCount = 300;

private:
	FTimerHandle CountDownTimerHandle = FTimerHandle();

	void CountDownTimer();

public:
	void BeginPlay() override;
};
