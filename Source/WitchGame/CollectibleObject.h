// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectibleObject.generated.h"


UCLASS()
class WITCHGAME_API ACollectibleObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectibleObject();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch")
	bool Collected = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Witch")
	bool Deposited = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
