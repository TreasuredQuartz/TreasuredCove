#include "Global/AbilitySystem/SkillTree/SkillTreeFactory.h"
#include "Global/AbilitySystem/SkillTree/GASkillTree.h"

#define LOCTEXT_NAMESPACE "SkillTreeFactory"

USkillTreeFactory::USkillTreeFactory()
{
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UGASkillTree::StaticClass();
}

UObject* USkillTreeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

FText USkillTreeFactory::GetDisplayName() const
{
    return LOCTEXT("FactoryName", "Skill Tree");
}

FString USkillTreeFactory::GetDefaultNewAssetName() const
{
    return "SkillTree";
}

#undef LOCTEXT_NAMESPACE