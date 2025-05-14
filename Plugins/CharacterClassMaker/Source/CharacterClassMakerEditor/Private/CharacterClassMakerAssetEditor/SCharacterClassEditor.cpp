// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "CharacterClassMakerAssetEditor/SCharacterClassEditor.h"
#include "Widgets/text/SInlineEditableTextBlock.h"
#include "CharacterClass.h"

void SCharacterClassEditor::Construct(const FArguments& InArgs, UCharacterClass* InRepresentingAsset)
{
	EditingClassAsset = InRepresentingAsset;

	RebuildEditor();
}

void SCharacterClassEditor::RebuildEditor()
{
	// We cannot edit any null assets.
	if (!EditingClassAsset)
		return;

	// We capture our current list of features per level up.
	TMap<int32, FFeatureList> FeatureList = EditingClassAsset->LevelUpFeatures;

	// Get only the level keys that have features.
	TArray<int32> LevelKeys;
	FeatureList.GetKeys(LevelKeys);

	// Get our corrosponding lists of features per level.
	TArray<FFeatureList> LevelValues;
	FeatureList.GenerateValueArray(LevelValues);

	/* SNew(SScrollBox)
	+ SScrollBox::Slot()
	[
		SNew(SHorizontalBox)
		// The current level...
		+ SHorizontalBox::Slot()
		.Padding(10.f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SInlineEditableTextBlock)
			.Text(FText::FromString("Level: 1"))
		]

		// The features associated with the current level...
		+ SHorizontalBox::Slot()
		[
			SNew(SExpandableArea)
			// The word "Features:" as a label.
			.HeaderContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Features: "))
			]
			// The current list of features.
			.BodyContent()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox)
					// The grabbable area, to shift features around.
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)
					]

					// The name of the feature
					+ SHorizontalBox::Slot()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Feature Name"))
					]

					// The "Remove" button, to remove individual features.
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
					]
				]
			]
		]

		// The "Add" button, to grant additional features to this level.
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
		]

		// The "Clear" button, to remove all features at this level.
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
		] 
	] // */
	TSharedPtr<SScrollBox> LevelFeaturesTable = SNew(SScrollBox);

	// Iterate over every level key.
	for (int32 Key : LevelKeys)
	{
		FText FormatText = NSLOCTEXT("SCharacterClassEditor", "CharacterClassLevelFormatTable", "Level: {Level}");
		FFormatNamedArguments Args;
		Args.Add("Level", Key);
		FText LevelText = FText::Format(FormatText, Args);

		TSharedPtr<SHorizontalBox> LevelBox = SNew(SHorizontalBox);
		
		LevelBox->AddSlot()
		.Padding(10.f)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			SNew(SInlineEditableTextBlock)
			.Text(LevelText)
		];

		TSharedPtr<SVerticalBox> FeaturesBox = SNew(SVerticalBox);

		// Get every feature that corrosponds to the current level key.
		for (UFeature* Feature : FeatureList[Key].Features)
		{
			// Do not access null features...
			if (Feature == nullptr) continue;

			FeaturesBox->AddSlot()
			[
				SNew(SHorizontalBox)
				// The grabbable area, to shift features around.
				+ SHorizontalBox::Slot()
				[
					SNew(SButton)
				]

				// The name of the feature
				+ SHorizontalBox::Slot()
				[
					SNew(STextBlock)
					.Text(Feature->GetDisplayName())
				]

				// The "Remove" button, to remove individual features.
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
				]
			];
		}

		LevelBox->AddSlot()
		[
			FeaturesBox.ToSharedRef()
		];

		// The feature list, sorted by level and showing every feature granted at its associated level.
		LevelFeaturesTable->AddSlot()
		[
			LevelBox.ToSharedRef()
		];
	}

	this->ChildSlot
	[
		LevelFeaturesTable.ToSharedRef()
	];
}