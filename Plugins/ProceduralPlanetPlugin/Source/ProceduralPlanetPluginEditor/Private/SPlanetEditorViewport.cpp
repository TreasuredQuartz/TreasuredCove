// Copywrite Me, All rights Reserved

#include "SPlanetEditorViewport.h"
#include "IPlanetEditor.h"
#include "EditorViewportClient.h"

#include "AdvancedPreviewScene.h"
#include "AssetViewerSettings.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "InputCoreTypes.h"
#include "UnrealWidget.h"

#include "GenericPlatform\GenericPlatformInputDeviceMapper.h"


/** Viewport Client for the preview viewport */
class FPlanetEditorViewportClient : public FEditorViewportClient
{
public:
	FPlanetEditorViewportClient(TWeakPtr<IPlanetEditor> InPlanetEditor, FAdvancedPreviewScene& InPreviewScene, const TSharedRef<SPlanetEditor3DPreviewViewport>& InPlanetEditorViewport);

	// FEditorViewportClient interface
	virtual bool InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = false) override;
	virtual bool InputAxis(FViewport* InViewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples/* =1 */, bool bGamepad/* =false */) override;
	virtual FLinearColor GetBackgroundColor() const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
	virtual bool ShouldOrbitCamera() const override;

	/**
	* Focuses the viewport to the center of the bounding box/sphere ensuring that the entire bounds are in view
	*
	* @param Bounds   The bounds to focus
	* @param bInstant Whether or not to focus the viewport instantly or over time
	*/
	void FocusViewportOnBounds(const FBoxSphereBounds Bounds, bool bInstant = false);

private:

	/** Pointer back to the planet editor tool that owns us */
	TWeakPtr<IPlanetEditor> PlanetEditorPtr;

	/** Preview Scene - uses advanced preview settings */
	class FAdvancedPreviewScene* AdvancedPreviewScene;
};

FPlanetEditorViewportClient::FPlanetEditorViewportClient(TWeakPtr<IPlanetEditor> InPlanetEditor, FAdvancedPreviewScene& InPreviewScene, const TSharedRef<SPlanetEditor3DPreviewViewport>& InPlanetEditorViewport)
	: FEditorViewportClient(nullptr, &InPreviewScene, StaticCastSharedRef<SEditorViewport>(InPlanetEditorViewport))
	, PlanetEditorPtr(InPlanetEditor)
{
	// Setup defaults for the common draw helper.
	DrawHelper.bDrawPivot = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.bDrawGrid = false;
	DrawHelper.GridColorAxis = FColor(80, 80, 80);
	DrawHelper.GridColorMajor = FColor(72, 72, 72);
	DrawHelper.GridColorMinor = FColor(64, 64, 64);
	DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;

	SetViewMode(VMI_Lit);

	EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetSnap(0);
	EngineShowFlags.SetSeparateTranslucency(true);

	OverrideNearClipPlane(1.0f);
	bUsingOrbitCamera = true;

	// Don't want to display the widget in this viewport
	Widget->SetDefaultVisibility(false);

	AdvancedPreviewScene = &InPreviewScene;

}

void FPlanetEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	// Tick the preview scene world.
	if (!GIntraFrameDebuggingGameThread)
	{
		PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

void FPlanetEditorViewportClient::Draw(FViewport* InViewport, FCanvas* Canvas)
{
	FEditorViewportClient::Draw(InViewport, Canvas);
	// PlanetEditorPtr.Pin()->DrawMessages(InViewport, Canvas);
}

bool FPlanetEditorViewportClient::ShouldOrbitCamera() const
{
	// Should always orbit around the preview object to keep it in view.
	return true;
}

bool FPlanetEditorViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	FInputKeyEventArgs Args(InViewport, ControllerId, Key, Event);
	Args.AmountDepressed = AmountDepressed;
	bool bHandled = FEditorViewportClient::InputKey(Args);

	// Handle viewport screenshot.
	bHandled |= InputTakeScreenshot(InViewport, Key, Event);

	bHandled |= AdvancedPreviewScene->HandleInputKey(Args);

	return bHandled;
}

