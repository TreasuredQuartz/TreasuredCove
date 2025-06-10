// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SSubobjectEditor.h" // class FGenericSubobjectEditorTreeNodePtrType;
#include "GenericSubobjectDataHandle.h"
#include "Widgets/SCompoundWidget.h"

class UGenericGraphNodeComponent;
struct FGenericSubobjectData;
class SSubobjectEditor; // For easy access peeking
class SGenericSubobjectEditor;

// Tree node pointer types
using FGenericSubobjectEditorTreeNodePtrType = TSharedPtr<class FGenericSubobjectEditorTreeNode>;
/////////////////////////////////////////////////////
// FSubobjectEditorTreeNode

/**
 * Wrapper struct that represents access an single subobject that is used
 * by slate as a layout for columns/rows should look like
 */
class GENERICGRAPHEDITOR_API FGenericSubobjectEditorTreeNode : public TSharedFromThis<FGenericSubobjectEditorTreeNode>
{
public:

	/** Delegate for when the context menu requests a rename */
	DECLARE_DELEGATE(FOnRenameRequested);

	explicit FGenericSubobjectEditorTreeNode(const FGenericSubobjectDataHandle& DataSource, bool InbIsSeperator = false);

	~FGenericSubobjectEditorTreeNode() = default;

	bool IsSeperator() const { return bIsSeperator; }

	const UObject* GetObject(bool bEvenIfPendingKill = false) const;

	// Get a pointer to the subobject data that this slate node is representing
	FGenericSubobjectData* GetDataSource() const;

	// Get the subobject handle that this slate node is representing
	FGenericSubobjectDataHandle GetDataHandle() const { return DataHandle; }

	/** Get the slate parent of this single tree node */
	FGenericSubobjectEditorTreeNodePtrType GetParent() const { return ParentNodePtr; }

	/** Get the display name of this single tree node */
	FString GetDisplayString() const;

	bool IsComponentNode() const;

	// Returns true if this is the root actor node of the tree
	bool IsRootActorNode() const;

	bool CanReparent() const;

	FName GetVariableName() const;

	// Returns true if this subobject data handle is valid
	bool IsValid() const { return bIsSeperator || DataHandle.IsValid(); }

	bool IsChildSubtreeNode() const;

	bool IsNativeComponent() const;

	// Returns true if this node is attached to the given slate node
	bool IsAttachedTo(FGenericSubobjectEditorTreeNodePtrType InNodePtr) const;

	/**
	* @return Whether or not this node is a direct child of the given node.
	*/
	bool IsDirectlyAttachedTo(FGenericSubobjectEditorTreeNodePtrType InNodePtr) const;

	bool CanDelete() const;

	bool CanRename() const;

	/**
	 * @return The set of nodes which are parented to this node (read-only).
	 */
	const TArray<FGenericSubobjectEditorTreeNodePtrType>& GetChildren() const { return Children; }

	// Add the given slate node to our child array and set it's parent to us
	// This had no effect on the actual structure of the subobjects this node represents
	// it is purely visual
	void AddChild(FGenericSubobjectEditorTreeNodePtrType AttachToPtr);

	// Remove the given slate node from our children array
	void RemoveChild(FGenericSubobjectEditorTreeNodePtrType InChildNodePtr);

	// Attempts to find the given subobject handle in the slate children on this node. Nullptr if none are found.
	FGenericSubobjectEditorTreeNodePtrType FindChild(const FGenericSubobjectDataHandle& InHandle);

	/** Query that determines if this item should be filtered out or not */
	bool IsFlaggedForFiltration() const;

	/** Sets this item's filtration state. Use bUpdateParent to make sure the parent's EFilteredState::ChildMatches flag is properly updated based off the new state */
	void SetCachedFilterState(bool bMatchesFilter, bool bUpdateParent);

	/** Used to update the EFilteredState::ChildMatches flag for parent nodes, when this item's filtration state has changed */
	void ApplyFilteredStateToParent();

	/** Updates the EFilteredState::ChildMatches flag, based off of children's current state */
	void RefreshCachedChildFilterState(bool bUpdateParent);

	/** Refreshes this item's filtration state. Set bRecursive to 'true' to refresh any child nodes as well */
	bool RefreshFilteredState(const UClass* InFilterType, const TArray<FString>& InFilterTerms, bool bRecursive);

