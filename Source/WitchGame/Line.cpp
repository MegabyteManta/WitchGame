// Fill out your copyright notice in the Description page of Project Settings.


#include "Line.h"

Line::Line(FVector PointOnLine, FVector PointPerpendicularToLine) {
	float DX = PointOnLine.x - PointPerpendicularToLine.x;
	float DY = PointOnLine.y - PointPerpendicularToLine.y;
	float DZ = PointOnLine.z - PointPerpendicularToLine.z;

	if (DX == 0) {
		GradientPerpendicular = VerticalLineGradient;
	}
	else {
		GradientPerpendicular = DY
	}
}

Line::Line()
{
}

Line::~Line() 
{
}
