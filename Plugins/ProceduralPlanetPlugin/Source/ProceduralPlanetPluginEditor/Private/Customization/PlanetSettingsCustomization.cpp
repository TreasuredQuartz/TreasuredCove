#include "PlanetSettingsCustomization.h"

TSharedRef<IPropertyTypeCustomization> FPlanetSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FPlanetSettingsCustomization());
}

void FPlanetSettingsCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> ObjectPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& ObjectCustomizationUtils)
{

}

void FPlanetSettingsCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> ObjectPropertyHandle, IDetailChildrenBuilder& ObjectBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
}
