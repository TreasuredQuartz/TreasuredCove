// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SResizeableBox.h"
#include "SlateWrapperTypes.h"

void SResizeableBox::Construct(const FArguments& InArgs)
{
	Size = InArgs._Size.Get();
	Position = InArgs._Position.Get();
	OnResizedCallback = InArgs._OnResized;

	// Text overrides button content. If nothing is specified, put an null widget in the button.
	// Null content makes the button enter a special mode where it will ask to be as big as the image used for its border.
	struct
	{
		TSharedRef<SWidget> operator()(const FArguments& InOpArgs) const
		{
			if ((InOpArgs._Content.Widget == SNullWidget::NullWidget) && (InOpArgs._Background->IsSet()))
			{
				return SNew(SImage)
					.Visibility(EVisibility::HitTestInvisible)
					.Image(InOpArgs._Background);
			}
			else
			{
				return InOpArgs._Content.Widget;
			}
		}
	} DetermineContent;
	ResizeContent = DetermineContent(InArgs);

	UpdateAppearence();
}

void SResizeableBox::UpdateAppearence()
{
	FVector2D EdgeSize = FVector2D(10, 10);
	FSlateColor EdgeColor = FSlateColor(FLinearColor(1, 1, 1, 0.1));
	ChildSlot
		[
			SNew(SGridPanel)
				.FillColumn(1, 0)
				+ SGridPanel::Slot(0, 0)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeSouthEast)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_SouthWest)
				]

				+ SGridPanel::Slot(1, 0)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeUpDown)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_NorthSouth)
				]

				+ SGridPanel::Slot(2, 0)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeSouthWest)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_SouthWest)
				]

				+ SGridPanel::Slot(0, 1)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeLeftRight)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_EastWest)
				]

				+ SGridPanel::Slot(1, 1)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(ContentBox, SBox)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.WidthOverride(Size.X)
						.HeightOverride(Size.Y)
						[
							ResizeContent.ToSharedRef()
						]
				]

				+ SGridPanel::Slot(2, 1)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeLeftRight)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_EastWest)
				]

				+ SGridPanel::Slot(0, 2)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeSouthWest)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_SouthWest)
				]

				+ SGridPanel::Slot(1, 2)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeUpDown)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_NorthSouth)
				]

				+ SGridPanel::Slot(2, 2)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(EdgeColor)
						.DesiredSizeOverride(EdgeSize)
						.Cursor(EMouseCursor::ResizeSouthEast)
						.Visibility(EVisibility::Visible)
						.OnMouseButtonDown(this, &SResizeableBox::OnMouseButtonDown_SouthWest)
				]
		];
}

FReply SResizeableBox::OnMouseButtonDown_NorthSouth(const FGeometry & SenderGeometry, const FPointerEvent & MouseEvent)
{
	FEventReply EventReply;
	EventReply.NativeReply = FReply::Unhandled();
	const FKey DragKey = EKeys::LeftMouseButton;

	UE_LOG(LogTemp, Warning, TEXT("ResizeableBox: Mouse Button Down!"));

	if (IsEnabled() && (MouseEvent.GetEffectingButton() == DragKey))
	{
		Press();

		// UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, Widget);
		PressedScreenSpacePosition = MouseEvent.GetScreenSpacePosition();

		if (MouseEvent.GetEffectingButton() == DragKey || MouseEvent.IsTouchEvent())
		{
			EventReply.NativeReply = FReply::Handled();
			// return UWidgetBlueprintLibrary::DetectDrag(EventReply, WidgetDetectingDrag, DragKey);
			EventReply.NativeReply = EventReply.NativeReply.DetectDrag(AsShared(), DragKey);
		}
		

		//we need to capture the mouse for MouseUp events
		EventReply.NativeReply.CaptureMouse(AsShared());

		ResizeType = EMouseCursor::ResizeUpDown;
	}

	// return the constructed reply
	return EventReply.NativeReply;
}

