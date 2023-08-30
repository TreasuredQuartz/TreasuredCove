#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SkillTreeFactory.generated.h"

UCLASS()
class USkillTreeFactory : public UFactory
{
    GENERATED_BODY()

public:
    USkillTreeFactory();

    virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
    virtual FText GetDisplayName() const override;
    virtual FString GetDefaultNewAssetName() const override;
};