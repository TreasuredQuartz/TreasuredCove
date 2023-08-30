#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

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

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;

public:
	/* Adds a new Image with the brush to the list */
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FSlateBrush> Item, const TSharedRef<STableViewBase>& OwnerTable);
	/* The actual UI list */
	TSharedPtr< SListView< TSharedPtr<FSlateBrush> > > ListViewWidget;
protected:
};

