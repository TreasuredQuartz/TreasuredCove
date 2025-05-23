// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "JsonDatabaseAssetEditor/SJsonEditor.h"
#include "JsonDatabaseAsset.h"
#include "JsonAsset.h"

#include "Widgets/input/SMultiLineEditableTextBox.h"

#define LOCTEXT_NAMESPACE "JsonEditor"

void SJsonEditor::Construct(const FArguments& InArgs, FEditorModeTools* Tools, UJsonDatabaseAsset* InEditingDatabase)
{
	EditingDatabase = InEditingDatabase;
	if (EditingDatabase)
	{
		EditingDatabase->OnEditNewAsset.Add(FOnDatabaseEditNewAsset::FDelegate::CreateSP(this, &SJsonEditor::RebuildEditor));
		RebuildEditor();
	}
}

void SJsonEditor::RebuildEditor()
{
	UJsonAsset* EditingFile;

	if (!EditingDatabase) return;
	EditingFile = EditingDatabase->GetCurrentAsset();
	if (!EditingFile) return;
	
	FString FullString = EditingFile->GetJsonString();
	/* TArray<FString> JsonString = EditingFile->GetJsonString();

	for (FString String : JsonString)
	{
		FullString += String + "\n";
	} */

	this->ChildSlot
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(InlineEditableText, SMultiLineEditableTextBox)
				.Text(FText::FromString(FullString))
				.OnVerifyTextChanged(this, &SJsonEditor::OnVerifyFileTextChanged)
				// .OnTextChanged(this, &SJsonEditor::OnFileTextChanged)
				.OnTextCommitted(this, &SJsonEditor::OnFileTextCommited)
		];
}

bool SJsonEditor::OnVerifyFileTextChanged(const FText& InText, FText& OutErrorMessage)
{
	UJsonAsset* EditingFile;

	if (!EditingDatabase) return false;
	EditingFile = EditingDatabase->GetCurrentAsset();
	if (!EditingFile) return false;

	FString JsonString = EditingFile->GetJsonString();
	FString NewString = InText.ToString();
	// InText.ToString().ParseIntoArrayLines(NewString, false);

	// If they are different, then it has changed.
	return NewString != JsonString;
}

void SJsonEditor::OnFileTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	if (EditingDatabase && EditingDatabase->GetCurrentAsset())
	{
		const FScopedTransaction Transaction(LOCTEXT("JsonEditorEditAssetContents", "Json Editor: Edit Asset Contents"));
		EditingDatabase->Modify();
		EditingDatabase->GetCurrentAsset()->Modify();
		FString NewString = InText.ToString();
		UE_LOG(LogTemp, Warning, TEXT("Setting [%s] Contents to: %s"), *EditingDatabase->GetCurrentAsset()->GetAssetFileName(), *NewString);
		// InText.ToString().ParseIntoArrayLines(NewString, false);
		EditingDatabase->GetCurrentAsset()->SetAssetContents(NewString);
		RebuildEditor();
	}
}

#undef LOCTEXT_NAMESPACE
