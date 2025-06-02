// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericGraphNodeComponent.generated.h"

class UGenericGraphNode;

/**
 * 
 */
UCLASS(Blueprintable)
class GENERICGRAPHRUNTIME_API UGenericGraphNodeComponent : public UObject
{
	GENERATED_BODY()
	
private:
	UGenericGraphNode* Owner;
	bool bEditableWhenInherited = true;

public:
	UGenericGraphNode* GetOwner() const { return Owner; };
	bool IsEditableWhenInherited() const;
	EComponentCreationMethod CreationMethod;
	bool IsCreatedByConstructionScript() const
	{
		return ((CreationMethod == EComponentCreationMethod::SimpleConstructionScript) || (CreationMethod == EComponentCreationMethod::UserConstructionScript));
	}
};

/**
 *
 */
UCLASS(Blueprintable)
class GENERICGRAPHRUNTIME_API UNodeIconComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UTexture2D* Icon;

};