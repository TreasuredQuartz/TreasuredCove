// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/GameplayLibrary.h"
#include "TownSystem/TownSystemInterface.h"
#include "TownSystem/TownSystemComponent.h"

void UGameplayLibrary::SetGameTime(float InDeltaTime, float InGameSpeed, float InInitialGameTime, float* OutGameTime, int* OutDaysPassed)
{
	*OutDaysPassed = ((InDeltaTime * InGameSpeed) + InInitialGameTime) / 24.f;
	*OutGameTime = fmodf( ((InDeltaTime * InGameSpeed) + InInitialGameTime), 24.f );
}

UTownSystemComponent* UGameplayLibrary::GetGameplayTownsComponent(AActor* Actor)
{
	if (Actor == nullptr)
	{
		return nullptr;
	}

	const ITownSystemInterface* TSI = Cast<ITownSystemInterface>(Actor);

	if (TSI)
	{
		return TSI->GetTownSystemComponent();
	} 

	// Fall back to a component search to better support BP-only actors
	return Actor->FindComponentByClass<UTownSystemComponent>();
}
