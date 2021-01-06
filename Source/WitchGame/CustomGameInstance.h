// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class WITCHGAME_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()

	public:
		UCustomGameInstance();
		
		UFUNCTION(BlueprintCallable, Category = "Timer")
		void StartTimer();
		UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateScore(int points);
		UFUNCTION(BlueprintCallable, Category = "Scoring")
		void DepositScore();



	protected:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
		int TempScore;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
		int FinalScore;

		virtual void Init() override;

		virtual void Shutdown() override;

		void EndGame();
};
