// Fill out your copyright notice in the Description page of Project Settings.


#include "PathTraveler.h"

// Sets default values for this component's properties
UPathTraveler::UPathTraveler()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// ...
}


// Called when the game starts
void UPathTraveler::BeginPlay()
{
	Super::BeginPlay();
	/*
	if (Positions.Num() > 0) {
		CurrentWaypoint = Positions[0];
	}
	*/
	// ...
}


// Called every frame
void UPathTraveler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//UE_LOG(LogTemp, Warning, TEXT("ACTOR DIST: %f, OUTERRADIUS: %f"), FVector::Dist(Target->GetActorLocation(), GetOwner()->GetActorLocation()), OuterRadius);

	//follow player if in view and in outer radius or in inner radius, otherwise follow path
	//I'm actually pretty sure this is unnecessary, check later but I think pathfinding covers this
	Node TargetNode = Grid->NodeFromWorldPoint(Target->GetActorLocation());
	if (TargetNode.Walkable &&
		((TargetInFieldOfView() && FVector::Dist(Target->GetActorLocation(), GetOwner()->GetActorLocation()) <= OuterRadius) ||
		(FVector::Dist(Target->GetActorLocation(), GetOwner()->GetActorLocation()) <= InnerRadius))) {

		state = FOLLOWING_PLAYER;
	}
	else {
		state = FOLLOWING_PATH;
	}

	switch (state) {
	case FOLLOWING_PATH:
		//UE_LOG(LogTemp, Warning, TEXT("FOLLOWING PATH"));
		FollowPath(DeltaTime);
		break;
	case FOLLOWING_PLAYER:
		//UE_LOG(LogTemp, Warning, TEXT("FOLLOWING PLAYER"));
		FollowPlayer(DeltaTime);
		if (Waypoints.Num() == 0) {
			FollowPath(DeltaTime);
			TargetNodeOld = Node();
		}
		break;
	}

}

void UPathTraveler::FollowPath(float DeltaTime) {
	//If a path is specified
	//UE_LOG(LogTemp, Warning, TEXT("IM DOING SOMETHING"));
	if (Positions.Num() > 0) {
		//and the path hasn't ended yet
		if (!PathEnded) {
			Node CurrentNode = Grid->NodeFromWorldPoint(GetOwner()->GetActorLocation());
			Node PositionNode = Grid->NodeFromWorldPoint(Positions[PositionIndex]);
			//if the actor reached the position, increase the position index
			if (CurrentNode == PositionNode) {
				PositionIndex++;
				//If the end of the path is reached, repeat of just stop
				if (PositionIndex >= Positions.Num()) {
					PathEnded = !bRepeatPath;
					PositionIndex = 0;
				}
			}
			Travel(DeltaTime, Positions[PositionIndex]);
		}
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("NO POSITIONS"));
	}
}

//only follow player if the player hasn't been reached yet
//if the player has been reached, resume following if the player moves
void UPathTraveler::FollowPlayer(float DeltaTime) {
	Node TargetNodeNew = Grid->NodeFromWorldPoint(Target->GetActorLocation());
	if (TargetNodeNew != TargetNodeOld) {
		ReachedPlayer = false;
	}
	if (!ReachedPlayer) Travel(DeltaTime, Target->GetActorLocation());
}

