// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"
#include "GenericGraphNodeComponent.h"

//////////////////////////////////////////////////////////////////////////
// List of component types

typedef TArray< TSubclassOf<UGenericGraphNodeComponent> > FComponentClassList;

//////////////////////////////////////////////////////////////////////////
// IComponentAssetBroker

/**
 * This class knows how to get or set the asset on a particular kind of actor component
 *
 * One asset type can be associated with multiple component types, but any given component
 * type only understands how to be created from a single asset type (for now)
 */
class IComponentAssetBroker
{
public:
	/** Reports the component class this broker knows how to handle */
	//virtual TSubclassOf<UGenericGraphNodeComponent> GetSupportedComponentClass()=0;

	/** Reports the asset class this broker knows how to handle */
	virtual UClass* GetSupportedAssetClass() = 0;

	/** Assign the assigned asset to the supplied component */
	virtual bool AssignAssetToComponent(UGenericGraphNodeComponent* InComponent, UObject* InAsset) = 0;

	/** Get the currently assigned asset from the component */
	virtual UObject* GetAssetFromComponent(UGenericGraphNodeComponent* InComponent) = 0;

	virtual ~IComponentAssetBroker() {}
};

//////////////////////////////////////////////////////////////////////////
// FComponentAssetBrokerageage

/** Utility class that associates assets with component classes */
class GENERICGRAPHEDITOR_API FGenericComponentAssetBrokerage
{
public:
	/** Find set of components that support this asset */
	static TArray< TSubclassOf<UGenericGraphNodeComponent> > GetComponentsForAsset(const UObject* InAsset);

	/** Get the primary component for this asset*/
	static TSubclassOf<UGenericGraphNodeComponent> GetPrimaryComponentForAsset(UClass* InAssetClass);

	/** Assign the assigned asset to the supplied component */
	static bool AssignAssetToComponent(UGenericGraphNodeComponent* InComponent, UObject* InAsset);

	/** Get the currently assigned asset from the component */
	static UObject* GetAssetFromComponent(UGenericGraphNodeComponent* InComponent);

	/** See if this component supports assets of any type */
	static bool SupportsAssets(UGenericGraphNodeComponent* InComponent);

	/** Register a component class for a specified asset class */
	static void RegisterAssetToComponentMapping(UClass* InAssetClass, TSubclassOf<UGenericGraphNodeComponent> InComponentClass, bool bSetAsPrimary);

	/** Unregister a component type for a specified asset class */
	static void UnregisterAssetToComponentMapping(UClass* InAssetClass, TSubclassOf<UGenericGraphNodeComponent> InComponentClass);

	/** Try to find the broker for the specified component type */
	static TSharedPtr<IComponentAssetBroker> FindBrokerByComponentType(TSubclassOf<UGenericGraphNodeComponent> InComponentClass);

	/** Try to find the *primary* broker for the specified asset type */
	static TSharedPtr<IComponentAssetBroker> FindBrokerByAssetType(UClass* InAssetClass);

	/** Get the currently supported assets, optionally filtered by the supplied component class */
	static TArray<UClass*> GetSupportedAssets(UClass* InFilterComponentClass = NULL);


	/** Register a component class for a specified asset class */
	static void RegisterBroker(TSharedPtr<IComponentAssetBroker> Broker, TSubclassOf<UGenericGraphNodeComponent> InComponentClass, bool bSetAsPrimary, bool bMapComponentForAssets);

	/** Unregister a component type for a specified asset class */
	static void UnregisterBroker(TSharedPtr<IComponentAssetBroker> Broker);

	/** Shut down the brokerage; should only be called by the editor during shutdown */
	static void PRIVATE_ShutdownBrokerage();

private:
	FGenericComponentAssetBrokerage() {}

	static void InitializeMap();

private:
	// Map from an asset class to all component classes that can use it
	// The first one in the array is the 'primary'
	static TMap<UClass*, FComponentClassList> AssetToComponentClassMap;

	// Map from component type to the broker for that type
	static TMap< TSubclassOf<UGenericGraphNodeComponent>, TSharedPtr<IComponentAssetBroker> > ComponentToBrokerMap;

	// Map from asset type to the *primary* broker for that type
	static TMap< UClass*, TArray< TSharedPtr<IComponentAssetBroker> > > AssetToBrokerMap;

	static bool bInitializedBuiltinMap;
	static bool bShutSystemDown;
};