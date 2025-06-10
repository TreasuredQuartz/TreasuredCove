// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericSubobjectEditor/GenericSubobjectDataHandle.h"
#include "SComponentClassCombo.h"

class UGenericGraphNode;
class UGenericGraphNodeComponent;

/** SCSEditor UI customization */
class IGenericEditorUICustomization
{
public:
	virtual ~IGenericEditorUICustomization() {}

	/** @return Whether to hide the components tree */
	virtual bool HideComponentsTree() const { return false; }

	/** @return Whether to hide the components filter box */
	virtual bool HideComponentsFilterBox() const { return false; }

	/** @return Whether to hide the "Add Component" combo button */
	virtual bool HideAddComponentButton() const { return false; }

	/** @return Whether to hide the "Edit Blueprint" and "Blueprint/Add Script" buttons */
	virtual bool HideBlueprintButtons() const { return false; }

	/** @return A component type that limits visible nodes when filtering the tree view */
	virtual TSubclassOf<UObject> GetTypeFilter() const { return nullptr; }
};

// Tree node pointer types
using FGenericComponentListNodePtrType = TSharedPtr<class FGenericComponentListNode>;

/**
 * Wrapper struct that represents access an single subobject that is used
 * by slate as a layout for columns/rows should look like
 */
class GENERICGRAPHEDITOR_API FGenericComponentListNode : public TSharedFromThis<FGenericComponentListNode>
{
public:
	/** Delegate for when the context menu requests a rename */
	DECLARE_DELEGATE(FOnRenameRequested);

	explicit FGenericComponentListNode(const FGenericSubobjectDataHandle& SubobjectDataSource, bool InbIsSeperator = false);
	~FGenericComponentListNode() = default;

	bool IsSeperator() const { return bIsSeperator; }

	FGenericSubobjectDataHandle GetDataHandle() { return DataHandle; }

	FGenericSubobjectData* GetDataSource() const { return DataHandle.GetSharedDataPtr().Get(); }

	/** Get the display name of this single list node */
	FString GetDisplayString() const;

	// Returns true if this subobject data handle is valid
	bool IsValid() const { return bIsSeperator || DataHandle.IsValid(); }

	bool MatchesFilterType(const UClass* InFilterType) const;

	/** Query that determines if this item should be filtered out or not */
	bool IsFlaggedForFiltration() const;

	/** Refreshes this item's filtration state. Set bRecursive to 'true' to refresh any child nodes as well */
	bool RefreshFilteredState(const UClass* InFilterType, const TArray<FString>& InFilterTerms);

	void SetCachedFilterState(bool bMatchesFilter);

	void SetOngoingCreateTransaction(TUniquePtr<FScopedTransaction> InTransaction);
	void CloseOngoingCreateTransaction();
	void GetOngoingCreateTransaction(TUniquePtr<FScopedTransaction>& OutPtr) { OutPtr = MoveTemp(OngoingCreateTransaction); }
	bool HasOngoingTransaction() const { return OngoingCreateTransaction.IsValid(); }

	/** Sets up the delegate for a rename operation */
	void SetRenameRequestedDelegate(FOnRenameRequested InRenameRequested) { RenameRequestedDelegate = InRenameRequested; }
	FOnRenameRequested GetRenameRequestedDelegate() { return RenameRequestedDelegate; }

	bool CanDelete() const;

	bool CanRename() const;

private:
	/** The Object this node is related to. */
	FGenericSubobjectDataHandle DataHandle;
	// Scope the creation of a node which ends when the initial 'name' is given/accepted by the user, which can be several frames after the node was actually created.
	TUniquePtr<FScopedTransaction> OngoingCreateTransaction;
	/** Handles rename requests */
	FOnRenameRequested RenameRequestedDelegate;
	/** Currently focused graph editor */
	TWeakPtr<class SGraphEditor> FocusedGraphEdPtr;

	enum EFilteredState
	{
		FilteredOut = 0x00,
		MatchesFilter = (1 << 0),
		ChildMatches = (1 << 1),

		FilteredInMask = (MatchesFilter | ChildMatches),
		Unknown = 0xFC // ~FilteredInMask
	};
	uint8 FilterFlags;

	/** A flag that indicates that this is a separator slate node and has no valid data */
	uint8 bIsSeperator : 1;
};

class GENERICGRAPHEDITOR_API SGenericComponentList : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_OneParam(FOnSelectionUpdated, const TArray<FGenericComponentListNodePtrType>&);

