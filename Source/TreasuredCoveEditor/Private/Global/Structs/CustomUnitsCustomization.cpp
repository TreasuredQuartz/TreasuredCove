// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Structs/CustomUnitsCustomization.h"
#include "Global/Structs/CustomUnits.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SSpinBox.h"

// We need our struct and basics slate widgets now
#include "SlateBasics.h"
// #include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "CustomUnitsCustomization"

void FCustomUnitsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyHandle> ValueProp = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCustomUnits, Value));
	TSharedRef<SWidget> NameWidget = PropertyHandle->CreatePropertyNameWidget();

	float value;
	ValueProp->GetValue(value);

	auto OnValueCommitted = [ValueProp](const float& InNumericValue, ETextCommit::Type InType)
	{
		ValueProp->SetValue(InNumericValue);
	};

	auto OnTextCommitted = [ValueProp](const FText& InText, ETextCommit::Type InType)
	{
		FString* L = nullptr;
		FString* R = nullptr;

		InText.ToString().Split(" ", L, R);

		if (L) ValueProp->SetValue(FCString::Atof(**L));
	};

	// PropertyHandle->CreatePropertyValueWidget();
	TSharedRef<SWidget> ValueWidget =
		SNew(SSpinBox<float>)
		.Value(value)
		.OnValueCommitted_Lambda(OnValueCommitted);

	/* if (TSharedPtr<SSpinBox<float>> SpinBox = StaticCastSharedPtr<SSpinBox<float>>(ValueWidget.ToSharedPtr()))
	{
		// This is valid, but like; idk what to do at this point.
		if (SpinBox.IsValid()) SpinBox->SetValue(value);
	} // */

	// SNew(SSpinBox<float>).Value(value);

	HeaderRow
	.NameContent()
	[
		//Create Slate widgets to display in the Name column of the
		//property editor here - a good default is to call the
		//CreatePropertyNameWidget like below, as it generates
		//the standard name column for the property.
		NameWidget
	]
	.ValueContent()
	[
		//Create Slate widgets to display the value of the property here.
		//What you need to put here depends entirely on what you are doing.
		ValueWidget
		/* SNew(SEditableTextBox) // Should be a sspinbox
			.Text(FText::Format(LOCTEXT("ValueContent", "{0} {1}"), value, GetPostfix()))
			// .Font(FEditorStyle::GetFontStyle("PropertyWindow.NormalFont"))
			.OnTextCommitted_Lambda(OnTextCommitted) */
	];
}

void FCustomUnitsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
}

TSharedRef<IPropertyTypeCustomization> FGrainsCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FGrainsCustomization);
}

FText FGrainsCustomization::GetPostfix() const
{
	return LOCTEXT("grains_postfix", "gr");
}

TSharedRef<IPropertyTypeCustomization> FRPMCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FRPMCustomization);
}

FText FRPMCustomization::GetPostfix() const
{
	return LOCTEXT("rounds_per_minute_postfix", "rpm");
}

#undef LOCTEXT_NAMESPACE

