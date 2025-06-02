// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "SComponentClassCombo.h" // EComponentCreateAction::Type
#include "GenericGraphNodeComponent.h"

class UGenericGraphNodeComponent;

struct FSubobjectDataHandle;
typedef TSharedPtr<class FGenericNodeComponentClassComboEntry> FGenericNodeComponentClassComboEntryPtr;
class FGenericNodeComponentClassComboEntry : public TSharedFromThis<FGenericNodeComponentClassComboEntry>
{
public:
	FGenericNodeComponentClassComboEntry(const FString& InHeadingText, TSubclassOf<UActorComponent> InComponentClass, bool InIncludedInFilter, EComponentCreateAction::Type InComponentCreateAction, FComponentEntryCustomizationArgs InCustomizationArgs = FComponentEntryCustomizationArgs())
		: ComponentClass(InComponentClass)
		, IconClass(InComponentClass)
		, ComponentName()
		, ComponentPath()
		, HeadingText(InHeadingText)
		, bIncludedInFilter(InIncludedInFilter)
		, ComponentCreateAction(InComponentCreateAction)
		, CustomizationArgs(InCustomizationArgs)
	{}

	FGenericNodeComponentClassComboEntry(const FString& InHeadingText, const FString& InComponentName, FTopLevelAssetPath InComponentPath, const UClass* InIconClass, bool InIncludedInFilter)
		: ComponentClass(nullptr)
		, IconClass(InIconClass)
		, ComponentName(InComponentName)
		, ComponentPath(InComponentPath)
		, HeadingText(InHeadingText)
		, bIncludedInFilter(InIncludedInFilter)
		, ComponentCreateAction(EComponentCreateAction::SpawnExistingClass)
	{}

	FGenericNodeComponentClassComboEntry(const FString& InHeadingText)
		: ComponentClass(nullptr)
		, IconClass(nullptr)
		, ComponentName()
		, ComponentPath()
		, HeadingText(InHeadingText)
		, bIncludedInFilter(false)
	{}

	FGenericNodeComponentClassComboEntry()
		: ComponentClass(nullptr)
		, IconClass(nullptr)
	{}


	TSubclassOf<UGenericGraphNodeComponent> GetComponentClass() const { return ComponentClass; }

	const UClass* GetIconClass() const { return IconClass; }
	const FString& GetHeadingText() const { return HeadingText; }

	bool IsHeading() const
	{
		return ((ComponentClass == NULL && ComponentName == FString()) && !HeadingText.IsEmpty());
	}
	bool IsSeparator() const
	{
		return ((ComponentClass == NULL && ComponentName == FString()) && HeadingText.IsEmpty());
	}
	bool IsClass() const
	{
		return (ComponentClass != NULL || ComponentName != FString());
	}
	bool IsIncludedInFilter() const
	{
		return bIncludedInFilter;
	}

	const FString& GetComponentNameOverride() const
	{
		return CustomizationArgs.ComponentNameOverride;
	}
	EComponentCreateAction::Type GetComponentCreateAction() const
	{
		return ComponentCreateAction;
	}
	FOnSubobjectCreated& GetOnSubobjectCreated()
	{
		return CustomizationArgs.OnSubobjectCreated;
	}
	FString GetClassDisplayName() const;
	FString GetClassName() const;
	FString GetComponentPath() const { return ComponentPath.ToString(); }
	UObject* GetAssetOverride()
	{
		return CustomizationArgs.AssetOverride.Get();
	}
	FName GetIconOverrideBrushName() const { return CustomizationArgs.IconOverrideBrushName; }
	int32 GetSortPriority() const { return CustomizationArgs.SortPriority; }

	void AddReferencedObjects(FReferenceCollector& Collector);
	bool OnBlueprintGeneratedClassUnloaded(UBlueprintGeneratedClass* BlueprintGeneratedClass);

	// If the component type is not loaded, this stores data that can be used for class filtering.
	TSharedPtr<IUnloadedBlueprintData> GetUnloadedBlueprintData() const { return UnloadedBlueprintData; }

