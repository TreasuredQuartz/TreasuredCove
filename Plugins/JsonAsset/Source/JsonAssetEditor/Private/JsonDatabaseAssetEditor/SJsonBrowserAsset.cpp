// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAssetEditor/SJsonBrowserAsset.h"
#include "JsonAsset.h"

#include "Widgets/text/SInlineEditableTextBlock.h"

void SJsonBrowserAsset::Construct(const FArguments& InArgs, UJsonAsset* InEditingAsset)
{
	EditingAsset = InEditingAsset;
	UpdateAsset();
}

void SJsonBrowserAsset::UpdateAsset()
{
	this->ChildSlot
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(0.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillHeight(1.5)
				[
					SNew(SBorder)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
								.Image(this, &SJsonBrowserAsset::GetAssetBrush)
								.DesiredSizeOverride(FVector2D(128, 128))
						]
				]

				+ SVerticalBox::Slot()
				.Padding(0.0f)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillHeight(1)
				[
					SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Top)
								.AutoHeight()
								[
									SNew(SImage)
										.Image(GetAssetSeperatorBrush())
								]

								+ SVerticalBox::Slot()
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Fill)
								[
									SAssignNew(EditableTextBlock, SInlineEditableTextBlock)
										.Text(this, &SJsonBrowserAsset::GetAssetName)
										.OnVerifyTextChanged(this, &SJsonBrowserAsset::OnVerifyNameTextChanged)
										.OnTextCommitted(this, &SJsonBrowserAsset::OnNameTextCommited)
								]

								+ SVerticalBox::Slot()
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Bottom)
								[
									SNew(STextBlock)
										.Text(FText::FromString("Json"))
								]
						]
				]
		];
}

void SJsonBrowserAsset::StartEditingName()
{
	EditableTextBlock->EnterEditingMode();
}

bool SJsonBrowserAsset::OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage)
{
	if (EditingAsset)
	{
		if (EditingAsset->GetAssetFileName() != InText.ToString())
			return true;
	}

	return false;
}

void SJsonBrowserAsset::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (EditingAsset)
	{
		// Change filename here
	}
}

FReply SJsonBrowserAsset::OnKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent)
{
	if ((KeyEvent.GetKey() == EKeys::F2) // &&
		/*(KeyEvent.IsCommandDown() || KeyEvent.IsControlDown()) */)
	{
		// do stuff
		// GetCurrentSelectedAsset();

		StartEditingName();
	}

	return FReply::Handled();
}

FText SJsonBrowserAsset::GetAssetName() const
{
	return FText::FromString(EditingAsset->GetAssetFileName());
}

const FSlateBrush* SJsonBrowserAsset::GetAssetBrush() const
{
	if (EditingAsset)
	{
		return EditingAsset->GetAssetBrush();
	}

	FSlateBrush* Brush = new FSlateBrush();

	return Brush;
}

const FSlateBrush* SJsonBrowserAsset::GetAssetSeperatorBrush()
{
	FSlateBrush* Brush = new FSlateBrush();
	Brush->ImageSize = FVector2D(1, 1);

	return Brush;
}
