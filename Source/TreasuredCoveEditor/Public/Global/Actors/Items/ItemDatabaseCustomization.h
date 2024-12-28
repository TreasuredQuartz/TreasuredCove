// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "IDetailCustomization.h"

/**
 * 
 */
class TREASUREDCOVEEDITOR_API FItemDatabaseCustomization : public IDetailCustomization
{
private:
	// Array of listview items
	TArray<TSharedPtr<FSlateBrush>> Items;

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

};
