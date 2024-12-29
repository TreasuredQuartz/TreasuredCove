// Fill out your copyright notice in the Description page of Project Settings.

// Header Files
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GASkillTree.h"
#include "GASkillTreeComponent.generated.h"

// Forward Declarations
class UGASkillTree;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewSkillTreeAdded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UGASkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UAbilitySystemComponent* AbilityComponent;
	TArray<struct FGameplayAbilitySpecHandle> SkillHandles;

public:	
	// Sets default values for this component's properties
	UGASkillTreeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// These skill trees are created and added on Begin Play
	/*UPROPERTY(EditAnywhere, Category = "Abilities")
	TMap<FString, TSubclassOf<UGASkillTree>> InitialSkillTreeClasses;*/

	// These are the actual skill tree objects which may be empty.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TMap<FString, UGASkillTree*> SkillTrees;

	// This will add a new skill tree at runtime to this component
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddSkillList(FString Category, UGASkillTree* SkillTree);
	void AddSkillList(FString Category, TSubclassOf<UGASkillTree> SkillTreeClass);

	void AquireSkillList(UGASkillTree* SkillTree);

	// Delegate bound function that gets called by created skill trees.
	UFUNCTION()
	void AquireSkill(TSubclassOf<UGameplayAbilityBase> Ability);

public:
	UPROPERTY(BlueprintAssignable)
	FOnNewSkillTreeAdded OnNewSkillTreeAdded;
};
