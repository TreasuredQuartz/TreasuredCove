// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Framework/GAAbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

void UGAAbilitySystemGlobals::AllocAttributeSetInitter()
{
	GlobalAttributeSetInitter = TSharedPtr<FAttributeSetInitter>(new FAttributeSetInitterDiscreteLevels());
}

//----------------------------
// Struct Functions
//----------------------------
/*
TSubclassOf<UAttributeSet> GAFindBestAttributeClass(TArray<TSubclassOf<UAttributeSet> >& ClassList, FString PartialName)
{
	for (auto Class : ClassList)
	{
		if (Class->GetName().Contains(PartialName))
		{
			return Class;
		}
	}

	return nullptr;
}

void FAttributeSetInitterCurveEval::PreloadAttributeSetData(const TArray<UCurveTable*>& CurveData)
{
	if (!ensure(CurveData.Num() > 0))
	{
		return;
	}
	//---
	//  Get list of AttributeSet classes loaded
	//---

	TArray<TSubclassOf<UAttributeSet> > ClassList;
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* TestClass = *ClassIt;
		if (TestClass->IsChildOf(UAttributeSet::StaticClass()))
		{
			ClassList.Add(TestClass);
		}
	}

	//---
	//	Loop through CurveData table and build sets of Defaults that keyed off of Name + Level
	//---
	for (const UCurveTable* CurTable : CurveData)
	{
		for (auto It = CurTable->RowMap.CreateConstIterator(); It; ++It)
		{
			FString RowName = It.Key().ToString();
			FString ClassName;
			FString SetName;
			FString AttributeName;
			FString Temp;

			RowName.Split(TEXT("."), &ClassName, &Temp);
			Temp.Split(TEXT("."), &SetName, &AttributeName);

			if (!ensure(!ClassName.IsEmpty() && !SetName.IsEmpty() && !AttributeName.IsEmpty()))
			{
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to parse row %s in %s"), *RowName, *CurTable->GetName());
				continue;
			}

			// Find the AttributeSet

			TSubclassOf<UAttributeSet> Set = GAFindBestAttributeClass(ClassList, SetName);
			if (!Set)
			{
				// This is ok, we may have rows in here that don't correspond directly to attributes
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match AttributeSet from %s (row: %s)"), *SetName, *RowName);
				continue;
			}

			// Find the UProperty
			UProperty* Property = FindField<UProperty>(*Set, *AttributeName);
			if (!IsSupportedProperty(Property))
			{
				ABILITY_LOG(Verbose, TEXT("FAttributeSetInitterDiscreteLevels::PreloadAttributeSetData Unable to match Attribute from %s (row: %s)"), *AttributeName, *RowName);
				continue;
			}

			FRichCurve* Curve = It.Value();
			FName ClassFName = FName(*ClassName);
			FAttributeSetDefaultsCurveCollection& DefaultCollection = Defaults.FindOrAdd(ClassFName);
			FAttributeDefaultCurveList* DefaultDataList = DefaultCollection.DataMap.Find(Set);
			if (DefaultDataList == nullptr)
			{
				ABILITY_LOG(Verbose, TEXT("Initializing new default Set for %s. PropertySize: %d.. DefaultSize %d"), Set->GetPropertiesSize(), UAttributeSet::StaticClass()->GetPropertiesSize());

				DefaultDataList = &DefaultCollection.DataMap.Add(Set);
			}

			check(DefaultDataList);
			DefaultDataList->AddPair(Property, Curve);
		}
	}
}

void FAttributeSetInitterCurveEval::InitAttributeSetDefaults(UAbilitySystemComponent* AbilitySystemComponent, FName GroupName, int32 Level, bool bInitialInit) const
{
	check(AbilitySystemComponent != nullptr)

		const FAttributeSetDefaultsCurveCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Warning, TEXT("Unable to find DefaultAttributeSet Group %s. Failing back to Defaults"), *GroupName.ToString());
		Collection = Defaults.Find(FName(TEXT("Default")));
		if (!Collection)
		{
			ABILITY_LOG(Error, TEXT("*FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
			return;
		}
	}

	for (const UAttributeSet* Set : AbilitySystemComponent->SpawnedAttributes)
	{
		const FAttributeDefaultCurveList* DefaultDataList = Collection->DataMap.Find(Set->GetClass());
		if (DefaultDataList)
		{
			ABILITY_LOG(Log, TEXT("*"));
			for (auto& DataPair : DefaultDataList->List)
			{
				check(DataPair.Property);

				if (Set->ShouldInitProperty(bInitialInit, DataPair.Property))
				{
					FGameplayAttribute AttributeToModify(DataPair.Property);
					AbilitySystemComponent->SetNumericAttributeBase(AttributeToModify, DataPair.Curve->Eval(Level));
				}
			}
		}
	}

	AbilitySystemComponent->ForceReplication();
}

void FAttributeSetInitterCurveEval::ApplyAttributeDefault(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute& InAttribute, FName GroupName, int32 Level) const
{
	check(AbilitySystemComponent != nullptr)

		const FAttributeSetDefaultsCurveCollection* Collection = Defaults.Find(GroupName);
	if (!Collection)
	{
		ABILITY_LOG(Warning, TEXT("Unable to find DefaultAttributeSet Group %s. Failing back to Defaults"), *GroupName.ToString());
		Collection = Defaults.Find(FName(TEXT("Default")));
		if (!Collection)
		{
			ABILITY_LOG(Error, TEXT("*FAttributeSetInitterDiscreteLevels::InitAttributeSetDefaults Default DefaultAttributeSet not found! Skipping Initialization"));
			return;
		}
	}

	for (const UAttributeSet* Set : AbilitySystemComponent->SpawnedAttributes)
	{
		const FAttributeDefaultCurveList* DefaultDataList = Collection->DataMap.Find(Set->GetClass());
		if (DefaultDataList)
		{
			ABILITY_LOG(Log, TEXT("*"));
			for (auto& DataPair : DefaultDataList->List)
			{
				check(DataPair.Property);

				if (DataPair.Property == InAttribute.GetUProperty())
				{
					FGameplayAttribute AttributeToModify(DataPair.Property);
					AbilitySystemComponent->SetNumericAttributeBase(AttributeToModify, DataPair.Curve->Eval(Level));
				}
			}
		}
	}

	AbilitySystemComponent->ForceReplication();
}

bool FAttributeSetInitterCurveEval::IsSupportedProperty(UProperty* Property) const
{
	return (Property && (Cast<UNumericProperty>(Property) || FGameplayAttribute::IsGameplayAttributeDataProperty(Property)));
}
*/