public:
	SLATE_BEGIN_ARGS(SGenericComponentList)
		: _HideComponentClassCombo(false) 
		{}

		SLATE_ATTRIBUTE(bool, HideComponentClassCombo)
		SLATE_EVENT(FOnSelectionUpdated, OnSelectionUpdated)
	SLATE_END_ARGS()

	////////////////// Slate Attributes //////////////////
private:
	/** Delegate to invoke on selection update. */
	FOnSelectionUpdated OnSelectionUpdated;

	/** Attribute to indicate whether or not editing is allowed. */
	TAttribute<bool> AllowEditing;

	/** Attribute to indicate whether or not the "Add Component" button is visible. If true, new components cannot be added to the Blueprint. */
	TAttribute<bool> HideComponentClassCombo;

	////////////////// Set-up //////////////////
public:
	void Construct(const FArguments& InArgs, UGenericGraphNode* InNode);

	void ConstructTreeWidget();

	void ConstructSearchBoxWidget();

	void ConstructHeaderBoxWidget();

	void UpdateTree(bool bRegenerateNodes = true);

	/** Dumps out the tree view contents to the log (used to assist with debugging widget hierarchy issues) */
	void DumpTree();

	/** Forces the details panel to refresh on the same objects */
	void RefreshSelectionDetails();

public:
	FGenericSubobjectDataHandle PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride);

	TSharedRef<ITableRow> GenerateListRow(FGenericComponentListNodePtrType InNodePtr, const TSharedRef<STableViewBase>& OwnerTable);

	void OnTreeSelectionChanged(FGenericComponentListNodePtrType InNodePtr, ESelectInfo::Type SelectInfo);

	void UpdateSelectionFromNodes(const TArray<FGenericComponentListNodePtrType>& SelectedNodes);

	////////////////// Subobjects //////////////////
public:
	FGenericSubobjectDataHandle AddNewSubobject(UClass* NewClass, UObject* Asset, FText& FailReason);

	static FGenericComponentListNodePtrType FindOrCreateSlateNodeForHandle(const FGenericSubobjectDataHandle& InHandle, TMap<FGenericSubobjectDataHandle, FGenericComponentListNodePtrType>& ExistingNodes);

	FGenericComponentListNodePtrType FindSlateNodeForHandle(const FGenericSubobjectDataHandle& InHandleToFind, FGenericComponentListNodePtrType InStartNodePtr = FGenericComponentListNodePtrType()) const;

	FGenericComponentListNodePtrType FindSlateNodeForVariableName(FName InVariableName) const;

	////////////////// Filters //////////////////
public:
	/** Recursively updates the filtered state for each component item */
	void OnFilterTextChanged(const FText& InFilterText);

	bool RefreshFilteredState(FGenericComponentListNodePtrType ListNode);

	TSubclassOf<UObject> GetComponentTypeFilterToApply() const;

	EVisibility GetComponentClassComboButtonVisibility() const;

	EVisibility GetComponentsFilterBoxVisibility() const;

	EVisibility GetComponentsTreeVisibility() const;

	FText GetFilterText() const;

	////////////////// Commands //////////////////
public:
	/** Select the given tree node */
	void SelectNode(FGenericComponentListNodePtrType InNodeToSelect, bool bIsCntrlDown);

	/** Clears the current selection */
	void ClearSelection();

	bool IsEditingAllowed() const;

	TArray<FGenericComponentListNodePtrType> GetSelectedNodes() const;

	TArray<FGenericSubobjectDataHandle> GetSelectedHandles() const;

	void RestoreSelectionState(const TArray<FGenericComponentListNodePtrType>& SelectedListNodes, bool bFallBackToVariableName = true);

	/** Creates a list of commands */
	void CreateCommandList();

	TSharedPtr<FUICommandList> GetCommandList() const { return CommandList; }

	void OnFindReferences(bool bSearchAllBlueprints, const EGetFindReferenceSearchStringFlags Flags);

	bool CanCutNodes() const;
	void CutSelectedNodes();

	bool CanCopyNodes() const;
	virtual void CopySelectedNodes();

	/** Pastes previously copied node(s) */
	virtual bool CanPasteNodes() const;
	virtual void PasteNodes();

public:
	/** Removes existing selected component nodes from the SCS */
	virtual bool CanDeleteNodes() const;
	virtual void OnDeleteNodes();

