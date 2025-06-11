// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/Interface_AssetUserData.h"
#include "GenericGraphNodeComponent.generated.h"

class UGenericGraphNode;
class UAssetUserData;
class UActorComponent;

/**
 * 
 */
UCLASS(DefaultToInstanced, BlueprintType, Blueprintable, abstract, meta = (ShortTooltip = "A GenericGraphNodeComponent is a reusable component that can be added to any Generic Graph Node."), config=Engine, MinimalAPI)
class UGenericGraphNodeComponent : public UObject, public IInterface_AssetUserData
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

protected:
	/** Array of user data stored with the component */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Instanced, Category = AssetUserData)
	TArray<TObjectPtr<UAssetUserData>> AssetUserData;

#if WITH_EDITORONLY_DATA
	/** Array of user data stored with the component */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Instanced, Category = AssetUserData)
	TArray<TObjectPtr<UAssetUserData>> AssetUserDataEditorOnly;
#endif

public:
	void SetOwner(UGenericGraphNode* NewOwner) { Owner = NewOwner; };
	UGenericGraphNode* GetOwner() const { return Owner; };
	GENERICGRAPHRUNTIME_API bool IsEditableWhenInherited() const;
	EComponentCreationMethod CreationMethod;
	bool IsCreatedByConstructionScript() const
	{
		return ((CreationMethod == EComponentCreationMethod::SimpleConstructionScript) || (CreationMethod == EComponentCreationMethod::UserConstructionScript));
	}

	FString GetReadableName() const { return ""; };

	bool IsRegistered() const { return bRegistered; }
	
	GENERICGRAPHRUNTIME_API void OnComponentCreated();
	GENERICGRAPHRUNTIME_API void RegisterComponent();


	GENERICGRAPHRUNTIME_API void UnregisterComponent();
	GENERICGRAPHRUNTIME_API void ExecuteUnregisterEvents();
	void OnUnregister() { bRegistered = false; }
	void UninitializeComponent() { bHasBeenInitialized = false; }
	void EndPlay(EEndPlayReason::Type Reason);
	GENERICGRAPHRUNTIME_API void DestroyComponent();
	GENERICGRAPHRUNTIME_API void OnComponentDestroyed();

	//~ Begin IInterface_AssetUserData Interface
	GENERICGRAPHRUNTIME_API virtual void AddAssetUserData(UAssetUserData* InUserData) override;
	GENERICGRAPHRUNTIME_API virtual void RemoveUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
	GENERICGRAPHRUNTIME_API virtual UAssetUserData* GetAssetUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
	GENERICGRAPHRUNTIME_API virtual const TArray<UAssetUserData*>* GetAssetUserDataArray() const override;
	//~ End IInterface_AssetUserData Interface
};

/**
 *
 */
UCLASS(ClassGroup = (Common), meta = (BlueprintSpawnableComponent))
class GENERICGRAPHRUNTIME_API UNodeIconComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	UTexture2D* Icon;

};

/**
 *
 */
UCLASS(ClassGroup = (Common), meta = (BlueprintSpawnableComponent))
class GENERICGRAPHRUNTIME_API UNodeNameComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FText DisplayName;

};

/**
 *
 */
UCLASS(ClassGroup = (Common), meta = (BlueprintSpawnableComponent))
class GENERICGRAPHRUNTIME_API UNodeAbilityComponent : public UGenericGraphNodeComponent
{
	GENERATED_BODY()

private:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	// TSubclassOf<class UGameplayAbility> Ability;

};