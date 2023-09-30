
#include "TreasuredCoveEditor.h"
#include "Modules/ModuleManager.h"
#include "ItemDatabase.h"
#include "ItemDatabaseCustomization.h"
#include "PropertyEditor/Public/PropertyEditorModule.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FTreasuredCoveEditorModule, TreasuredCoveEditor, "TreasuredCoveEditor");

#define LOCTEXT_NAMESPACE "TreasuredCoveEditor"

void FTreasuredCoveEditorModule::StartupModule() 
{
	UE_LOG(LogTemp, Warning, TEXT("%s - The header customization is called"), ANSI_TO_TCHAR(__FUNCTION__));
	// import the PropertyEditor module...
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	// to register our custom class
	PropertyModule.RegisterCustomClassLayout(
		// This is the name of the Struct (we can also use "MyStruct" instead)
		// this tells the property editor which is the struct property our customization will applied on.
		UItemDatabase::StaticClass()->GetFName(),
		// this is where our MakeInstance() method is usefull
		FOnGetDetailCustomizationInstance::CreateStatic(&FItemDatabaseCustomization::MakeInstance));

	PropertyModule.NotifyCustomizationModuleChanged();
}

void FTreasuredCoveEditorModule::ShutdownModule() 
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties when the module is shutdown
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(UItemDatabase::StaticClass()->GetFName());

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}


#undef LOCTEXT_NAMESPACE
