// Fill out your copyright notice in the Description page of Project Settings.


#include "OrganSystem.h"
#include "AnatomicalStructure.h"

// Sets default values for this component's properties
UOrganSystem::UOrganSystem()
{
	// ...
}

void UOrganSystem::Initialize(UAnatomicalStructure* InParent)
{
	Parent = InParent;
}

