#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

/** Delegate used to set a class */
DECLARE_DELEGATE_OneParam(FOnSetClass, const UClass*);

class UEdNode_GenericGraphNode;

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

