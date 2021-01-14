// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node(bool _Walkable, FVector _WorldPosition, int _X, int _Y, int _Z)
{
	Walkable = _Walkable;
	WorldPosition = _WorldPosition;
	X = _X;
	Y = _Y;
	Z = _Z;
	GridPos = FVector(X, Y, Z);
}

Node::Node()
{
}

Node::~Node()
{
	//parent = nullptr;
}
