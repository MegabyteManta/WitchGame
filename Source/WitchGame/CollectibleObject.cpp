// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectibleObject.h"
#include "WitchCharacter.h"



// Sets default values
ACollectibleObject::ACollectibleObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACollectibleObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollectibleObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("TICKING"));
	
}

