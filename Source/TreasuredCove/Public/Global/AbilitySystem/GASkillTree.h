// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GASkillTree.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAquired, TSubclassOf<class UGameplayAbilityBase>, Ability);

class UGASkillTreeNode;

/** Check out ReidsChannel on YouTube to find a similar 
 * Blueprint based implementation.
 * 
 * Members:
 ** Size - The Amount of elements in the linked list
 ** Points - The Currency for exchanging skills
 ** Root - Head of the linked list (The first skill)
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UGASkillTree : public UObject
{
	GENERATED_BODY()

	UGASkillTree();
private:
	int Size;
	int Points;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGASkillTreeNode* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UWidget> Widget;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// struct FGASkillTreeStyleStruct Style;
public:
	int GetSize()
	{
		return Size;
	}

	int GetHeight();

	void AddPoint()
	{
		++Points;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Abilities")
	UGASkillTreeNode* GetRoot()
	{
		return Root;
	}

	void BindOnSkillAuquiredToAll(UGASkillTreeNode* Parent);

	UFUNCTION()
	void OnSkillAquired(FString Category, UGASkillTreeNode* Node);

	void Initialize();

	UPROPERTY(BlueprintAssignable)
	FOnAquired OnAquired;
};
