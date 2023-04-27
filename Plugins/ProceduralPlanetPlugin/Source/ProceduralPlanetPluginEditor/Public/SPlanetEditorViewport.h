#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "PreviewScene.h"
#include "SEditorViewport.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Editor/UnrealEd/Public/SCommonEditorViewportToolbarBase.h"

class IPlanetEditor;

class SPlanetEditor3DPreviewViewport : 
	public SEditorViewport, 
	public FGCObject, 
	public ICommonEditorViewportToolbarInfoProvider
{
public:
	SLATE_BEGIN_ARGS(SPlanetEditor3DPreviewViewport) {}
		SLATE_ARGUMENT(TWeakPtr<IPlanetEditor>, PlanetEditor)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	~SPlanetEditor3DPreviewViewport();

	//// Begin SEditorViewport Interface ////
	TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//// End SEditorViewport Interface ////

	//// Begin FGCObject Interface ////
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	//// End FGCObject Interface ////

	//// Begin ICommonEditorViewportToolbarInfoProvider Interface ////
	TSharedRef<class SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;
	//// End ICommonEditorViewportToolbarInfoProvider Interface ////

	/** Call back for when the user changes preview scene settings in the UI */
	void OnAssetViewerSettingsChanged(const FName& InPropertyName);

	/**
	 * Sets the mesh on which to preview the material.  One of either InStaticMesh or InSkeletalMesh must be non-NULL!
	 * Does nothing if a skeletal mesh was specified but the material has bUsedWithSkeletalMesh=false.
	 *
	 * @return	true if a mesh was set successfully, false otherwise.
	 */
	bool SetPreviewAsset(UObject* InAsset);

	/** Component for the preview mesh. */
	UMeshComponent* PreviewMeshComponent;

	/** The preview primitive we are using. */
	EThumbnailPrimType PreviewPrimType;

	//// Begin SEditorViewport overrides ////
	/** The planet editor has been added to a tab */
	void OnAddedToTab(const TSharedRef<SDockTab>& OwnerTab);
private:
	bool IsVisible() const override;
	//// End SEditorViewport overrides ////

	/** The parent tab where this viewport resides */
	TWeakPtr<SDockTab> ParentTab;

	/** Pointer back to the material editor tool that owns us */
	TWeakPtr<IPlanetEditor> PlanetEditorPtr;

	/** Level viewport client */
	TSharedPtr<class FPlanetEditorViewportClient> EditorViewportClient;

	/** Preview Scene - uses advanced preview settings */
	TSharedPtr<class FAdvancedPreviewScene> AdvancedPreviewScene;

	/** Handle to the registered OnPreviewFeatureLevelChanged delegate. */
	FDelegateHandle PreviewFeatureLevelChangedHandle;
};