protected:
	bool CanDuplicateComponent() const;
	virtual void OnDuplicateComponent();

	/** Checks to see if renaming is allowed on the selected component */
	bool CanRenameComponent() const;
	void OnRenameComponent();
	void OnRenameComponent(TUniquePtr<FScopedTransaction> InComponentCreateTransaction);

	/** Called at the end of each frame. */
	void OnPostTick(float);

	////////////////// Private Vars. //////////////////
private:
	/** The Object with components that we are editing. */
	UGenericGraphNode* GenericGraphNode;
	/** Command list for handling actions in the editor. */
	TSharedPtr<FUICommandList> CommandList;
	/** SCSEditor UI customizations */
	TSharedPtr<IGenericEditorUICustomization> UICustomization;
	/** Name of a node that has been requested to be renamed */
	FGenericSubobjectDataHandle DeferredRenameRequest;
	/** Scope the creation of a component which ends when the initial component 'name' is given/accepted by the user, which can be several frames after the component was actually created. */
	TUniquePtr<FScopedTransaction> DeferredOngoingCreateTransaction;
	/** Used to unregister from the post tick event. */
	FDelegateHandle PostTickHandle;
	/** */
	bool bUpdatingSelection;

	////////////////// Widgets //////////////////

	/** The filter box that handles filtering for the tree. */
	TSharedPtr<SSearchBox> FilterBox; 
	/** The header box that contains the search bar and add component button. */
	TSharedPtr<SVerticalBox> HeaderBox;
	/** The physical items array source for our ListWidget. */
	TArray<FGenericComponentListNodePtrType> NodeList;
	/** The widget displaying our component list. */
	TSharedPtr<SListView<FGenericComponentListNodePtrType>> ListWidget;
};

class SGenericComponent_RowWidget : public SMultiColumnTableRow<FGenericComponentListNodePtrType>
{
public:
	SLATE_BEGIN_ARGS(SGenericComponent_RowWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<SGenericComponentList> InEditor, FGenericComponentListNodePtrType InNodePtr, TSharedPtr<STableViewBase> InOwnerTableView);

	// SMultiColumnTableRow<T> interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

protected:
	/** Drag-drop handlers */
	void HandleOnDragEnter(const FDragDropEvent& DragDropEvent);
	void HandleOnDragLeave(const FDragDropEvent& DragDropEvent);
	FReply HandleOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	TOptional<EItemDropZone> HandleOnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericComponentListNodePtrType TargetItem);
	FReply HandleOnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericComponentListNodePtrType TargetItem);
	/** End Drag-drop handlers */

public:
	/** Get the asset name of this subobject from the asset brokerage */
	FText GetAssetName() const;

	/** Get the asset path of this subobject from the asset brokerage */
	FText GetAssetPath() const;

	FText GetNameLabel() const;

	/** Check if this asset is visible from the asset brokerage */
	EVisibility GetAssetVisibility() const;

	virtual const FSlateBrush* GetIconBrush() const;

	FSlateColor GetColorTintForIcon() const;

	FGenericComponentListNodePtrType GetComponentPtr() const { return ComponentPtr; }

private:
	/** Creates a tooltip for this row */
	TSharedRef<SToolTip> CreateToolTipWidget() const;

	/** Create the tooltip for component subobjects */
	TSharedRef<SToolTip> CreateComponentTooltipWidget(const FGenericComponentListNodePtrType& InNode) const;

	/** Create the tooltip for actor subobjects */
	TSharedRef<SToolTip> CreateActorTooltipWidget(const FGenericComponentListNodePtrType& InNode) const;

	FText GetTooltipText() const;

	/** Gets the context of this subobject, such as "(Self)" or "(Instance)" for actors. */
	FText GetObjectContextText() const;

	// DOCUMENTATION //
	FString GetDocumentationLink() const;
	FString GetDocumentationExcerptName() const;
	// DOCUMENTATION //

	/** Commits the new name of the component */
	void OnNameTextCommit(const FText& InNewName, ETextCommit::Type InTextCommit);

	/** Verifies the name of the component when changing it */
	bool OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage);

	bool IsReadOnly() const;

private:
	TWeakPtr<SGenericComponentList> ComponentEditor;
	FGenericComponentListNodePtrType ComponentPtr;

	TSharedPtr<SInlineEditableTextBlock> InlineWidget;
};
