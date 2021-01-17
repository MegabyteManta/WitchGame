// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"


// Sets default values
APathfinding::APathfinding()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APathfinding::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APathfinding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//FindPath(Seeker->GetActorLocation(), Target->GetActorLocation());
}

TArray<FVector> APathfinding::FindPath(FVector StartPos, FVector TargetPos) {
	//Start/Target nodes found from world positions
	Node StartNode = Grid->NodeFromWorldPoint(StartPos);
	Node TargetNode = Grid->NodeFromWorldPoint(TargetPos);

	//TArrays can be heaped
	//Open is the set of neighbors visited but not yet travelled
	TArray<Node> OpenHeap;
	TSet<Node> OpenSet;

	//Closed is the set of nodes travelled
	TSet<Node> ClosedSet;

	//Start off with the start Node
	OpenSet.Add(StartNode);
	OpenHeap.HeapPush(StartNode);

	//Finds path, continuously adding visited nodes to OpenHeap
	//O(N) if path never found
	while (OpenHeap.Num() > 0) {


		//Get the current Node from the top of the heap
		Node CurrentNode;
		OpenHeap.HeapPop(CurrentNode);
		if (bDebugConsideredNodes) DrawDebugBox(GetWorld(), CurrentNode.WorldPosition, FVector(Grid->NodeRadius*0.5f, Grid->NodeRadius*0.5f, Grid->NodeRadius*0.5f), FColor::Green, true, -1, 0, 10);

		//Remove the current Node from the Open set, and add it to the Closed Set
		//i.e. the Node is being travelled, so it never needs to be visited again
		OpenSet.Remove(CurrentNode);
		ClosedSet.Add(CurrentNode);

		//If the current Node is the Target Node, the path was found
		if (CurrentNode == TargetNode) {
			TArray<FVector> Waypoints = RetracePath(StartNode, CurrentNode);
			return Waypoints;
		}

		//Go through all of the current Node's neighbors, calculate their costs,
		//and add them to the Open set
		for (Node Neighbor : Grid->GetNeighbors(CurrentNode)) {
		//for (int i = 0; i < Grid->GetNeighbors(CurrentNode))
			//Don't check unwalkable or travelled Nodes
			if (!Neighbor.Walkable || ClosedSet.Contains(Neighbor)) {
				continue;
			}

			//Calculate the cost from start to the neighbor, and replace parent and cost if cost is less
			int NewCostToNeighbor = CurrentNode.Cost.GCost + GetDistance(CurrentNode, Neighbor);
			/*
			if (NewCostToNeighbor < Neighbor.Cost.GCost || !OpenSet.Contains(Neighbor)) {
				Neighbor.Cost = CostVector(GetDistance(Neighbor, TargetNode), NewCostToNeighbor);
				//Neighbor.parent = &CurrentNode;
				Neighbor.HasParent = true;
				Neighbor.ParentPos = CurrentNode.WorldPosition;
				Grid->grid[Neighbor.X][Neighbor.Y][Neighbor.Z] = Neighbor;
			*/

				//Add the neighbor to Open (visited)
				if (!OpenSet.Contains(Neighbor)) {

					Neighbor.Cost = CostVector(GetDistance(Neighbor, TargetNode), NewCostToNeighbor);
					//Neighbor.parent = &CurrentNode;
					Neighbor.HasParent = true;
					Neighbor.ParentPos = CurrentNode.WorldPosition;
					Grid->grid[Neighbor.X][Neighbor.Y][Neighbor.Z] = Neighbor;

					OpenHeap.HeapPush(Neighbor);
					OpenSet.Add(Neighbor);
				}
				
				//I AM MAKING THE DECISION TO FORGO ACCURACY FOR SPEED
				/*
				else {
					
					if (NewCostToNeighbor < Neighbor.Cost.GCost) {
						UE_LOG(LogTemp, Warning, TEXT("Warning: OpenSet contained neighbor."));
						
						//OpenHeap.Remove(Neighbor);
						//OpenHeap.Heapify();
						//OpenHeap.HeapPush(Neighbor);
						
						
						OpenHeap[OpenHeap.Find(Neighbor)].Cost = CostVector(GetDistance(Neighbor, TargetNode), NewCostToNeighbor);
						OpenHeap.Heapify();
						Neighbor.HasParent = true;
						Neighbor.ParentPos = CurrentNode.WorldPosition;
						Grid->grid[Neighbor.X][Neighbor.Y][Neighbor.Z] = Neighbor;
					}
					

					//OpenHeap.hea
				}
				*/
				
			//}
		}
	}
	return TArray<FVector>();
}

