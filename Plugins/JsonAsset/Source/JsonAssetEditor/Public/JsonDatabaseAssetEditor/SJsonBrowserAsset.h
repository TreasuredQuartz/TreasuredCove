// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UJsonAsset;

/**
 * 
 */
class JSONASSETEDITOR_API SJsonBrowserAsset : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SJsonBrowserAsset) {}

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UJsonAsset* InRepresentingAsset);

	void UpdateAsset();
	virtual FReply OnKeyDown(const FGeometry& Geometry, const FKeyEvent& KeyEvent) override;
	bool OnVerifyNameTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	FText GetAssetName() const;
	const FSlateBrush* GetAssetBrush() const;
	const FSlateBrush* GetAssetSeperatorBrush();

private:
	TSharedPtr<SInlineEditableTextBlock> EditableTextBlock;
	UJsonAsset* EditingAsset;
};
