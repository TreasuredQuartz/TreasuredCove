// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SubobjectEditorExtensionContext.h"
#include "GenericGraphNodeSEEC.generated.h"

/**
 * 
 */
UCLASS()
class GENERICGRAPHEDITOR_API UGenericGraphNodeSEEC : public USubobjectEditorExtensionContext
{
	GENERATED_BODY()

private:
	friend class SSubobjectGenericGraphNodeEditor;
};
