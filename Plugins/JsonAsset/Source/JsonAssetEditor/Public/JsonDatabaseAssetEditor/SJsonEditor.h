// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UJsonDatabaseAsset;
class SMultiLineEditableTextBox;

/**
 * 
 */
class JSONASSETEDITOR_API SJsonEditor : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SJsonEditor) {}

	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, FEditorModeTools* Tools, UJsonDatabaseAsset* InEditingDatabase);

private:
	void RebuildEditor();

	bool OnVerifyFileTextChanged(const FText& InText, FText& OutErrorMessage);
	void OnFileTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

private:
	UJsonDatabaseAsset* EditingDatabase;
	TSharedPtr<SMultiLineEditableTextBox> InlineEditableText;
};
