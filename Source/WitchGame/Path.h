// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Line.h"


/**
 * 
 */
class WITCHGAME_API Path
{
public:

	Path(TArray<FVector> Waypoints, FVector StartPos, float TurnDst);
	Path();
	~Path();

	TArray<FVector> LookPoints;
	TArray<Line> TurnBoundaries;
	int FinishLineIndex;

};