	/** Returns whether the node will match the given type (for filtering) */
	bool MatchesFilterType(const UClass* InFilterType) const;

	void SetOngoingCreateTransaction(TUniquePtr<FScopedTransaction> InTransaction);
	void CloseOngoingCreateTransaction();
	void GetOngoingCreateTransaction(TUniquePtr<FScopedTransaction>& OutPtr) { OutPtr = MoveTemp(OngoingCreateTransaction); }
	bool HasOngoingTransaction() const { return OngoingCreateTransaction.IsValid(); }

	/** Sets up the delegate for a rename operation */
	void SetRenameRequestedDelegate(FOnRenameRequested InRenameRequested) { RenameRequestedDelegate = InRenameRequested; }
	FOnRenameRequested GetRenameRequestedDelegate() { return RenameRequestedDelegate; }

protected:

	/** Pointer to the parent of this subobject */
	FGenericSubobjectEditorTreeNodePtrType ParentNodePtr;

	// Scope the creation of a node which ends when the initial 'name' is given/accepted by the user, which can be several frames after the node was actually created.
	TUniquePtr<FScopedTransaction> OngoingCreateTransaction;

	/** Handles rename requests */
	FOnRenameRequested RenameRequestedDelegate;

	/**
	* Any children that this subobject has in the hierarchy, used to
	* collapsed things within in the tree
	*/
	TArray<FGenericSubobjectEditorTreeNodePtrType> Children;

	/** The data source of this subobject */
	FGenericSubobjectDataHandle DataHandle;

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

/////////////////////////////////////////////////////
// SSubobjectEditorDragDropTree

/** Implements the specific node type and add drag/drop functionality */
class SGenericSubobjectEditorDragDropTree : public STreeView<FGenericSubobjectEditorTreeNodePtrType>
{
public:
	SLATE_BEGIN_ARGS(SGenericSubobjectEditorDragDropTree)
		: _SubobjectEditor(nullptr)
		, _OnGenerateRow()
		, _OnGetChildren()
		, _OnSetExpansionRecursive()
		, _TreeItemsSource(static_cast<TArray<FGenericSubobjectEditorTreeNodePtrType>*>(nullptr))
		, _ItemHeight(16)
		, _OnContextMenuOpening()
		, _OnMouseButtonDoubleClick()
		, _OnSelectionChanged()
		, _SelectionMode(ESelectionMode::Multi)
		, _ClearSelectionOnClick(true)
		, _ExternalScrollbar()
		, _OnTableViewBadState()
		{
			_Clipping = EWidgetClipping::ClipToBounds;
		}

		SLATE_ARGUMENT(SGenericSubobjectEditor*, SubobjectEditor)

		SLATE_EVENT(FOnGenerateRow, OnGenerateRow)

		SLATE_EVENT(FOnItemScrolledIntoView, OnItemScrolledIntoView)

		SLATE_EVENT(FOnGetChildren, OnGetChildren)

		SLATE_EVENT(FOnSetExpansionRecursive, OnSetExpansionRecursive)

		SLATE_ARGUMENT(TArray<FGenericSubobjectEditorTreeNodePtrType>*, TreeItemsSource)

		SLATE_ATTRIBUTE(float, ItemHeight)

		SLATE_EVENT(FOnContextMenuOpening, OnContextMenuOpening)

		SLATE_EVENT(FOnMouseButtonDoubleClick, OnMouseButtonDoubleClick)

		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)

		SLATE_ATTRIBUTE(ESelectionMode::Type, SelectionMode)

		SLATE_ARGUMENT(TSharedPtr<SHeaderRow>, HeaderRow)

		SLATE_ARGUMENT(bool, ClearSelectionOnClick)

		SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)

		SLATE_EVENT(FOnTableViewBadState, OnTableViewBadState)

	SLATE_END_ARGS()

	/** Object construction - mostly defers to the base STreeView */
	void Construct(const FArguments& InArgs);

	// SWidget interface
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	// End SWidget interface

private:

	SGenericSubobjectEditor* SubobjectEditor;
};

class SGenericSubobject_RowWidget : public SMultiColumnTableRow<FGenericSubobjectEditorTreeNodePtrType>
{
public:
	SLATE_BEGIN_ARGS(SGenericSubobject_RowWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<SGenericSubobjectEditor> InEditor, FGenericSubobjectEditorTreeNodePtrType InNodePtr, TSharedPtr<STableViewBase> InOwnerTableView);