bool FPlanetEditorViewportClient::InputAxis(FViewport* InViewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples/* =1 */, bool bGamepad/* =false */)
{
	bool bResult = true;

	if (!bDisableInput)
	{
		bResult = AdvancedPreviewScene->HandleViewportInput(InViewport, FInputDeviceId::CreateFromInternalId(ControllerId), Key, Delta, DeltaTime, NumSamples, bGamepad);
		if (bResult)
		{
			Invalidate();
		}
		else
		{
			FInputDeviceId DeviceID = INPUTDEVICEID_NONE;
			FPlatformUserId UserId = FGenericPlatformMisc::GetPlatformUserForUserIndex(ControllerId);
			IPlatformInputDeviceMapper::Get().RemapControllerIdToPlatformUserAndDevice(ControllerId, UserId, DeviceID);

			bResult = FEditorViewportClient::InputAxis(InViewport, DeviceID, Key, Delta, DeltaTime, NumSamples, bGamepad);
		}
	}

	return bResult;
}

FLinearColor FPlanetEditorViewportClient::GetBackgroundColor() const
{
	if (AdvancedPreviewScene != nullptr)
	{
		return AdvancedPreviewScene->GetBackgroundColor();
	}
	else
	{
		FLinearColor BackgroundColor = FLinearColor::Black;
		if (PlanetEditorPtr.IsValid())
		{
			
		}
		return BackgroundColor;
	}
}

void FPlanetEditorViewportClient::FocusViewportOnBounds(const FBoxSphereBounds Bounds, bool bInstant /*= false*/)
{
	const FVector Position = Bounds.Origin;
	float Radius = Bounds.SphereRadius;

	float AspectToUse = AspectRatio;
	FIntPoint ViewportSize = Viewport->GetSizeXY();
	if (!bUseControllingActorViewInfo && ViewportSize.X > 0 && ViewportSize.Y > 0)
	{
		AspectToUse = Viewport->GetDesiredAspectRatio();
	}

	const bool bEnable = false;
	ToggleOrbitCamera(bEnable);

	/**
	* We need to make sure we are fitting the sphere into the viewport completely, so if the height of the viewport is less
	* than the width of the viewport, we scale the radius by the aspect ratio in order to compensate for the fact that we have
	* less visible vertically than horizontally.
	*/
	if (AspectToUse > 1.0f)
	{
		Radius *= AspectToUse;
	}

	/**
	* Now that we have a adjusted radius, we are taking half of the viewport's FOV,
	* converting it to radians, and then figuring out the camera's distance from the center
	* of the bounding sphere using some simple trig.  Once we have the distance, we back up
	* along the camera's forward vector from the center of the sphere, and set our new view location.
	*/
	const float HalfFOVRadians = FMath::DegreesToRadians(ViewFOV / 2.0f);
	const float DistanceFromSphere = Radius / FMath::Sin(HalfFOVRadians);
	FViewportCameraTransform& ViewTransform = GetViewTransform();
	FVector CameraOffsetVector = ViewTransform.GetRotation().Vector() * -DistanceFromSphere;

	ViewTransform.SetLookAt(Position);
	ViewTransform.TransitionToLocation(Position + CameraOffsetVector, EditorViewportWidget, bInstant);

	// Tell the viewport to redraw itself.
	Invalidate();
}



//// Constructor And Deconstructor ////

void SPlanetEditor3DPreviewViewport::Construct(const FArguments& InArgs)
{
	PlanetEditorPtr = InArgs._PlanetEditor;
	AdvancedPreviewScene = MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues()));

	// restore last used feature level
	UWorld* PreviewWorld = AdvancedPreviewScene->GetWorld();
	if (PreviewWorld != nullptr)
	{
		PreviewWorld->ChangeFeatureLevel(GWorld->GetFeatureLevel());
	}

	UEditorEngine* Editor = CastChecked<UEditorEngine>(GEngine);
	PreviewFeatureLevelChangedHandle = Editor->OnPreviewFeatureLevelChanged().AddLambda([this](ERHIFeatureLevel::Type NewFeatureLevel)
	{
		AdvancedPreviewScene->GetWorld()->ChangeFeatureLevel(NewFeatureLevel);
	});


	PreviewPrimType = TPT_None;

	SEditorViewport::Construct(SEditorViewport::FArguments());

	// PreviewMaterial = nullptr;
	PreviewMeshComponent = nullptr;
	// PostProcessVolumeActor = nullptr;

	/*UMaterialInterface* Material = MaterialEditorPtr.Pin()->GetMaterialInterface();
	if (Material)
	{
		SetPreviewMaterial(Material);
	}*/

	SetPreviewAsset(GUnrealEd->GetThumbnailManager()->EditorSphere);

	UAssetViewerSettings* Settings = UAssetViewerSettings::Get();
	const int32 ProfileIndex = AdvancedPreviewScene->GetCurrentProfileIndex();
	if (Settings->Profiles.IsValidIndex(ProfileIndex))
	{
		AdvancedPreviewScene->SetEnvironmentVisibility(Settings->Profiles[ProfileIndex].bShowEnvironment, true);
	}
}

