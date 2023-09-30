// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "PropertyEditor/Public/IDetailCustomization.h"

/**
 * 
 */
class TREASUREDCOVEEDITOR_API FItemDatabaseCustomization : public IDetailCustomization
{
public:
	/**
	 * It is just a convenient helpers which will be used
	 * to register our customization. When the propertyEditor module
	 * find our FMyStruct property, it will use this static method
	 * to instanciate our customization object.
	 */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** Called when details should be customized */
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	// BEGIN IPropertyTypeCustomization interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils);
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle,
		class IDetailChildrenBuilder& StructBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils);
	// END IPropertyTypeCustomization interface
};
