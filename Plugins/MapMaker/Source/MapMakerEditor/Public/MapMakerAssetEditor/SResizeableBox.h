// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/** Notification for bool value change */
DECLARE_DELEGATE_OneParam(FOnVector2DChanged, const FVector2D&);
SButton;

/**
 * 
 */
class SResizeableBox : public SCompoundWidget
{
	// SLATE_DECLARE_WIDGET_API(SResizeableBox, SCompoundWidget, MAPMAKEREDITOR_API)

	TSharedPtr<SBox> ContentBox;
	TSharedPtr<SGridPanel> Grid;

public:
	SLATE_BEGIN_ARGS(SResizeableBox)
		: _Content()
		, _Size( FVector2D(1, 1) )
		{}
		/** Slot for this button's content (optional) */
		SLATE_DEFAULT_SLOT(FArguments, Content)
		SLATE_ATTRIBUTE(FVector2D, Size)
		SLATE_ATTRIBUTE(FVector2D, Position)
		SLATE_ARGUMENT(FSlateBrush*, Background)
		SLATE_EVENT(FOnVector2DChanged, OnResized)
	SLATE_END_ARGS()

	/**
	 * Sets the content scale for this widget.
	 *
	 * @param InContentScale Content scaling factor.
	 */
	//void SetSize(TAttribute<FVector2D> InSize)
	//{
	//	SizeAttribute = InSize;
	//	// SizeAttribute.Assign(*this, MoveTemp(InSize));
	//}

	/*const FVector2D GetSize() const
	{
		return SizeAttribute.Get();
	}*/

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	//~ SWidget
	FReply OnMouseButtonDown_NorthSouth(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonDown_EastWest(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonDown_SouthWest(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent);
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~ SWidget

	void Press();
	void Release();
	void Resize(const FVector2D& Delta);

protected:
	/** Disallow public construction */
	// SLATECORE_API SResizeableBox();

private:
	/**/
	FOnVector2DChanged OnResizedCallback;

	/** True if this button is currently in a pressed state */
	uint8 bIsPressed : 1;
private:
	/** The location in screenspace the button was pressed */
	FVector2D PressedScreenSpacePosition;

	FVector2D Size;
	FVector2D Position;

	EMouseCursor::Type ResizeType;
};
