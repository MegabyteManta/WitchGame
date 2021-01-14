// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
struct WITCHGAME_API Line
{
	Line(FVector PointOnLine, FVector PointPerpendicularToLine);
	Line();
	~Line();

	const float VerticalLineGradient = 1e5f;

	float Gradient;
	float YIntercept;
	FVector PointOnLine1;
	FVector PointOnLine2;

	float GradientPerpendicular;

	bool ApproachSide;
};
