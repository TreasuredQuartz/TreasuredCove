// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GraphEditor.h"
// #include "FPreviewScene_ProceduralBuilding.h"

#if ENGINE_MAJOR_VERSION == 5
#include "UObject/ObjectSaveContext.h"
#endif // #if ENGINE_MAJOR_VERSION == 5

class UProceduralBuilding;

/**
* 
*/
class PROCEDURALBUILDINGEDITOR_API FAssetEditor_ProceduralBuilding : public FAssetEditorToolkit, public FNotifyHook, public FGCObject
{
public:
	FAssetEditor_ProceduralBuilding();
	virtual ~FAssetEditor_ProceduralBuilding();

	void InitProceduralBuildingAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UProceduralBuilding* Graph);

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
		return TEXT("FAssetEditor_LTProceduralBuilding");
	}
	// ~FGCObject interface

protected:
	void CreateInternalWidgets();
	
private:
	UProceduralBuilding* EditingBuilding;
};
