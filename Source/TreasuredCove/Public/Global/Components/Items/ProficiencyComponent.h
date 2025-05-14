// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProficiencyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UProficiencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProficiencyComponent();

	// The Proficiency that the owner of this component is associated with.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkillTrees")
	FName Name;
};
