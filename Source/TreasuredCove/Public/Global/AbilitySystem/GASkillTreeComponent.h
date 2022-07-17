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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TMap<FString, UGASkillTree*> SkillTrees;

	void AddSkillList(FString Category, UGASkillTree* SkillTree);
	void AquireSkill(TSubclassOf<UGameplayAbilityBase> Ability);
};
