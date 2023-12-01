// Fill out your copyright notice in the Description page of Project Settings.


#include "PlutoBaseGameMode.h"
#include "Kismet/GameplayStatics.h"

void APlutoBaseGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(CountDownTimerHandle, this,
		&APlutoBaseGameMode::CountDownTimer, 1.0f, true, 1.0f);

}

void APlutoBaseGameMode::RestartGameplay(bool Won)
{
	if (Won)
	{
		ResetLevel();
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this,
			&APlutoBaseGameMode::ResetLevel, 3.0f);
	}
}

void APlutoBaseGameMode::ResetLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), "Gameplay");
}

void APlutoBaseGameMode::CountDownTimer()
{
	TimerCount--;

	if(TimerCount == 0)
	{
		GetWorldTimerManager().ClearTimer(CountDownTimerHandle);
		ResetLevel();
	}
}

