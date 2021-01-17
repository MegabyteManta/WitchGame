// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Line.h"

/**
 *
 */
class WITCHGAME_API Line
{
public:
	Line(FVector PointOnLine, FVector PointPerpendicularToLine);
	Line();
	~Line();

	float VerticalLineGradient = 1e5f;

	FVector Gradient;
	FVector PointOnLine1;
	FVector PointOnLine2;

	FVector GradientPerpendicular;

	bool ApproachSide = false;

	bool GetSide(FVector Point);

	bool HasCrossedLine(FVector Point);
};
