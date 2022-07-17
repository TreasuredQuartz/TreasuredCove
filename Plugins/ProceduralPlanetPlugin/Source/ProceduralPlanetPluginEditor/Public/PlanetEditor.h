// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IPlanetEditor.h"
// #include "Widgets/Docking/SDockableTab.h" UE5 REMOVED DockeableTab???
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

//struct FPlanetGraphEditorSummoner : public FDocumentTabFactoryForObjects<UEdGraph>
//{
//public:
//	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);
//	FPlanetGraphEditorSummoner(TSharedPtr<class FTestEditor> InTestEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);
//	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
//	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;
//protected:
//	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
//	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
//};

/**
 * 
 */
class PROCEDURALPLANETPLUGINEDITOR_API FPlanetEditor : 
	public IPlanetEditor, public FNotifyHook
{
public:
	/** FAssetEditorToolkit interface */
	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	/** Begin IPlanetEditor Interface */
	virtual FText GetOriginalObjectName() const override;
	/** End IPlanetEditor Interface */

	virtual void Init(const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		UObject* ObjectToEdit);

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& inTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& inTabManager) override;

	TSharedRef<SDockTab> SpawnTab_Preview(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_GraphCanvas(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Properties(const FSpawnTabArgs& Args);

	void CreateInternalWidgets();

	// Graph Editor
	TSharedRef<SGraphEditor> CreateGraphEditorWidget();
	FGraphAppearanceInfo GetGraphAppearance() const;
	// End Graph Editor
private:
	// Our object we are editing
	class AProceduralPlanet* Planet;

	// Our graph
	class UPlanetGraph* EdGraph;
private:
	/** List of open tool panels; used to ensure only one exists at any one time */
	// TMap<FName, TWeakPtr<SDockableTab>> SpawnedToolPanels;

	/** Preview Viewport widget */
	TSharedPtr<class SPlanetEditor3DPreviewViewport> PreviewViewport;

	/** Graph Editor */
	TSharedPtr<SGraphEditor> GraphEditor;

	/** Property View */
	TSharedPtr<IDetailsView> DetailsView;

private:
	/**	The tab ids for all the tabs used */
	static const FName PreviewTabId;
	static const FName GraphCanvasTabId;
	static const FName PropertiesTabId;

	/** App Identifier */
	static const FName AppIdentifier;
};
