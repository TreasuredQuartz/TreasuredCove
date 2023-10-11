// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/ItemData.h"
#include "Global/Actors/Items/ItemAttributeData.h"
#include "GASkillTree.h"

TMap<FString, FText> UItemTypeData::GetPopupProperties() const
{
	return TMap<FString, FText>({ 
		TPairInitializer<const FString&, const FText&>("Type", ProficiencyTree->DisplayName) 
		});
}

TMap<FString, FText> UItemCreditData::GetPopupProperties() const
{
	return TMap<FString, FText>({
		TPairInitializer<const FString&, const FText&>("CreatorName", CreatorName)
		});
}
