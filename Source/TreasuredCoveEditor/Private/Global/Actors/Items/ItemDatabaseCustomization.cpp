// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Actors/Items/ItemDatabaseCustomization.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailCategoryBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "ItemDatabase.h"

#define LOCTEXT_NAMESPACE "TreasuredCoveEditor"

TSharedRef<IDetailCustomization> FItemDatabaseCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FItemDatabaseCustomization);
}

void FItemDatabaseCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TSharedRef< IPropertyHandle > Prop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UItemDatabase, ItemDatabase));
    IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory(TEXT("Database"));

    UE_LOG(LogTemp, Warning, TEXT("%s - The header customization is called"), ANSI_TO_TCHAR(__FUNCTION__));
	if (Prop->IsValidHandle())
	{
        Cat.AddProperty(Prop);
	}

    TArray< TWeakObjectPtr< UObject > > Objects;
    DetailBuilder.GetObjectsBeingCustomized(Objects);
    if (Objects.Num() != 1)
    {
        return;
    }
    TWeakObjectPtr< UItemDatabase > MyObject = Cast< UItemDatabase >(Objects[0].Get());

    /*
    Displaying a button that triggers editor-time processing.
    */
    auto OnRegenerate = [MyObject]
    {
        if (MyObject.IsValid())
        {
            MyObject->ItemDatabase.Empty();
        }

        return FReply::Handled();
    };

    Cat.AddCustomRow(LOCTEXT("MyButtonRowFilterString", "Search Filter Keywords"))
        .WholeRowContent()
        [
            SNew(SButton)
            .Text(LOCTEXT("RegenerateBtnText", "Regenerate List"))
            .OnClicked_Lambda(OnRegenerate)
        ];
}

void FItemDatabaseCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class FDetailWidgetRow& HeaderRow,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	UE_LOG(LogTemp, Warning, TEXT("%s - The header customization is called"), ANSI_TO_TCHAR(__FUNCTION__));
	// Should customize here soon
}

void FItemDatabaseCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
	class IDetailChildrenBuilder& StructBuilder,
	IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// Should customize here soon
}


#undef LOCTEXT_NAMESPACE