// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Framework/GAProgressionManager.h"
#include "Global/Components/Characters/HexProgressionComponent.h"
#include "Global/AbilitySystem/SkillTree/GASkillTreeComponent.h"

AGAProgressionManager::AGAProgressionManager()
{
	// Hex Grid Component
	SkillHexGridComponent =
		CreateDefaultSubobject<UHexProgressionComponent>(TEXT("Hex Grid"));

	// Skill Tree Component
	SkillTreeComponent =
		CreateDefaultSubobject<UGASkillTreeComponent>(TEXT("Skill Trees"));
}
