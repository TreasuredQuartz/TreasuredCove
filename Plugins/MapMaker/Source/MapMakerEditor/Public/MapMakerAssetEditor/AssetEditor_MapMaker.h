#pragma once

#include "CoreMinimal.h"
#include "MapMaker.h"
#include "GraphEditor.h"
#include "FPreviewScene_MapMaker.h"

#if ENGINE_MAJOR_VERSION == 5
#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5

class UMapMaker;

class MAPMAKEREDITOR_API FAssetEditor_MapMaker : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
public:
	FAssetEditor_MapMaker();
	virtual ~FAssetEditor_MapMaker();

	void InitMapMakerAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, TObjectPtr<UMapMaker> Graph);

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
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Preview(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Assets(const FSpawnTabArgs& Args);

	void CreateInternalWidgets();
	TSharedRef<SGraphEditor> CreateViewportWidget();
	TSharedRef<SCompoundWidget> CreatePreviewWidget();
	TSharedRef<SCompoundWidget> CreateAssetSelectionWidget();

	void CreateEdGraph();
	void CreateEdAssets();
	void CreateCommandList();

	TSharedPtr<SGraphEditor> GetCurrGraphEditor() const;

	FGraphPanelSelectionSet GetSelectedNodes() const;

	void RebuildMapMaker();

	// Delegates for graph editor commands
	void SelectAllNodes();
	bool CanSelectAllNodes();
	void DeleteSelectedNodes();
	bool CanDeleteNodes();
	void DeleteSelectedDuplicatableNodes();
	void CutSelectedNodes();
	bool CanCutNodes();
	void CopySelectedNodes();
	bool CanCopyNodes();
	void PasteNodes();
	void PasteNodesHere(const FVector2D& Location);
	bool CanPasteNodes();
	void DuplicateNodes();
	bool CanDuplicateNodes();
	void OnRenameNode();
	bool CanRenameNodes() const;

	void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
	void OnNodeDoubleClicked(UEdGraphNode* Node);
	void OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent);

	void OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext);

protected:
	TObjectPtr<UMapMaker> EditingGraph;
	FPreviewScene_MapMaker PreviewScene;

	/** Handle to the registered OnPackageSave delegate */
	FDelegateHandle OnPackageSavedDelegateHandle;

	TSharedPtr<SGraphEditor> ViewportWidget;
	TSharedPtr<class SCompoundWidget> PreviewWidget;
	TSharedPtr<class IDetailsView> PropertyWidget;
	TSharedPtr<class SCompoundWidget> AssetsWidget;

	/** The command list for this editor */
	TSharedPtr<FUICommandList> MapEditorCommands;
};


