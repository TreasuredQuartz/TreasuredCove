// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildings/GameplayResidence.h"
#include "TownSystem/TownSystemComponent.h"
#include "TownSystem/TownSystemInterface.h"

void AGameplayResidence::ReduceFatigue(AActor* User)
{
	UTownSystemComponent* OtherTownSystem = Cast<ITownSystemInterface>(User)->GetTownSystemComponent();

	switch (BeddingQuality)
	{
	case 0:
		// OtherTownSystem->AddResource("BadSleepQuality", 1);
		break;
	case 1:
		// OtherTownSystem->AddResource("CommonSleepQuality", 1);
		break;
	case 2:
		// OtherTownSystem->AddResource("ExquisiteSleepQuality", 1);
		break;
	}
}

void AGameplayResidence::ReduceHunger(AActor* User)
{
	UTownSystemComponent* OtherTownSystem = Cast<ITownSystemInterface>(User)->GetTownSystemComponent();

	switch (FoodQuality)
	{
	case 0:
		// OtherTownSystem->AddResource("BadEatQuality", 1);
		break;
	case 1:
		// OtherTownSystem->AddResource("CommonEatQuality", 1);
		break;
	case 2:
		// OtherTownSystem->AddResource("ExquisiteEatQuality", 1);
		break;
	}
}