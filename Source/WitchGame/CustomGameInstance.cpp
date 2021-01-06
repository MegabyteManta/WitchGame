// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameInstance.h"

UCustomGameInstance::UCustomGameInstance() {
	TempScore = 0;
	FinalScore = 0;
}

void UCustomGameInstance::Init() {
	Super::Init();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("CustomGameInstance Init"));

	
}

void UCustomGameInstance::Shutdown()
{
	//Super::Shutdown();
	
}

void UCustomGameInstance::EndGame() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TIMER DONE"));
}

void UCustomGameInstance::StartTimer() {
	FTimerHandle EndGameHandle;
	GetWorld()->GetTimerManager().SetTimer(EndGameHandle, this, &UCustomGameInstance::EndGame, 3.f, false);
	//UE_LOG(LogTemp, Warning, TEXT("CustomGameInstance Init"));	
}

void UCustomGameInstance::UpdateScore(int points) {
	
	TempScore += points;
	if (TempScore < 0) UE_LOG(LogTemp, Warning, TEXT("Updating score resulted in negative number"));
	UE_LOG(LogTemp, Warning, TEXT("TEMPORARY SCORE: %d"), TempScore);
}

void UCustomGameInstance::DepositScore() {
	FinalScore += TempScore;
	TempScore = 0;
	UE_LOG(LogTemp, Warning, TEXT("FINAL SCORE: %d"), FinalScore);
}