	// SMultiColumnTableRow<T> interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

protected:
	virtual ESelectionMode::Type GetSelectionMode() const override;
	// End of SMultiColumnTableRow<T>

	/** Drag-drop handlers */
	void HandleOnDragEnter(const FDragDropEvent& DragDropEvent);
	void HandleOnDragLeave(const FDragDropEvent& DragDropEvent);
	FReply HandleOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	TOptional<EItemDropZone> HandleOnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericSubobjectEditorTreeNodePtrType TargetItem);
	FReply HandleOnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericSubobjectEditorTreeNodePtrType TargetItem);

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

	FGenericSubobjectEditorTreeNodePtrType GetSubobjectPtr() const { return SubobjectPtr; }

private:

	static void AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, TSharedRef<SWidget> ValueIcon, const TAttribute<FText>& Value, bool bImportant);

	/** Creates a tooltip for this row */
	TSharedRef<SToolTip> CreateToolTipWidget() const;

	/** Create the tooltip for component subobjects */
	TSharedRef<SToolTip> CreateComponentTooltipWidget(const FGenericSubobjectEditorTreeNodePtrType& InNode) const;

	/** Create the tooltip for actor subobjects */
	TSharedRef<SToolTip> CreateActorTooltipWidget(const FGenericSubobjectEditorTreeNodePtrType& InNode) const;

	FText GetTooltipText() const;

	FText GetActorClassNameText() const;
	FText GetActorSuperClassNameText() const;
	FText GetActorMobilityText() const;

	/** Returns a widget that represents the inheritance of this subobject which includes a hyperlink to edit the property */
	TSharedRef<SWidget> GetInheritedLinkWidget();

	/** Gets the context of this subobject, such as "(Self)" or "(Instance)" for actors. */
	FText GetObjectContextText() const;

	FString GetDocumentationLink() const;
	FString GetDocumentationExcerptName() const;

	/** Callback used when the user clicks on a blueprint inherited variable */
	void OnEditBlueprintClicked();

	EVisibility GetEditBlueprintVisibility() const;
	EVisibility GetEditNativeCppVisibility() const;

	/** Callback used when the user clicks on a native inherited variable */
	void OnEditNativeCppClicked();

	/**
	 * Retrieves tooltip text describing the specified component's mobility.
	 *
	 * @returns An FText object containing a description of the component's mobility
	 */
	FText GetMobilityToolTipText() const;

	/**
	 * Retrieves an image brush signifying the specified component's mobility (could sometimes be NULL).
	 *
	 * @returns A pointer to the FSlateBrush to use (NULL for Static and Non-SceneComponents)
	 */
	FSlateBrush const* GetMobilityIconImage() const;

	/**
	* Retrieves tooltip text describing where the component was first introduced (for inherited components).
	*
	* @returns An FText object containing a description of when the component was first introduced
	*/
	FText GetIntroducedInToolTipText() const;

	/**
	* Retrieves tooltip text describing how the component was introduced
	*
	* @returns An FText object containing a description of when the component was first introduced
	*/
	FText GetComponentAddSourceToolTipText() const;

	/**
	* Retrieves a tooltip text describing if the component is marked Editor only or not
	*
	* @returns An FText object containing a description of if the component is marked Editor only or not
	*/
	FText GetComponentEditorOnlyTooltipText() const;

	/**
	* Retrieves tooltip text for the specified Native Component's underlying Name
	*
	* @returns An FText object containing the Component's Name
	*/
	FText GetNativeComponentNameToolTipText() const;

	FText GetActorDisplayText() const;

	/** Commits the new name of the component */
	void OnNameTextCommit(const FText& InNewName, ETextCommit::Type InTextCommit);

	/** Verifies the name of the component when changing it */
	bool OnNameTextVerifyChanged(const FText& InNewText, FText& OutErrorMessage);

	bool IsReadOnly() const;

	TWeakPtr<SGenericSubobjectEditor> SubobjectEditor;
	TSharedPtr<SInlineEditableTextBlock> InlineWidget;

	FGenericSubobjectEditorTreeNodePtrType SubobjectPtr;
};

/**
 * 
 */