FReply SResizeableBox::OnMouseButtonDown_EastWest(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply EventReply;
	EventReply.NativeReply = FReply::Unhandled();
	const FKey DragKey = EKeys::LeftMouseButton;

	UE_LOG(LogTemp, Warning, TEXT("ResizeableBox: Mouse Button Down!"));

	if (IsEnabled() && (MouseEvent.GetEffectingButton() == DragKey))
	{
		Press();

		// UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, Widget);
		PressedScreenSpacePosition = MouseEvent.GetScreenSpacePosition();

		if (MouseEvent.GetEffectingButton() == DragKey || MouseEvent.IsTouchEvent())
		{
			EventReply.NativeReply = FReply::Handled();
			// return UWidgetBlueprintLibrary::DetectDrag(EventReply, WidgetDetectingDrag, DragKey);
			EventReply.NativeReply = EventReply.NativeReply.DetectDrag(AsShared(), DragKey);
		}


		//we need to capture the mouse for MouseUp events
		EventReply.NativeReply.CaptureMouse(AsShared());
		ResizeType = EMouseCursor::ResizeLeftRight;
	}

	// return the constructed reply
	return EventReply.NativeReply;
}

FReply SResizeableBox::OnMouseButtonDown_SouthWest(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply EventReply;
	EventReply.NativeReply = FReply::Unhandled();
	const FKey DragKey = EKeys::LeftMouseButton;

	UE_LOG(LogTemp, Warning, TEXT("ResizeableBox: Mouse Button Down!"));

	if (IsEnabled() && (MouseEvent.GetEffectingButton() == DragKey))
	{
		Press();

		// UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, Widget);
		PressedScreenSpacePosition = MouseEvent.GetScreenSpacePosition();

		if (MouseEvent.GetEffectingButton() == DragKey || MouseEvent.IsTouchEvent())
		{
			EventReply.NativeReply = FReply::Handled();
			// return UWidgetBlueprintLibrary::DetectDrag(EventReply, WidgetDetectingDrag, DragKey);
			EventReply.NativeReply = EventReply.NativeReply.DetectDrag(AsShared(), DragKey);
		}


		//we need to capture the mouse for MouseUp events
		EventReply.NativeReply.CaptureMouse(AsShared());
		ResizeType = EMouseCursor::ResizeSouthWest;
	}

	// return the constructed reply
	return EventReply.NativeReply;
}

FReply SResizeableBox::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FEventReply EventReply;
	EventReply.NativeReply = FReply::Unhandled();
	const FKey DragKey = EKeys::LeftMouseButton;

	UE_LOG(LogTemp, Warning, TEXT("ResizeableBox: Mouse Button Up!"));

	if (bIsPressed && MouseEvent.GetEffectingButton() == DragKey)
	{
		Release();
		OnResizedCallback.Execute(ContentBox->GetDesiredSize());

		if (MouseEvent.GetEffectingButton() == DragKey || MouseEvent.IsTouchEvent())
		{
			EventReply.NativeReply = FReply::Handled();
			// return UWidgetBlueprintLibrary::DetectDrag(EventReply, WidgetDetectingDrag, DragKey);
			EventReply.NativeReply = EventReply.NativeReply.DetectDrag(AsShared(), DragKey);
		}

		//we need to capture the mouse for MouseUp events
		EventReply.NativeReply.ReleaseMouseCapture();
	}

	return EventReply.NativeReply;
}

FReply SResizeableBox::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = FReply::Unhandled();

	if (bIsPressed)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResizeableBox: Mouse Move!"));

		const FVector2D Delta = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
		Resize(Delta);
		Reply = FReply::Handled();
	}

	return Reply;
}

void SResizeableBox::Press()
{
	if (!bIsPressed)
	{
		bIsPressed = true;
	}
}

void SResizeableBox::Release()
{
	if (bIsPressed)
	{
		bIsPressed = false;
	}
}

void SResizeableBox::Resize(const FVector2D& Delta)
{
	if (!OnResizedCallback.IsBound())
		return;

	// OnResizedCallback.Execute(Delta);
	// UpdateAppearence();

	switch (ResizeType)
	{
	case EMouseCursor::ResizeLeftRight:
		ContentBox.Get()->SetWidthOverride(Delta.X);
		return;
	case EMouseCursor::ResizeUpDown:
		ContentBox.Get()->SetHeightOverride(Delta.Y);
		return;
	default: break;
	}

	ContentBox.Get()->SetWidthOverride(Delta.X);
	ContentBox.Get()->SetHeightOverride(Delta.Y); // */
}

