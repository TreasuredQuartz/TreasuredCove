// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraph.h"
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
class TREASUREDCOVE_API UGASkillTree : public UGenericGraph
{
	GENERATED_BODY()

public:
	UGASkillTree();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* SkillTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UWidget> Widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Points;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Level;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// struct FGASkillTreeStyleStruct Style;
public:
	void AddPoint()
	{
		++Points;
	}

	UFUNCTION(BlueprintCallable)
	void OnSkillAquired(FString Category, UGASkillTreeNode* Node, uint8 SkillIndex);
	void BindOnSkillAquired(UGASkillTreeNode* InNode);
	void BindOnSkillAquiredToAll();

	// Called on owning skill tree components when added after NewObject<>() is called.
	void Initialize();

	UPROPERTY(BlueprintAssignable)
	FOnAquired OnAquired;
};
