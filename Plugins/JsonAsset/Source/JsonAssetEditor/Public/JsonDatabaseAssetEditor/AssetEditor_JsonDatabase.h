// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UAssetEditor.h"

#if ENGINE_MAJOR_VERSION == 5
#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5

class UJsonDatabaseAsset;
class SCompoundWidget;

/**
 * 
 */
class JSONASSETEDITOR_API FAssetEditor_JsonDatabase : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
public:
	FAssetEditor_JsonDatabase();
	virtual ~FAssetEditor_JsonDatabase();

	void InitJsonDatabaseAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UJsonDatabaseAsset* JsonDatabase);

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
	TSharedRef<SDockTab> SpawnTab_JsonEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_JsonBrowser(const FSpawnTabArgs& Args);

	void CreateInternalWidgets();
	TSharedRef<SCompoundWidget> CreateJsonEditorWidget();
	TSharedRef<SCompoundWidget> CreateJsonBrowserWidget();

	void RebuildJsonDatabase();

	void OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

protected:
	UJsonDatabaseAsset* EditingDatabase;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;

	TSharedPtr<SCompoundWidget> JsonEditorWidget;
	TSharedPtr<SCompoundWidget> JsonBrowserWidget;
};
