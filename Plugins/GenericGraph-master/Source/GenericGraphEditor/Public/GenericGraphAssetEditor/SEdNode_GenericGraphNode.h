#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SComponentClassCombo.h"

/** Delegate used to set a class */
DECLARE_DELEGATE_OneParam(FOnSetClass, const UClass*);

class UEdNode_GenericGraphNode;
class UGenericGraphNode;
class UGenericGraphNodeComponent;
struct FGenericSubobjectDataHandle;

// Tree node pointer types
using FGenericComponentListNodePtrType = TSharedPtr<class FGenericComponentListNode>;

/**
 * Wrapper struct that represents access an single subobject that is used
 * by slate as a layout for columns/rows should look like
 */
class GENERICGRAPHEDITOR_API FGenericComponentListNode : public TSharedFromThis<FGenericComponentListNode>
{
public:
	explicit FGenericComponentListNode(UObject* InObject) : Data(InObject) { ; }
	~FGenericComponentListNode() = default;

	bool bIsSeperator;

	bool IsSeperator() { return bIsSeperator; };

	UObject* GetDataSource() { return Data; }

	UObject* Data;
};

class GENERICGRAPHEDITOR_API SGenericNodeComponentList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGenericNodeComponentList) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UGenericGraphNode* InNode);

	void UpdateTree();

	FGenericSubobjectDataHandle AddNewSubobject(UClass* NewClass, UObject* Asset, FText& FailReason);

	FGenericSubobjectDataHandle PerformComboAddClass(TSubclassOf<UGenericGraphNodeComponent> ComponentClass, EComponentCreateAction::Type ComponentCreateAction, UObject* AssetOverride);

	TSharedRef<ITableRow> GenerateListRow(FGenericComponentListNodePtrType InNodePtr, const TSharedRef<STableViewBase>& OwnerTable);
	
private:
	UGenericGraphNode* GenericGraphNode;

	TArray<FGenericComponentListNodePtrType> NodeList;
	TSharedPtr<SListView<FGenericComponentListNodePtrType>> ListWidget;
};

class SGenericComponent_RowWidget : public SMultiColumnTableRow<FGenericComponentListNodePtrType>
{
public:
	SLATE_BEGIN_ARGS(SGenericComponent_RowWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TWeakPtr<SGenericNodeComponentList> InEditor, FGenericComponentListNodePtrType InNodePtr, TSharedPtr<STableViewBase> InOwnerTableView);

	// SMultiColumnTableRow<T> interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

	FGenericComponentListNodePtrType GetComponentPtr() { return ComponentPtr; }
protected:
	/** Drag-drop handlers */
	void HandleOnDragEnter(const FDragDropEvent& DragDropEvent);
	void HandleOnDragLeave(const FDragDropEvent& DragDropEvent);
	FReply HandleOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	TOptional<EItemDropZone> HandleOnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericComponentListNodePtrType TargetItem);
	FReply HandleOnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone DropZone, FGenericComponentListNodePtrType TargetItem);
	/** End Drag-drop handlers */

private:
	TWeakPtr<SGenericNodeComponentList> ComponentEditor;
	FGenericComponentListNodePtrType ComponentPtr;

	TSharedPtr<SInlineEditableTextBlock> InlineWidget;
};

class GENERICGRAPHEDITOR_API SEdNode_GenericGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdNode_GenericGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdNode_GenericGraphNode* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	virtual TSharedRef<SWidget> GenerateComponentPicker();
	virtual void OnComponentPicked(UClass* PickedComponentClass);
	virtual void SendToObjects(const FString& NewValue);
	virtual void CreateClassFilterObtions();

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;
	virtual const TArray<TSharedPtr<FText>>* GetComponentItems();

public:
	/* Adds a new Image with the brush to the list */
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FText> Item, const TSharedRef<STableViewBase>& OwnerTable);
	/* The actual UI list */
	TSharedPtr< SListView< TSharedPtr<FText> > > ListViewWidget;
	/* The real data list */
	TArray<TSharedPtr<FText>> ListItems;
	/* Component Class addition button */
	TSharedPtr<SComboButton> AddComponentButton;
	/** Filter used to restrict class viewer widgets in the editor context to imported namespaces only */
	TSharedPtr<class IClassViewerFilter> ImportedClassViewerFilter;

protected:
private:
	/** Options used for creating the component class viewer. */
	// FClassViewerInitializationOptions ComponentClassViewerOptions;
	/** Attribute used to get the currently selected class (required if PropertyEditor == null) */
	TAttribute<const UClass*> SelectedClass;
	/** Delegate used to set the currently selected class (required if PropertyEditor == null) */
	FOnSetClass OnSetClass;
};

