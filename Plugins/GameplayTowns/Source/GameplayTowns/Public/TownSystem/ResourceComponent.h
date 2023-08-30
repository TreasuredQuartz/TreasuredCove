//

// Header files
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"

UCLASS(BlueprintType)
class UResourceComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	TMap<FName, float> Resources;

public:
	//
	void AddToResource(FName Name, float Amount);
	//
	void RemoveFromResource(FName Name, float Amount);
	//
	TMap<FName, float> GetAllResources() const { return Resources; };
};