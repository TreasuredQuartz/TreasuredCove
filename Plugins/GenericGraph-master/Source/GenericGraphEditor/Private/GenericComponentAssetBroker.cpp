// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "GenericComponentAssetBroker.h"

//////////////////////////////////////////////////////////////////////////
// FGenericComponentAssetBrokerageage statics

TMap<UClass*, FComponentClassList> FGenericComponentAssetBrokerage::AssetToComponentClassMap;
TMap< TSubclassOf<UGenericGraphNodeComponent>, TSharedPtr<IComponentAssetBroker> > FGenericComponentAssetBrokerage::ComponentToBrokerMap;
TMap< UClass*, TArray< TSharedPtr<IComponentAssetBroker> > > FGenericComponentAssetBrokerage::AssetToBrokerMap;

bool FGenericComponentAssetBrokerage::bInitializedBuiltinMap = false;
bool FGenericComponentAssetBrokerage::bShutSystemDown = false;

//////////////////////////////////////////////////////////////////////////
// FGenericComponentAssetBrokerageage

/** Find set of components that support this asset */
FComponentClassList FGenericComponentAssetBrokerage::GetComponentsForAsset(const UObject* InAsset)
{
	InitializeMap();
	FComponentClassList OutClasses;

	if (InAsset != NULL)
	{
		for (UClass* Class = InAsset->GetClass(); Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			if (FComponentClassList* pTypesForClass = AssetToComponentClassMap.Find(Class))
			{
				OutClasses.Append(*pTypesForClass);
			}
		}
	}

	return OutClasses;
}

TSubclassOf<UGenericGraphNodeComponent> FGenericComponentAssetBrokerage::GetPrimaryComponentForAsset(UClass* InAssetClass)
{
	InitializeMap();

	if (InAssetClass != NULL)
	{
		for (UClass* Class = InAssetClass; Class != UObject::StaticClass(); Class = Class->GetSuperClass())
		{
			if (FComponentClassList* pTypesForClass = AssetToComponentClassMap.Find(Class))
			{
				if (pTypesForClass->Num() > 0)
				{
					return (*pTypesForClass)[0];
				}
			}
		}
	}

	return NULL;
}

/** Assign the assigned asset to the supplied component */
bool FGenericComponentAssetBrokerage::AssignAssetToComponent(UGenericGraphNodeComponent* InComponent, UObject* InAsset)
{
	InitializeMap();

	if (InComponent != NULL)
	{
		TSharedPtr<IComponentAssetBroker> Broker = FindBrokerByComponentType(InComponent->GetClass());
		if (Broker.IsValid())
		{
			return Broker->AssignAssetToComponent(InComponent, InAsset);
		}
	}

	return false;
}

UObject* FGenericComponentAssetBrokerage::GetAssetFromComponent(UGenericGraphNodeComponent* InComponent)
{
	InitializeMap();

	if (InComponent != NULL)
	{
		TSharedPtr<IComponentAssetBroker> Broker = FindBrokerByComponentType(InComponent->GetClass());
		if (Broker.IsValid())
		{
			return Broker->GetAssetFromComponent(InComponent);
		}
	}

	return NULL;
}

/** See if this component supports assets of any type */
bool FGenericComponentAssetBrokerage::SupportsAssets(UGenericGraphNodeComponent* InComponent)
{
	InitializeMap();

	if (InComponent == NULL)
	{
		return false;
	}
	else
	{
		TSharedPtr<IComponentAssetBroker> Broker = FindBrokerByComponentType(InComponent->GetClass());
		return Broker.IsValid();
	}
}

void FGenericComponentAssetBrokerage::PRIVATE_ShutdownBrokerage()
{
	check(!bShutSystemDown);
	bShutSystemDown = true;

	AssetToComponentClassMap.Empty();
	AssetToBrokerMap.Empty();
	ComponentToBrokerMap.Empty();
}

