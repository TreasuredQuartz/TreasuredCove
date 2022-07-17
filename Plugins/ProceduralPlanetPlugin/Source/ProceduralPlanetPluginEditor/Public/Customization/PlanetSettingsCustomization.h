#pragma once

#include "PropertyEditor/Public/IPropertyTypeCustomization.h"

class FPlanetSettingsCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> ObjectPropertyHandle,
		class FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& ObjectCustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> ObjectPropertyHandle,
		class IDetailChildrenBuilder& ObjectBuilder,
		IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};