class GENERICGRAPHEDITOR_API SGenericSubobjectEditor final : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnHighlightPropertyInDetailsView, const class FPropertyPath&);
	DECLARE_DELEGATE_OneParam(FOnSelectionUpdated, const TArray<FGenericSubobjectEditorTreeNodePtrType>&);
	DECLARE_DELEGATE_OneParam(FOnItemDoubleClicked, const FGenericSubobjectEditorTreeNodePtrType);

private:
	SLATE_BEGIN_ARGS(SGenericSubobjectEditor)
		: _ObjectContext(nullptr)
		, _GraphContext(nullptr)
		, _AllowEditing(true)
		, _HideComponentClassCombo(false)
		, _OnSelectionUpdated()
		{}

		SLATE_ATTRIBUTE(UObject*, ObjectContext)
		SLATE_ATTRIBUTE(UEdGraph*, GraphContext)
		SLATE_ATTRIBUTE(bool, AllowEditing)
		SLATE_ATTRIBUTE(bool, HideComponentClassCombo)
		SLATE_EVENT(FOnSelectionUpdated, OnSelectionUpdated)
		SLATE_EVENT(FOnItemDoubleClicked, OnItemDoubleClicked)
		SLATE_ARGUMENT(TArray<TSharedRef<IClassViewerFilter>>, SubobjectClassListFilters)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	/** Delegate to invoke on selection update. */
	FOnSelectionUpdated OnSelectionUpdated;

	/** Delegate to invoke when an item in the tree is double clicked. */
	FOnItemDoubleClicked OnItemDoubleClicked;

	/** Attribute that provides access to the Object context for which we are viewing/editing. */
	TAttribute<UObject*> ObjectContext;

	/** Attribute that provides access to the Object context for which we are viewing/editing. */
	TAttribute<UEdGraph*> GraphContext;

	/** Attribute to indicate whether or not editing is allowed. */
	TAttribute<bool> AllowEditing;

	/** Attribute to indicate whether or not the "Add Component" button is visible. If true, new components cannot be added to the Blueprint. */
	TAttribute<bool> HideComponentClassCombo;

	/** Attribute to limit visible nodes to a particular component type when filtering the tree view. */
	TAttribute<TSubclassOf<UObject>> SubobjectTypeFilter;
	
	/////////////////////////////////////////////////////////
	// Widget Callbacks
