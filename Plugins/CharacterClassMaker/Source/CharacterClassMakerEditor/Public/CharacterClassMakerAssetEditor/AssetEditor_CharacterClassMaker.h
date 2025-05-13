// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if ENGINE_MAJOR_VERSION == 5
#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5

class UCharacterClass;

/**
 * 
 */
class CHARACTERCLASSMAKEREDITOR_API FAssetEditor_CharacterClassMaker : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
public:
	FAssetEditor_CharacterClassMaker();
	virtual ~FAssetEditor_CharacterClassMaker();

	void InitCharacterClassMakerAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UCharacterClass* CharacterClassMaker);

	// IToolkit interface
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	// End of IToolkit interface

	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FString GetDocumentationLink() const override;
	virtual void SaveAsset_Execute() override;
	// End of FAssetEditorToolkit

	// FSerializableObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	// End of FSerializableObject interface

	// FGCObject interface
	virtual FString GetReferencerName() const
	{
		return TEXT("FAssetEditor_LTMapMaker");
	}
	// ~FGCObject interface

protected:
	TSharedRef<SDockTab> SpawnTab_ClassEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_ClassBrowser(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);

	void CreateInternalWidgets();
	TSharedRef<SCompoundWidget> CreateClassEditorWidget();
	TSharedRef<SCompoundWidget> CreateClassBrowserWidget();

	void RebuildCharacterClassMaker();

	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);
	void OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

protected:
	TObjectPtr<UCharacterClass> EditingClass;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;

	TSharedPtr<SCompoundWidget> ClassEditorWidget;
	TSharedPtr<SCompoundWidget> ClassBrowserWidget;
	TSharedPtr<IDetailsView> PropertyWidget;
};
