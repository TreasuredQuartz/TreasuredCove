// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComponentTypeRegistry.h"

class UGenericGraphNodeComponent;

typedef TSharedPtr<class FGenericNodeComponentClassComboEntry> FGenericNodeComponentClassComboEntryPtr;

/**
 * 
 */
struct GENERICGRAPHEDITOR_API FGenericNodeComponentTypeRegistry
{
	static FGenericNodeComponentTypeRegistry& Get();

	/**
	 * Called when the user changes the text in the search box.
	 * @OutComponentList Pointer that will be set to the (globally shared) component type list
	 * @return Deleate that can be used to handle change notifications. change notifications are raised when entries are
	 *	added or removed from the component type list
	 */
	FOnComponentTypeListChanged& SubscribeToComponentList(TArray<FGenericNodeComponentClassComboEntryPtr>*& OutComponentList);
	FOnComponentTypeListChanged& SubscribeToComponentList(const TArray<FComponentTypeEntry>*& OutComponentList);
	FOnComponentTypeListChanged& GetOnComponentTypeListChanged();

	/**
	 * Called when a specific class has been updated and should force the component type registry to update as well
	 */
	void InvalidateClass(TSubclassOf<UGenericGraphNodeComponent> ClassToUpdate);

	/** Schedules a full update of the component type registry on the next frame */
	void Invalidate();

	/**
	 * Attempts to locate the class entry corresponding to the given object path.
	 */
	FGenericNodeComponentClassComboEntryPtr FindClassEntryForObjectPath(FTopLevelAssetPath InObjectPath) const;

private:
	void OnReloadComplete(EReloadCompleteReason Reason);
	void OnBlueprintGeneratedClassUnloaded(UBlueprintGeneratedClass* BlueprintGeneratedClass);

private:
	FGenericNodeComponentTypeRegistry();
	~FGenericNodeComponentTypeRegistry();
	struct FGenericNodeComponentTypeRegistryData* Data;
};
