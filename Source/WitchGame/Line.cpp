// Fill out your copyright notice in the Description page of Project Settings.


#include "Line.h"

Line::Line(FVector PointOnLine, FVector PointPerpendicularToLine) {
	float DX = PointOnLine.X - PointPerpendicularToLine.X;
	float DY = PointOnLine.Y - PointPerpendicularToLine.Y;
	float DZ = PointOnLine.Z - PointPerpendicularToLine.Z;

	Gradient = FVector(DX, DY, DZ);
	if (DX == 0) DX = -1 / VerticalLineGradient;
	if (DY == 0) DY = -1 / VerticalLineGradient;
	if (DZ == 0) DZ = - 1 / VerticalLineGradient;
	GradientPerpendicular = FVector(-1 / DX, -1 / DY, -1 / DZ);
	PointOnLine1 = PointOnLine;
	PointOnLine2 = PointOnLine + Gradient;

	ApproachSide = GetSide(PointPerpendicularToLine);
}

//Find what side of gradient plane Point is on (gradient plane to waypoint line)
bool Line::GetSide(FVector Point) {
	float d = -FVector::DotProduct(Gradient, PointOnLine1);
	return Dot4(FVector4(Gradient, d), FVector4(Point, 1)) > 0;
}

bool Line::HasCrossedLine(FVector Point) {
	return GetSide(Point) != ApproachSide;
}

Line::Line() {

}

Line::~Line() {

}