void FGenericComponentAssetBrokerage::InitializeMap()
{
	check(!bShutSystemDown);

	if (!bInitializedBuiltinMap)
	{
		bInitializedBuiltinMap = true;

		// Register Brokers here...
	}
}

void FGenericComponentAssetBrokerage::RegisterBroker(TSharedPtr<IComponentAssetBroker> Broker, TSubclassOf<UGenericGraphNodeComponent> InComponentClass, bool bSetAsPrimary, bool bMapComponentForAssets)
{
	InitializeMap();

	check(Broker.IsValid());

	UClass* AssetClass = Broker->GetSupportedAssetClass();
	check((AssetClass != NULL) && (AssetClass != UObject::StaticClass()));

	checkf(!ComponentToBrokerMap.Contains(InComponentClass), TEXT("Component class already has a registered broker; you have to chain them yourself."));
	ComponentToBrokerMap.Add(InComponentClass, Broker);

	if (bSetAsPrimary)
	{
		AssetToBrokerMap.FindOrAdd(AssetClass).Insert(Broker, 0);
	}
	else
	{
		AssetToBrokerMap.FindOrAdd(AssetClass).Add(Broker);
	}

	if (bMapComponentForAssets)
	{
		FComponentClassList& ValidComponentTypes = AssetToComponentClassMap.FindOrAdd(AssetClass);
		if (bSetAsPrimary)
		{
			ValidComponentTypes.Insert(InComponentClass, 0);
		}
		else
		{
			ValidComponentTypes.Add(InComponentClass);
		}
	}
}

void FGenericComponentAssetBrokerage::UnregisterBroker(TSharedPtr<IComponentAssetBroker> Broker)
{
	UClass* AssetClass = Broker->GetSupportedAssetClass();

	if (TArray< TSharedPtr<IComponentAssetBroker> >* pBrokerList = AssetToBrokerMap.Find(AssetClass))
	{
		pBrokerList->Remove(Broker);
	}

	if (FComponentClassList* pTypesForClass = AssetToComponentClassMap.Find(AssetClass))
	{
		for (auto ComponentTypeIt = ComponentToBrokerMap.CreateIterator(); ComponentTypeIt; ++ComponentTypeIt)
		{
			TSubclassOf<UGenericGraphNodeComponent> ComponentClass = ComponentTypeIt.Key();

			if (ComponentTypeIt.Value() == Broker)
			{
				ComponentTypeIt.RemoveCurrent();
				pTypesForClass->Remove(ComponentClass);
			}
		}

		if (pTypesForClass->Num() == 0)
		{
			AssetToComponentClassMap.Remove(AssetClass);
		}
	}
}

TSharedPtr<IComponentAssetBroker> FGenericComponentAssetBrokerage::FindBrokerByComponentType(TSubclassOf<UGenericGraphNodeComponent> InComponentClass)
{
	InitializeMap();

	return ComponentToBrokerMap.FindRef(InComponentClass);
}


TSharedPtr<IComponentAssetBroker> FGenericComponentAssetBrokerage::FindBrokerByAssetType(UClass* InAssetClass)
{
	InitializeMap();

	TArray< TSharedPtr<IComponentAssetBroker> >* pBrokerList = AssetToBrokerMap.Find(InAssetClass);
	return ((pBrokerList != NULL) && (pBrokerList->Num() > 0)) ? (*pBrokerList)[0] : NULL;
}

TArray<UClass*> FGenericComponentAssetBrokerage::GetSupportedAssets(UClass* InFilterComponentClass)
{
	InitializeMap();

	TArray< UClass* > SupportedAssets;

	for (auto ComponentTypeIt = ComponentToBrokerMap.CreateIterator(); ComponentTypeIt; ++ComponentTypeIt)
	{
		TSubclassOf<UGenericGraphNodeComponent> Component = ComponentTypeIt.Key();

		if (InFilterComponentClass == NULL || Component->IsChildOf(InFilterComponentClass))
		{
			SupportedAssets.Add(ComponentTypeIt.Value()->GetSupportedAssetClass());
		}
	}

	return SupportedAssets;
}