	// Can optionally be called to set unloaded component type data to assist with class filtering.
	void SetUnloadedBlueprintData(TSharedPtr<IUnloadedBlueprintData> InUnloadedBlueprintData)
	{
		UnloadedBlueprintData = InUnloadedBlueprintData;
	}

private:
	TSubclassOf<UGenericGraphNodeComponent> ComponentClass;
	TObjectPtr<const UClass> IconClass;
	// For components that are not loaded we just keep the name of the component,
	// loading occurs when the blueprint is spawned, which should also trigger a refresh
	// of the component list:
	FString ComponentName;
	FTopLevelAssetPath ComponentPath;
	FString HeadingText;
	bool bIncludedInFilter;
	EComponentCreateAction::Type ComponentCreateAction;
	FComponentEntryCustomizationArgs CustomizationArgs;
	TSharedPtr<IUnloadedBlueprintData> UnloadedBlueprintData;
};

DECLARE_DELEGATE_RetVal_ThreeParams(UGenericGraphNodeComponent*, FGenericNodeComponentClassSelected, TSubclassOf<UGenericGraphNodeComponent>, EComponentCreateAction::Type, UObject*);
DECLARE_DELEGATE_RetVal_ThreeParams(FSubobjectDataHandle, FGenericNodeSubobjectClassSelected, TSubclassOf<UGenericGraphNodeComponent>, EComponentCreateAction::Type, UObject*);

/**
 * 
 */
class GENERICGRAPHEDITOR_API SGenericNodeComponentClassCombo : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGenericNodeComponentClassCombo)
		: _IncludeText(true)
		{}

		SLATE_ATTRIBUTE(bool, IncludeText)
		SLATE_EVENT(FGenericNodeComponentClassSelected, OnComponentClassSelected)
		SLATE_EVENT(FGenericNodeSubobjectClassSelected, OnSubobjectClassSelected)
		SLATE_ARGUMENT(TArray<TSharedRef<IClassViewerFilter>>, CustomClassFilters)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	~SGenericNodeComponentClassCombo();

	void GenerateFilteredComponentList();

	void UpdateComponentClassList();

	void OnAddComponentSelectionChanged(FGenericNodeComponentClassComboEntryPtr InItem, ESelectInfo::Type SelectInfo);

	TSharedRef<ITableRow> GenerateAddComponentRow(FGenericNodeComponentClassComboEntryPtr Entry, const TSharedRef<STableViewBase>& OwnerTable) const;

	void OnSearchBoxTextChanged(const FText& InSearchText);

	void OnSearchBoxTextCommitted(const FText& NewText, ETextCommit::Type CommitInfo);

	void ClearSelection();

	void ToggleFilterOption(TSharedRef<FClassViewerFilterOption> FilterOption);

	bool IsFilterOptionEnabled(TSharedRef<FClassViewerFilterOption> FilterOption) const;
	bool IsComponentClassAllowed(FGenericNodeComponentClassComboEntryPtr Entry) const;

	TSharedRef<SToolTip> GetComponentToolTip(FGenericNodeComponentClassComboEntryPtr Entry) const;
	FString GetSanitizedComponentName(FGenericNodeComponentClassComboEntryPtr Entry) const;
	FText GetCurrentSearchString() const;
	FText GetFriendlyComponentName(FGenericNodeComponentClassComboEntryPtr Entry) const;
	void GetComponentClassFilterOptions(TArray<TSharedRef<FClassViewerFilterOption>>& OutFilterOptions) const;
	EVisibility GetFilterOptionsButtonVisibility() const;
	TSharedRef<SWidget> GetFilterOptionsMenuContent();

	TSharedPtr<SListView<FGenericNodeComponentClassComboEntryPtr>> ComponentClassListView;

	TSharedPtr<class SPositiveActionButton> AddNewButton;

	TSharedPtr<class SSearchBox> SearchBox;

private:
	/** Internal data used to facilitate component class filtering */
	struct FComponentClassFilterData
	{
		TSharedPtr<FClassViewerInitializationOptions> InitOptions;
		TSharedPtr<IClassViewerFilter> ClassFilter;
		TSharedPtr<FClassViewerFilterFuncs> FilterFuncs;
	};

	FGenericNodeComponentClassSelected OnComponentClassSelected;

	FGenericNodeSubobjectClassSelected OnSubobjectClassSelected;

	/** List of component class names used by combo box */
	TArray<FGenericNodeComponentClassComboEntryPtr>* ComponentClassList;

	/** List of component class names, filtered by the current search string */
	TArray<FGenericNodeComponentClassComboEntryPtr> FilteredComponentClassList;

	/** The current search string */
	TSharedPtr<FTextFilterExpressionEvaluator> TextFilter;

	/** Internal data that facilitates custom class filtering */
	FComponentClassFilterData ComponentClassFilterData;

	/** Cached selection index used to skip over unselectable items */
	int32 PrevSelectedIndex;
};