SPlanetEditor3DPreviewViewport::~SPlanetEditor3DPreviewViewport()
{
	UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().RemoveAll(this);

	if (PreviewMeshComponent != nullptr)
	{
		PreviewMeshComponent->OverrideMaterials.Empty();
	}

	if (EditorViewportClient.IsValid())
	{
		EditorViewportClient->Viewport = NULL;
	}
}

//// Constructor And Deconstructor ////



//// Interfaces Implementation ////

TSharedRef<FEditorViewportClient> SPlanetEditor3DPreviewViewport::MakeEditorViewportClient()
{
	EditorViewportClient = MakeShareable(new FPlanetEditorViewportClient(PlanetEditorPtr, *AdvancedPreviewScene.Get(), SharedThis(this)));
	UAssetViewerSettings::Get()->OnAssetViewerSettingsChanged().AddRaw(this, &SPlanetEditor3DPreviewViewport::OnAssetViewerSettingsChanged);
	EditorViewportClient->SetViewLocation(FVector::ZeroVector);
	EditorViewportClient->SetViewRotation(FRotator(-15.0f, -90.0f, 0.0f));
	EditorViewportClient->SetViewLocationForOrbiting(FVector::ZeroVector);
	EditorViewportClient->bSetListenerPosition = false;
	EditorViewportClient->EngineShowFlags.EnableAdvancedFeatures();
	EditorViewportClient->EngineShowFlags.SetLighting(true);
	EditorViewportClient->EngineShowFlags.SetIndirectLightingCache(true);
	EditorViewportClient->EngineShowFlags.SetPostProcessing(true);
	EditorViewportClient->Invalidate();
	EditorViewportClient->VisibilityDelegate.BindSP(this, &SPlanetEditor3DPreviewViewport::IsVisible);

	return EditorViewportClient.ToSharedRef();
}

TSharedPtr<FExtender> SPlanetEditor3DPreviewViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SPlanetEditor3DPreviewViewport::OnFloatingButtonClicked()
{
}

void SPlanetEditor3DPreviewViewport::OnAddedToTab(const TSharedRef<SDockTab>& OwnerTab)
{
	ParentTab = OwnerTab;
}

bool SPlanetEditor3DPreviewViewport::IsVisible() const
{
	return ViewportWidget.IsValid() && (!ParentTab.IsValid() || ParentTab.Pin()->IsForeground()) && SEditorViewport::IsVisible();
}

void SPlanetEditor3DPreviewViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(PreviewMeshComponent);
}

TSharedRef<class SEditorViewport> SPlanetEditor3DPreviewViewport::GetViewportWidget()
{
	return SharedThis(this);
}

//// End Interfaces Implementation ////

void SPlanetEditor3DPreviewViewport::OnAssetViewerSettingsChanged(const FName& InPropertyName)
{
	if (InPropertyName == GET_MEMBER_NAME_CHECKED(FPreviewSceneProfile, bRotateLightingRig) || InPropertyName == NAME_None)
	{
		UAssetViewerSettings* Settings = UAssetViewerSettings::Get();
		const int32 ProfileIndex = AdvancedPreviewScene->GetCurrentProfileIndex();
		if (Settings->Profiles.IsValidIndex(ProfileIndex) &&
			Settings->Profiles[ProfileIndex].bRotateLightingRig
			&& !EditorViewportClient->IsRealtime())
		{
			EditorViewportClient->SetRealtime(true);
		}
	}
}

bool SPlanetEditor3DPreviewViewport::SetPreviewAsset(UObject* InAsset)
{
	if (PreviewMeshComponent != nullptr)
		AdvancedPreviewScene->AddComponent(PreviewMeshComponent, FTransform::Identity);
	return (PreviewMeshComponent != nullptr);
}
