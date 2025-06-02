// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SSubobjectEditor.h" // class FSubobjectEditorTreeNodePtrType;

class UGenericGraphNodeComponent;

/**
 * 
 */
class GENERICGRAPHEDITOR_API SSubobjectGenericGraphNodeEditor final : public SSubobjectEditor
{
public:
	DECLARE_DELEGATE_OneParam(FOnHighlightPropertyInDetailsView, const class FPropertyPath&);

private:
	SLATE_BEGIN_ARGS(SSubobjectGenericGraphNodeEditor)
		: _ObjectContext(nullptr)
		, _AllowEditing(true)
		, _HideComponentClassCombo(false)
		, _OnSelectionUpdated()
		{}

		SLATE_ATTRIBUTE(UObject*, ObjectContext)
		SLATE_ATTRIBUTE(bool, AllowEditing)
		SLATE_ATTRIBUTE(bool, HideComponentClassCombo)
		SLATE_EVENT(FOnSelectionUpdated, OnSelectionUpdated)
		SLATE_EVENT(FOnItemDoubleClicked, OnItemDoubleClicked)
		SLATE_ARGUMENT(TArray<TSharedRef<IClassViewerFilter>>, SubobjectClassListFilters)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	/**
	 * Spawns a new SWindow giving the user options for creating a new C++ component class.
	 * The user will be prompted to pick a new subclass name and code will be recompiled
	 *
	 * @return The new class that was created
	 */
	UClass* SpawnCreateNewCppComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass);
	UClass* SpawnCreateNewBPComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass);

	/** Add a component from the selection in the combo box */
	FSubobjectDataHandle PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride);
	static void BuildMenuEventsSection(FMenuBuilder& Menu, UBlueprint* Blueprint, UClass* SelectedClass, FCanExecuteAction CanExecuteActionDelegate, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate);
	static void CreateEventsForSelection(UBlueprint* Blueprint, FName EventName, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate);
	static void ConstructEvent(UBlueprint* Blueprint, const FName EventName, const FComponentEventConstructionData EventData);
	static void ViewEvent(UBlueprint* Blueprint, const FName EventName, const FComponentEventConstructionData EventData);

	// SSubobjectEditor interface
	virtual bool ShouldModifyBPOnAssetDrop() const override { return true; }
	virtual void OnDeleteNodes() override;
	virtual void CopySelectedNodes() override;
	virtual void OnDuplicateComponent() override;
	virtual bool CanPasteNodes() const override;
	virtual void PasteNodes() override;

	virtual void OnAttachToDropAction(FSubobjectEditorTreeNodePtrType DroppedOn, const TArray<FSubobjectEditorTreeNodePtrType>& DroppedNodePtrs);
	virtual void OnDetachFromDropAction(const TArray<FSubobjectEditorTreeNodePtrType>& DroppedNodePtrs);
	virtual void OnMakeNewRootDropAction(FSubobjectEditorTreeNodePtrType DroppedNodePtr) { ; };
	virtual void PostDragDropAction(bool bRegenerateTreeNodes);

	/** Builds a context menu pop up for dropping a child node onto the scene root node */
	virtual TSharedPtr<SWidget> BuildSceneRootDropActionMenu(FSubobjectEditorTreeNodePtrType DroppedOntoNodePtr, FSubobjectEditorTreeNodePtrType DroppedNodePtr) { return SNew(SBox); };
	virtual bool ClearSelectionOnClick() const override { return true; }
	virtual bool ShowInlineSearchWithButtons() const override { return true; }
	virtual FSubobjectDataHandle AddNewSubobject(const FSubobjectDataHandle& ParentHandle, UClass* NewClass, UObject* AssetOverride, FText& OutFailReason, TUniquePtr<FScopedTransaction> InOngoingTransaction) override;
	virtual void PopulateContextMenuImpl(UToolMenu* InMenu, TArray<FSubobjectEditorTreeNodePtrType>& InSelectedItems, bool bIsChildActorSubtreeNodeSelected) override;
	// virtual FSubobjectEditorTreeNodePtrType GetSceneRootNode() const override;
public:
	virtual FSubobjectEditorTreeNodePtrType FindSlateNodeForObject(const UObject* InObject, bool bIncludeAttachmentComponents = true) const override;
	// End of SSubobjectEditor

public:
	void UpdateGenericTree(bool bRegenerateTreeNodes = true);
};