void UPathTraveler::Travel(float DeltaTime, FVector TargetLocation)
{
	//UE_LOG(LogTemp, Warning, TEXT("TRAVELLING"));
	FVector2D ActorLocation2D = FVector2D(GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y);
	FVector ActorLocation = GetOwner()->GetActorLocation();
	FVector2D CurrentWaypoint2D = FVector2D(CurrentWaypoint.X, CurrentWaypoint.Y);
	FRotator CurrentWaypointRotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), CurrentWaypoint);
	//I should just use nodes here
	//If the actor has reached the current waypoint, move onto the next waypoint
	//UE_LOG(LogTemp, Warning, TEXT("CURRENT WAYPOINT: %s"), *CurrentWaypoint.ToString());

	//The algorithm gets fucked when trying to recalculate around boundaries because it rapidly switches between 2 nodes
	//This fixes it by forcing it to move until it's not next to a boundary
	//Maybe change it so that it only affects it if it needs to cross a boundary, not just be close to it
	bool BoundaryCleared = true;
	for (Node Neighbor : Grid->GetNeighbors(Grid->NodeFromWorldPoint(GetOwner()->GetActorLocation()))) {
		if (!Neighbor.Walkable) {
			BoundaryCleared = false;
			break;
		}
	}
	//BoundaryCleared = true;
	if (CurrentWaypointsNotSet || ActorLocation.Equals(CurrentWaypoint, 5.0f) || ((BoundaryCleared || WaypointIndex == Waypoints.Num() - 1) && Grid->NodeFromWorldPoint(TargetLocation) != TargetNodeOld)) {
		CurrentWaypointsNotSet = false;
		Node TargetNodeNew = Grid->NodeFromWorldPoint(TargetLocation);
		//If the target node changed, recalculate the path
		if ((BoundaryCleared || WaypointIndex == Waypoints.Num() - 1) && TargetNodeNew != TargetNodeOld) {
			//UE_LOG(LogTemp, Warning, TEXT("RECALCING PATH"));
			//Waypoints is the list of node positions
			Waypoints = Pathfinder->FindPath(GetOwner()->GetActorLocation(), TargetLocation);
			if (Waypoints.Num() == 0) {
				UE_LOG(LogTemp, Warning, TEXT("ERROR: THERE ARE NO WAYPOINTS"));
				FollowPath(DeltaTime);
				return;
			}
			//Restart following
			CurrentWaypoint = Waypoints[0];
			WaypointIndex = 0;
			TargetNodeOld = TargetNodeNew;
			ReachedPlayer = false;
		}
		//Go to the next waypoint
		WaypointIndex++;


		//Make sure no index out of range error
		if (WaypointIndex >= Waypoints.Num()) {
			ReachedPlayer = true;
			//UE_LOG(LogTemp, Warning, TEXT("ERROR: WAYPOINT INDEX >= WAYPOINTS.NUM"));
			UE_LOG(LogTemp, Warning, TEXT("ReachedPlayer"));
			return;
		}

		//Get the next waypoint
		CurrentWaypoint = Waypoints[WaypointIndex];
		//CurrentWaypoint2D = FVector2D(CurrentWaypoint.X, CurrentWaypoint.Y);
		if (WaypointIndex == Waypoints.Num() - 1) {
			CurrentWaypointRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
		}
		else {
			CurrentWaypointRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, CurrentWaypoint);
		}
	}
	//Move and rotate the actor towards the current waypoint
	//Just rotate towards target instead of node (the last waypoint is always the target's node)
	if (WaypointIndex == Waypoints.Num() - 1) {
		CurrentWaypointRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, TargetLocation);
	}
	else {
		CurrentWaypointRotation = UKismetMathLibrary::FindLookAtRotation(ActorLocation, CurrentWaypoint);
	}
	FRotator RotationStep = FMath::RInterpConstantTo(GetOwner()->GetActorRotation(), CurrentWaypointRotation, DeltaTime, RotationRate);
	GetOwner()->SetActorRotation(FRotator(RotationStep.Pitch, RotationStep.Yaw, 0));
	
	FVector LocationStep = FMath::VInterpConstantTo(ActorLocation, CurrentWaypoint, DeltaTime, Velocity);
	GetOwner()->SetActorLocation(LocationStep);
	//FVector2D LocationStepp = FMath::Vector2DInterpConstantTo(ActorLocation2D, CurrentWaypoint2D, DeltaTime, Velocity);
	//GetOwner()->SetActorLocation(FVector(LocationStepp.X, LocationStepp.Y, GetOwner()->GetActorLocation().Z));
	UE_LOG(LogTemp, Warning, TEXT("CurrentWaypoint: %s"), *CurrentWaypoint.ToString());
	UE_LOG(LogTemp, Warning, TEXT("LOCATION STEP: %s, ROTATION STEP: %s"), *LocationStep.ToString(), *RotationStep.ToString());
}

//Is target in view of the actor?
bool UPathTraveler::TargetInFieldOfView() {
	//Normalized dot product = cos(theta)
	FVector DistanceVector = Target->GetActorLocation() - GetOwner()->GetActorLocation();
	float View = FVector::DotProduct(GetOwner()->GetActorForwardVector(), DistanceVector.GetSafeNormal());
	//UE_LOG(LogTemp, Warning, TEXT("DOT PRODUCT: %f, ViewThreshold: %f"), View, ViewThreshold);
	return View >= ViewThreshold;

}