protected:

	TSharedRef<ITableRow> MakeTableRowWidget(FGenericSubobjectEditorTreeNodePtrType InNodePtr, const TSharedRef<STableViewBase>& OwnerTable);

	/** @return The visibility of the components tree */
	EVisibility GetComponentsTreeVisibility() const;

	/** Used by tree control - get children for a specified subobject node */
	void OnGetChildrenForTree(FGenericSubobjectEditorTreeNodePtrType InNodePtr, TArray<FGenericSubobjectEditorTreeNodePtrType>& OutChildren);

	/** Update any associated selection (e.g. details view) from the passed in nodes */
	void UpdateSelectionFromNodes(const TArray<FGenericSubobjectEditorTreeNodePtrType>& SelectedNodes);

	/** Called when selection in the tree changes */
	void OnTreeSelectionChanged(FGenericSubobjectEditorTreeNodePtrType InSelectedNodePtr, ESelectInfo::Type SelectInfo);

	/** Callback when a component item is double clicked. */
	void HandleItemDoubleClicked(FGenericSubobjectEditorTreeNodePtrType InItem);

	void OnFindReferences(bool bSearchAllBlueprints, const EGetFindReferenceSearchStringFlags Flags);

	/** @return The visibility of the components filter box */
	EVisibility GetComponentsFilterBoxVisibility() const;

	/** Recursively updates the filtered state for each component item */
	void OnFilterTextChanged(const FText& InFilterText);

	/** Callback when a component item is scrolled into view */
	void OnItemScrolledIntoView(FGenericSubobjectEditorTreeNodePtrType InItem, const TSharedPtr<ITableRow>& InWidget);

	/** SWidget interface */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	void OnRenameComponent();

	void OnRenameComponent(TUniquePtr<FScopedTransaction> InComponentCreateTransaction);

	/////////////////////////////////////////////////////////
	// Widgets

	/** Root set of tree that represents all subobjects for the current context */
	TArray<FGenericSubobjectEditorTreeNodePtrType> RootNodes;

	/** Tree widget */
	TSharedPtr<SGenericSubobjectEditorDragDropTree> TreeWidget;

	/** Command list for handling actions in the editor. */
	TSharedPtr<FUICommandList> CommandList;

	/** The filter box that handles filtering for the tree. */
	TSharedPtr<SSearchBox> FilterBox;

	/** The tools buttons box */
	TSharedPtr<SHorizontalBox> ButtonBox;

	/** The add component button / type selector */
	TSharedPtr<SComponentClassCombo> ComponentClassCombo;

	/** Gate to prevent changing the selection while selection change is being broadcast. */
	bool bUpdatingSelection;

	/** Controls whether or not to allow calls to UpdateTree() */
	bool bAllowTreeUpdates;

	/** SCSEditor UI customizations */
	TSharedPtr<ISCSEditorUICustomization> UICustomization;

	/** SCSEditor UI extension */
	TSharedPtr<SExtensionPanel> ExtensionPanel;

	/** Scope the creation of a component which ends when the initial component 'name' is given/accepted by the user, which can be several frames after the component was actually created. */
	TUniquePtr<FScopedTransaction> DeferredOngoingCreateTransaction;

	/** Used to unregister from the post tick event. */
	FDelegateHandle PostTickHandle;

	/** Name of a node that has been requested to be renamed */
	FGenericSubobjectDataHandle DeferredRenameRequest;

	/**
	 * The handle to the current root context. If this is different in between
	 * UpdateTree calls, then we know the context has changed and we should clean up
	 * the subobject memory layout.
	 */
	FGenericSubobjectDataHandle CachedRootHandle;

	virtual FMenuBuilder CreateMenuBuilder();

	/** Constructs the slate drag/drop tree for this subobject editor */
	virtual void ConstructTreeWidget();

	/** Creates a list of commands */
	virtual void CreateCommandList();

	/** Recursively visits the given node + its children and invokes the given function for each. */
	void DepthFirstTraversal(const FGenericSubobjectEditorTreeNodePtrType& InNodePtr, TSet<FGenericSubobjectEditorTreeNodePtrType>& OutVisitedNodes, const TFunctionRef<void(const FGenericSubobjectEditorTreeNodePtrType&)> InFunction) const;

	/** Returns the set of expandable nodes that are currently collapsed in the UI */
	void GetCollapsedNodes(const FGenericSubobjectEditorTreeNodePtrType& InNodePtr, TSet<FGenericSubobjectEditorTreeNodePtrType>& OutCollapsedNodes) const;

	// Attempt to find an existing slate node that matches the given handle
	static FGenericSubobjectEditorTreeNodePtrType FindOrCreateSlateNodeForHandle(const FGenericSubobjectDataHandle& Handle, TMap<FGenericSubobjectDataHandle, FGenericSubobjectEditorTreeNodePtrType>& ExistingNodes);

	/** @return Type of component to filter the tree view with or nullptr if there's no filter. */
	TSubclassOf<UGenericGraphNodeComponent> GetComponentTypeFilterToApply() const;

	/**
	 * Compares the filter bar's text with the item's component name. Use
	 * bRecursive to refresh the state of child nodes as well. Returns true if
	 * the node is set to be filtered out
	 */
	bool RefreshFilteredState(FGenericSubobjectEditorTreeNodePtrType TreeNode, bool bRecursive);

	/** Callback for the action trees to get the filter text */
	FText GetFilterText() const;

public:
	/** Get the currently selected nodes from the tree sorted in order from parent to child */
	TArray<FGenericSubobjectEditorTreeNodePtrType> GetSelectedNodes() const;

	/** Get the currently selected handles from the tree sorted in order from parent to child */
	TArray<FGenericSubobjectDataHandle> GetSelectedHandles() const;

	virtual bool IsEditingAllowed() const;

	/** Pointer to the current object that is represented by the subobject editor */
	UObject* GetObjectContext() const;

	/** SubobjectHandle of the current object that is represented by the subobject editor */
	FGenericSubobjectDataHandle GetObjectContextHandle() const;

protected:
	/** Restore the previous selection state when updating the tree */
	virtual void RestoreSelectionState(TArray<FGenericSubobjectEditorTreeNodePtrType>& SelectedTreeNodes, bool bFallBackToVariableName = true);

