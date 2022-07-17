// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Interfaces/RepositoryInterface.h"

// Add default functionality here for any IRepsitoryInterface functions that are not pure virtual.
URepositoryComponent* IRepositoryInterface::GetRepositoryComponent_Implementation() const
{
	UE_LOG(LogTemp, Warning, TEXT("IRepositoryInterface inherited but not implemented!"));
	return nullptr;
}