//Backtracking, get path after finding end node
TArray<FVector> APathfinding::RetracePath(Node StartNode, Node EndNode) {
	TArray<Node> Path = TArray<Node>();
	Node CurrentNode = EndNode;

	//Reconstruct path (backwards)
	while (CurrentNode != StartNode) {
		Path.Add(CurrentNode);
		CurrentNode = Grid->NodeFromWorldPoint(CurrentNode.ParentPos);
	}
	Path.Add(StartNode);

	//Path created backwards, so reverse it
	Algo::Reverse(Path);

	//Set the path on the Grid
	//Grid->Path = Path;

	//Make an array of positions of the nodes, this is the path
	//UE_LOG(LogTemp, Warning, TEXT("I SHOULD BE GETTING COLORED"));
	TArray<FVector> NodePositions;
	Node Checkpoint = Path[0];
	NodePositions.Add(Checkpoint.WorldPosition);

	int i = 1;
	while (i < Path.Num()) {
		if (!Walkable(Checkpoint, Path[i])) {

			Checkpoint = Path[i-1];
			NodePositions.Add(Checkpoint.WorldPosition);
		}
		i++;
	}
	NodePositions.Add(Path[Path.Num()-1].WorldPosition);
	/*
	for (Node node : Path) {
		if (bDebug) DrawDebugBox(GetWorld(), node.WorldPosition, FVector(Grid->NodeRadius*0.5f, Grid->NodeRadius*0.5f, Grid->NodeRadius*0.5f), FColor::Yellow, true, -1, 0, 10);
		NodePostitions.Add(node.WorldPosition);
	}
	*/
	for (FVector Pos : NodePositions)
		if (bDebug) DrawDebugBox(GetWorld(), Pos, FVector(Grid->NodeRadius*0.5f, Grid->NodeRadius*0.5f, Grid->NodeRadius*0.5f), FColor::Yellow, true, -1, 0, 10);

	return NodePositions;
	return SimplifyPath(Path);
}

bool APathfinding::Walkable(Node NodeA, Node NodeB) {
	FVector Direction = NodeB.WorldPosition - NodeA.WorldPosition;
	Direction.Normalize();
	int Dist = FVector::Dist(NodeA.WorldPosition, NodeB.WorldPosition);
	int Step = Grid->NodeRadius * 2 / 5;
	int Travelled = 0;
	while (Travelled < Dist) {
		Travelled += Step;
		Node CurrentNode = Grid->NodeFromWorldPoint(NodeA.WorldPosition + Direction * Travelled);
		if (!CurrentNode.Walkable) return false;
		/*
		for (Node Neighbor : Grid->GetNeighbors(CurrentNode)) {
			if (!Neighbor.Walkable) return false;
		}
		*/
		

	}
	return true;
}

//This is for simplifying path down to points when direction changes
//currently unused
TArray<FVector> APathfinding::SimplifyPath(TArray<Node> Path) {
	TArray<FVector> Waypoints;
	FVector2D DirectionOld = FVector2D().ZeroVector;
	for (int i = 1; i < Path.Num(); i++) {
		FVector2D DirectionNew = FVector2D(Path[i - 1].X - Path[i].X, Path[i - 1].Y - Path[i].Y);
		if (DirectionNew != DirectionOld) {
			Waypoints.Add(Path[i - 1].WorldPosition);
			DirectionOld = DirectionNew;
		}
	}
	Waypoints.Add(Path[Path.Num() - 1].WorldPosition);
	return Waypoints;
}

//Get distance between Nodes
int APathfinding::GetDistance(Node NodeA, Node NodeB) {
	return FVector::Dist(NodeA.WorldPosition, NodeB.WorldPosition);
	int DstX = abs(NodeA.X - NodeB.X);
	int DstY = abs(NodeA.Y - NodeB.Y);
	int DstZ = abs(NodeA.Z - NodeB.Z);
	//return sqrt(DstX*DstX + DstY * DstY + DstZ * DstZ);
	return DstX + DstY + DstZ;
	int Dist = 0;
	TArray<int> SortedDistances = { DstX, DstY, DstZ };
	SortedDistances.Sort();
	//return 17 * SortedDistances[2] + 14 * (SortedDistances[1]-SortedDistances[2]) + 10 * (SortedDistances[0] - SortedDistances[1]);
	//return 17 * SortedDistances[2] + 14 * (SortedDistances[0]-SortedDistances[1]) + 10 * (SortedDistances[1] - SortedDistances[2]);
	return 17 * SortedDistances[0] + 14 * SortedDistances[1] + 10 * SortedDistances[2];

	/*
	if (DstX > DstY) return DstY * 14 + 10 * abs(DstX - DstY);
	return DstX * 14 + 10 * abs(DstX - DstY);
	*/
	if (SortedDistances[1] > SortedDistances[0]) return 17 * SortedDistances[2] + 14 * (SortedDistances[0] - SortedDistances[2]) + 10 * abs(SortedDistances[0] - SortedDistances[1] - SortedDistances[2]);
	return 17 * SortedDistances[2] + 14 * (SortedDistances[1] - SortedDistances[0]) + 10 * abs(SortedDistances[0] - SortedDistances[1] - SortedDistances[2]);
	
}
