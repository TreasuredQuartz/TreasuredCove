// Fill out your copyright notice in the Description page of Project Settings.


#include "Anatomy/Systems/OrganSystem.h"
#include "Anatomy/AnatomicalStructure.h"

// Sets default values for this component's properties
UOrganSystem::UOrganSystem()
{
	// ...
}

void UOrganSystem::Initialize(UAnatomicalStructure* InParent)
{
	Parent = InParent;
}

TArray<TSubclassOf<UOrgan>> UOrganSystem::GetOrganClasses() const
{
	return OrganClasses;
}

