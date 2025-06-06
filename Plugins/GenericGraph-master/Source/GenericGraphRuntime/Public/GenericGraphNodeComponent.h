// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GenericGraphNodeComponent.generated.h"

class UGenericGraphNode;

/**
 * 
 */
UCLASS(DefaultToInstanced, Blueprintable, abstract)
class GENERICGRAPHRUNTIME_API UGenericGraphNodeComponent : public UObject
{
	GENERATED_BODY()

private:
	UGenericGraphNode* Owner;
	bool bEditableWhenInherited = true;
	bool bIsReadyForReplication = true;
	bool bHasBeenInitialized = false;
	bool bIsBeingDestroyed = false;
	bool bHasBeenCreated = false;
	bool bHasBegunPlay = false;
	bool bRegistered = false;

public:
	UGenericGraphNode* GetOwner() const { return Owner; };
	bool IsEditableWhenInherited() const;
	EComponentCreationMethod CreationMethod;
	bool IsCreatedByConstructionScript() const
	{
		return ((CreationMethod == EComponentCreationMethod::SimpleConstructionScript) || (CreationMethod == EComponentCreationMethod::UserConstructionScript));
	}

	FString GetReadableName() const { return ""; };

	bool IsRegistered() const { return bRegistered; }

	void OnComponentCreated();
	void RegisterComponent();


	void UnregisterComponent();
	void ExecuteUnregisterEvents();
	void OnUnregister() { bRegistered = false; }
	void UninitializeComponent() { bHasBeenInitialized = false; }
	void EndPlay(EEndPlayReason::Type Reason);
	void DestroyComponent();
	void OnComponentDestroyed();
};

/**
 *
 */
UCLASS(ClassGroup = (Common), meta = (BlueprintSpawnableComponent))
class GENERICGRAPHRUNTIME_API UNodeIconComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UTexture2D* Icon;

};