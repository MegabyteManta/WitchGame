// Fill out your copyright notice in the Description page of Project Settings.


#include "Path.h"

Path::Path(TArray<FVector> Waypoints, FVector StartPos, float TurnDst) {
	LookPoints = Waypoints;
	TurnBoundaries.Init(Line(), Waypoints.Num());
	FinishLineIndex = TurnBoundaries.Num() - 1;

	FVector PreviousPoint = StartPos;
	for (int i = 0; i < LookPoints.Num(); i++) {
		FVector CurrentPoint = LookPoints[i];
		FVector DirToCurrentPoint = CurrentPoint - PreviousPoint;
		DirToCurrentPoint.Normalize();
		FVector TurnBoundaryPoint = (i == FinishLineIndex) ? CurrentPoint : CurrentPoint - DirToCurrentPoint*TurnDst;
		TurnBoundaries[i] = Line(TurnBoundaryPoint, PreviousPoint - DirToCurrentPoint * TurnDst);
		PreviousPoint = TurnBoundaryPoint;
	}
}

Path::Path() {

}

Path::~Path() {

}