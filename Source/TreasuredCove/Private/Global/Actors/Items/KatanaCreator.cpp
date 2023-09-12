// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/KatanaCreator.h"

UKatanaCreator::UKatanaCreator()
{
	Handle = NewObject<UPart>();
	Guard = NewObject<UPart>();
	Blade = NewObject<UPart>();
}

void UKatanaCreator::GenerateKatana()
{
	Handle = nullptr;
	Guard = nullptr;
	Blade = nullptr;
}

