// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "TownSystemInterface.generated.h"

class UTownSystemComponent;

/** Gameplay Towns Interface  
* Previous UInterfaceType:
*
* - MinimalAPI, meta = (CannotImplementInterfaceInBlueprint)
* 
* Could not cast to Interface in Blueprint!
* Why does the GAS (Gameplay Ability System) use the previous Interface Type?
* Use the library to grab the component instead.
*/
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTownSystemInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class GAMEPLAYTOWNS_API ITownSystemInterface
{
	GENERATED_IINTERFACE_BODY()

	/** Getter for component */
	virtual UTownSystemComponent* GetTownSystemComponent() const = 0;

	virtual void UpdateGameSpeed(float SpeedMultiplier);
};
