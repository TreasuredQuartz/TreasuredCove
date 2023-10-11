// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "Global/Actors/Items/ItemDatabaseCustomization.h"
#include "PropertyEditor/Public/DetailLayoutBuilder.h"
#include "PropertyEditor/Public/DetailCategoryBuilder.h"
#include "PropertyEditor/Public/IDetailChildrenBuilder.h"
#include "PropertyEditor/Public/DetailWidgetRow.h"
#include "PropertyCustomizationHelpers.h"
#include "ItemDatabase.h"
#include "GAActor.h"

#define LOCTEXT_NAMESPACE "TreasuredCoveEditor"

class TREASUREDCOVEEDITOR_API FDatabaseArrayBuilder : public FDetailArrayBuilder
{
public:
    FDatabaseArrayBuilder(TSharedRef<IPropertyHandle> InBaseProperty, bool InGenerateHeader = true, bool InDisplayResetToDefault = true, bool InDisplayElementNum = true)
        : FDetailArrayBuilder(InBaseProperty, InGenerateHeader, InDisplayResetToDefault, InDisplayElementNum)
    {
        
    }
public:
    void GenerateHeaderRowContent(FDetailWidgetRow& Node) override;
};

void FDatabaseArrayBuilder::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
    FDetailArrayBuilder::GenerateHeaderRowContent(NodeRow);

    auto OnTextChanged = [this](FText Text)
    {
        //TArray<FItemDatabaseRow> Items;

        //if (MyObject.IsValid())
        //{
        //    /* TArray<FName> Names;
        //    // TArray<FItemDatabaseRow> Values;
        //    // TArray<FItemDatabaseRow> Items;

        //    // MyObject->ItemDatabase.GenerateValueArray(Values);
        //    // MyObject->ItemDatabase.GenerateKeyArray(Names);

        //    for (FName ItemName : Names)
        //    {
        //        if (ItemName.ToString().Contains(Text.ToString()))
        //        {
        //            Items.Add(MyObject->ItemDatabase[ItemName]);
        //        }

        //        if (MyObject->ItemDatabase[ItemName].Category.Contains(Text.ToString()))
        //        {

        //        }
        //    }*/

        //    if (Text.IsEmpty()) 
        //        return MyObject->ItemDatabase;

        //    for (FItemDatabaseRow Row : MyObject->ItemDatabase)
        //    {
        //        if (Row.Category.Contains(Text.ToString()) || Row.Name.Contains(Text.ToString()))
        //        {
        //            Items.Add(Row);
        //        }
        //    }

        //    return Items;
        //}

        //return Items;
    };

    NodeRow
    .FilterString(LOCTEXT("ItemDatabaseSearch", "Search"))
    .NameContent()
    [
        SNew(SEditableTextBox)
        .HintText(LOCTEXT("HintTextSearch", "Search"))
        .OnTextChanged_Lambda(OnTextChanged)
    ];
}

TSharedRef<IDetailCustomization> FItemDatabaseCustomization::MakeInstance()
{
	// Create the instance and returned a SharedRef
	return MakeShareable(new FItemDatabaseCustomization);
}

void FItemDatabaseCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    TArray< TWeakObjectPtr< UObject > > Objects;
    DetailBuilder.GetObjectsBeingCustomized(Objects);
    if (Objects.Num() != 1)
    {
        return;
    }
    TWeakObjectPtr< UItemDatabase > MyObject = Cast< UItemDatabase >(Objects[0].Get());

    TSharedRef< IPropertyHandle > Prop = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UItemDatabase, ItemDatabase));
    IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory(TEXT("Database"));

    if (Prop->IsValidHandle())
    {
        auto OnGenerateRow = [](TSharedPtr<FSlateBrush> Item, const TSharedRef<STableViewBase>& OwnerTable)
        {
            //Create the row
            return
                SNew(STableRow< TSharedPtr<FSlateBrush> >, OwnerTable)
                .Padding(2.0f)
                [
                    SNew(SImage)
                    .Image(Item.Get())
                ];
        };

        TSharedPtr<SResetToDefaultMenu> ResetToDefaultMenu;
        DetailBuilder.HideProperty(Prop);
        AGAActor* ItemObject = const_cast<AGAActor*>(TSubclassOf<AGAActor>().GetDefaultObject());

        auto GenerateAdditionalItemWidget = [](TSharedRef<IPropertyHandle> PropertyHandle, int32 ArrayIndex, IDetailChildrenBuilder& ChildrenBuilder)
        {
            IDetailPropertyRow& Row = ChildrenBuilder.AddProperty(PropertyHandle);

            Row.ShowPropertyButtons(false);
            Row.CustomWidget(true)
            .NameContent()
            [
                PropertyHandle->CreatePropertyNameWidget()
            ]
            .ValueContent()
            .MaxDesiredWidth(TOptional<float>(1))
            [
                PropertyHandle->CreatePropertyValueWidget()
            ];
        };

        // Create the additional textures widget
        TSharedRef<FDetailArrayBuilder> ItemDatabaseBuilder = MakeShareable(new FDatabaseArrayBuilder(Prop, true, true, true));
        ItemDatabaseBuilder->OnGenerateArrayElementWidget(FOnGenerateArrayElementWidget::CreateLambda(GenerateAdditionalItemWidget));
        Cat.AddCustomBuilder(ItemDatabaseBuilder);
    }
}


#undef LOCTEXT_NAMESPACE