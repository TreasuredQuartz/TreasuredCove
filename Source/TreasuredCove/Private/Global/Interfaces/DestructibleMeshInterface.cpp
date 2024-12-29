// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Interfaces/DestructibleMeshInterface.h"

// Add default functionality here for any IDestructibleMeshInterface functions that are not pure virtual.
void IDestructibleMeshInterface::InflictDamageToMesh(float Damage, float Strength, const FVector& HitLoc)
{
	OnDamageInflicted(Damage, Strength, HitLoc);
}