private:
	/**
	 * Spawns a new SWindow giving the user options for creating a new C++ component class.
	 * The user will be prompted to pick a new subclass name and code will be recompiled
	 *
	 * @return The new class that was created
	 */
	UClass* SpawnCreateNewCppComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass);
	UClass* SpawnCreateNewBPComponentWindow(TSubclassOf<UGenericGraphNodeComponent> ComponentClass);

	/** Add a component from the selection in the combo box */
	FGenericSubobjectDataHandle PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride);
	static void BuildMenuEventsSection(FMenuBuilder& Menu, UClass* SelectedClass, FCanExecuteAction CanExecuteActionDelegate, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate);
	static void CreateEventsForSelection(FName EventName, FGetSelectedObjectsDelegate GetSelectedObjectsDelegate);
	static void ConstructEvent(const FName EventName, const FComponentEventConstructionData EventData);
	static void ViewEvent(const FName EventName, const FComponentEventConstructionData EventData);

	// SSubobjectEditor interface
	virtual bool ShouldModifyBPOnAssetDrop() const { return true; }
	virtual void OnDeleteNodes();
	virtual void CopySelectedNodes();
	virtual void OnDuplicateComponent();
	virtual bool CanPasteNodes() const;
	virtual void PasteNodes();

	virtual void OnAttachToDropAction(FGenericSubobjectEditorTreeNodePtrType DroppedOn, const TArray<FGenericSubobjectEditorTreeNodePtrType>& DroppedNodePtrs);
	virtual void OnDetachFromDropAction(const TArray<FGenericSubobjectEditorTreeNodePtrType>& DroppedNodePtrs);
	// virtual void OnMakeNewRootDropAction(FGenericSubobjectEditorTreeNodePtrType DroppedNodePtr) { ; };
	virtual void PostDragDropAction(bool bRegenerateTreeNodes);

	/** Builds a context menu pop up for dropping a child node onto the scene root node */
	// virtual TSharedPtr<SWidget> BuildSceneRootDropActionMenu(FGenericSubobjectEditorTreeNodePtrType DroppedOntoNodePtr, FGenericSubobjectEditorTreeNodePtrType DroppedNodePtr) { return SNew(SBox); };
	virtual bool ClearSelectionOnClick() const { return true; }
	virtual bool ShowInlineSearchWithButtons() const { return true; }
	virtual FGenericSubobjectDataHandle AddNewSubobject(const FGenericSubobjectDataHandle& ParentHandle, UClass* NewClass, UObject* AssetOverride, FText& OutFailReason, TUniquePtr<FScopedTransaction> InOngoingTransaction);
	virtual void PopulateContextMenuImpl(UToolMenu* InMenu, TArray<FGenericSubobjectEditorTreeNodePtrType>& InSelectedItems, bool bIsChildActorSubtreeNodeSelected);
	// virtual FGenericSubobjectEditorTreeNodePtrType GetSceneRootNode() const override;
public:
	virtual FGenericSubobjectEditorTreeNodePtrType FindSlateNodeForObject(const UObject* InObject, bool bIncludeAttachmentComponents = true) const;

	/** Returns true if the specified component is currently selected */
	bool IsComponentSelected(const UPrimitiveComponent* PrimComponent) const;

	/** Assigns a selection override delegate to the specified component */
	void SetSelectionOverride(UPrimitiveComponent* PrimComponent) const;

	/**
	* Fills the supplied array with the currently selected objects
	* @param OutSelectedItems The array to fill.
	*/
	void GetSelectedItemsForContextMenu(TArray<FComponentEventConstructionData>& OutSelectedItems) const;

	/** Return an array of the current slate node hierarchy in the tree */
	const TArray<FGenericSubobjectEditorTreeNodePtrType>& GetRootNodes() const { return RootNodes; };

	// Attempt to find an existing slate node that matches the given handle
	FGenericSubobjectEditorTreeNodePtrType FindSlateNodeForHandle(const FGenericSubobjectDataHandle& Handle, FGenericSubobjectEditorTreeNodePtrType InStartNodePtr = FGenericSubobjectEditorTreeNodePtrType()) const;

	// Attempt to find an existing slate node that has a given variable name
	FGenericSubobjectEditorTreeNodePtrType FindSlateNodeForVariableName(FName InVariableName) const;

	void UpdateTree(bool bRegenerateTreeNodes = true);
	// End of SSubobjectEditor

	/** @return The visibility of the Add Component combo button */
	virtual EVisibility GetComponentClassComboButtonVisibility() const;
};
