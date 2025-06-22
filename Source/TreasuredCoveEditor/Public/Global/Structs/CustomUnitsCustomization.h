// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "IPropertyTypeCustomization.h"

/**
 *
 */
class TREASUREDCOVEEDITOR_API FCustomUnitsCustomization : public IPropertyTypeCustomization
{
public:
	/**
	 * Called when the header of the property (the row in the details panel where the property is shown)
	 * If nothing is added to the row, the header is not displayed
	 *
	 * @param PropertyHandle			Handle to the property being customized
	 * @param HeaderRow					A row that widgets can be added to
	 * @param StructCustomizationUtils	Utilities for customization
	 */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	/**
	 * Called when the children of the property should be customized or extra rows added
	 *
	 * @param PropertyHandle			Handle to the property being customized
	 * @param StructBuilder				A builder for adding children
	 * @param StructCustomizationUtils	Utilities for customization
	 */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;

	virtual FText GetPostfix() const { return FText(); };
};

/**
 *
 */
class TREASUREDCOVEEDITOR_API FGrainsCustomization : public FCustomUnitsCustomization
{
public:
	/**
	 * It is just a convenient helpers which will be used
	 * to register our customization. When the propertyEditor module
	 * find our FMyStruct property, it will use this static method
	 * to instanciate our customization object.
	 */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FText GetPostfix() const override;
};

/**
 *
 */
class TREASUREDCOVEEDITOR_API FRPMCustomization : public FCustomUnitsCustomization
{
public:
	/**
	 * It is just a convenient helpers which will be used
	 * to register our customization. When the propertyEditor module
	 * find our FMyStruct property, it will use this static method
	 * to instanciate our customization object.
	 */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	FText GetPostfix() const override;
};
