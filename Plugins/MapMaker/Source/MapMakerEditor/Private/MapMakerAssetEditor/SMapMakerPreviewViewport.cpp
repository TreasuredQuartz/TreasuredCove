// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerAssetEditor/SMapMakerPreviewViewport.h"
#include "MapMakerAssetEditor/SMapMakerViewportToolbar.h"
#include "MapMakerAssetEditor/AssetEditor_MapMaker.h"

#include "DragAndDrop/DecoratedDragDropOp.h"
#include "DragAndDrop/ClassDragDropOp.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "DragAndDrop/ExportTextDragDropOp.h"
#include "DragAndDrop/BrushBuilderDragDropOp.h"
#include "InViewportUIDragOperation.h"

#include "Exporters/ExportTextContainer.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "UnrealWidget.h"
#include "TabManager.h"
#include "AssetSelection.h"
#include "LevelUtils.h"
#include "SNotificationList.h"
#include "NotificationManager.h"

#define LOCTEXT_NAMESPACE "LevelViewport"

void SMapMakerPreviewViewport::Construct(const FArguments& InArgs)
{
	LevelViewportClient = InArgs._LevelEditorViewportClient;

	SEditorViewport::Construct(SEditorViewport::FArguments());
}

TSharedRef<class SEditorViewport> SMapMakerPreviewViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SMapMakerPreviewViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SMapMakerPreviewViewport::OnFloatingButtonClicked()
{
}

TSharedRef<FEditorViewportClient> SMapMakerPreviewViewport::MakeEditorViewportClient()
{
	return LevelViewportClient.ToSharedRef();
}

TSharedPtr<SWidget> SMapMakerPreviewViewport::MakeViewportToolbar()
{
	return SNew(SMapMakerViewportToolbar, SharedThis(this))
		.Cursor(EMouseCursor::Default)
		;
}

TSharedRef<SWidget> SMapMakerPreviewViewport::BuildViewportDragDropContextMenu()
{
	return SNew(SImage);
}

void SMapMakerPreviewViewport::SetKeyboardFocusToThisViewport()
{
}

void SMapMakerPreviewViewport::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	// Prevent OnDragEnter from reentering because it will affect the drop preview placement and management.
	// This may happen currently if an unloaded class is dragged from the class viewer and a slow task is triggered,
	// which re-ticks slate and triggers another mouse move.
	static bool bDragEnterReentranceGuard = false;
	if (!bDragEnterReentranceGuard)
	{
		bDragEnterReentranceGuard = true;
		// Don't execute the dragdrop op if the current level is locked.
		// This prevents duplicate warning messages firing on DragEnter and Placement.
		ULevel* CurrentLevel = (GetWorld()) ? GetWorld()->GetCurrentLevel() : nullptr;

		if (CurrentLevel && !FLevelUtils::IsLevelLocked(CurrentLevel))
		{
			if (HandleDragObjects(MyGeometry, DragDropEvent))
			{
				// Hide the decorator before dropping the object to avoid having a decorator present for the 
				// entire duration of an async asset build if required. 
				// Restore the decorator visibility if the drop fails to preserve previous behavior.
				DragDropEvent.GetOperation()->SetDecoratorVisibility(false);
				if (!HandlePlaceDraggedObjects(MyGeometry, DragDropEvent, /*bCreateDropPreview=*/true))
				{
					DragDropEvent.GetOperation()->SetDecoratorVisibility(true);
				}
			}
		}

		bDragEnterReentranceGuard = false;
	}
}

void SMapMakerPreviewViewport::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	if (LevelViewportClient->HasDropPreviewActors())
	{
		LevelViewportClient->DestroyDropPreviewActors();
	}

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (Operation.IsValid())
	{
		Operation->SetDecoratorVisibility(true);

		if (Operation->IsOfType<FDecoratedDragDropOp>())
		{
			TSharedPtr<FDecoratedDragDropOp> DragDropOp = StaticCastSharedPtr<FDecoratedDragDropOp>(Operation);
			DragDropOp->ResetToDefaultToolTip();
		}
	}
}

bool SMapMakerPreviewViewport::HandleDragObjects(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	bool bValidDrag = false;
	TArray<FAssetData> SelectedAssetDatas;

	TSharedPtr< FDragDropOperation > Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		return false;
	}

	if (Operation->IsOfType<FClassDragDropOp>())
	{
		auto ClassOperation = StaticCastSharedPtr<FClassDragDropOp>(Operation);

		bValidDrag = true;

		for (int32 DroppedAssetIdx = 0; DroppedAssetIdx < ClassOperation->ClassesToDrop.Num(); ++DroppedAssetIdx)
		{
			new(SelectedAssetDatas)FAssetData(ClassOperation->ClassesToDrop[DroppedAssetIdx].Get());
		}
	}
	else if (Operation->IsOfType<FExportTextDragDropOp>())
	{
		bValidDrag = true;
	}
	else if (Operation->IsOfType<FBrushBuilderDragDropOp>())
	{
		bValidDrag = true;

		auto BrushOperation = StaticCastSharedPtr<FBrushBuilderDragDropOp>(Operation);

		new(SelectedAssetDatas) FAssetData(BrushOperation->GetBrushBuilder().Get());
	}
	else if (Operation->IsOfType<FInViewportUIDragOperation>())
	{
		bValidDrag = true;
	}
	else
	{
		GetAssetsFromDrag(DragDropEvent, SelectedAssetDatas);

		if (SelectedAssetDatas.Num() > 0)
		{
			bValidDrag = true;
		}
	}

	// Update cached mouse position
	if (bValidDrag)
	{
		// Grab viewport to offset click position correctly
		FIntPoint ViewportOrigin, ViewportSize;
		LevelViewportClient->GetViewportDimensions(ViewportOrigin, ViewportSize);

		// Save off the local mouse position from the drop point for potential use later (with Drag Drop context menu)
		CachedOnDropLocalMousePos = MyGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()) * MyGeometry.Scale;
		CachedOnDropLocalMousePos.X -= ViewportOrigin.X;
		CachedOnDropLocalMousePos.Y -= ViewportOrigin.Y;
	}

	// Update the currently dragged actor if it exists
	bool bDroppedObjectsVisible = true;
	if (LevelViewportClient->UpdateDropPreviewActors(CachedOnDropLocalMousePos.X, CachedOnDropLocalMousePos.Y, DroppedObjects, bDroppedObjectsVisible))
	{
		// if dragged actors were hidden, show decorator
		Operation->SetDecoratorVisibility(!bDroppedObjectsVisible);
	}

	Operation->SetCursorOverride(TOptional<EMouseCursor::Type>());

	FText HintText;

	// Determine if we can drop the assets
	for (auto InfoIt = SelectedAssetDatas.CreateConstIterator(); InfoIt; ++InfoIt)
	{
		const FAssetData& AssetData = *InfoIt;

		// Ignore invalid assets
		if (!AssetData.IsValid())
		{
			continue;
		}

		FDropQuery DropResult = LevelViewportClient->CanDropObjectsAtCoordinates(CachedOnDropLocalMousePos.X, CachedOnDropLocalMousePos.Y, AssetData);

		if (!DropResult.bCanDrop)
		{
			// At least one of the assets can't be dropped.
			Operation->SetCursorOverride(EMouseCursor::SlashedCircle);
			bValidDrag = false;
			HintText = DropResult.HintText;
			break;
		}
		else
		{
			if (HintText.IsEmpty())
			{
				HintText = DropResult.HintText;
			}
		}
	}

	if (Operation->IsOfType<FAssetDragDropOp>())
	{
		auto AssetOperation = StaticCastSharedPtr<FAssetDragDropOp>(DragDropEvent.GetOperation());
		AssetOperation->SetToolTip(HintText, nullptr);
	}

	return bValidDrag;
}

FReply SMapMakerPreviewViewport::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (HandleDragObjects(MyGeometry, DragDropEvent))
	{
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

bool SMapMakerPreviewViewport::HandlePlaceDraggedObjects(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, bool bCreateDropPreview)
{
	bool bAllAssetWereLoaded = false;
	bool bValidDrop = false;
	UActorFactory* ActorFactory = nullptr;

	TSharedPtr< FDragDropOperation > Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		return false;
	}

	// Don't handle the placement if we couldn't handle the drag
	if (!HandleDragObjects(MyGeometry, DragDropEvent))
	{
		return false;
	}

	if (Operation->IsOfType<FClassDragDropOp>())
	{
		auto ClassOperation = StaticCastSharedPtr<FClassDragDropOp>(Operation);

		DroppedObjects.Empty();

		// Check if the asset is loaded, used to see if the context menu should be available
		bAllAssetWereLoaded = true;

		for (int32 DroppedAssetIdx = 0; DroppedAssetIdx < ClassOperation->ClassesToDrop.Num(); ++DroppedAssetIdx)
		{
			UObject* Object = ClassOperation->ClassesToDrop[DroppedAssetIdx].Get();

			if (Object)
			{
				DroppedObjects.Add(Object);
			}
			else
			{
				bAllAssetWereLoaded = false;
			}
		}

		bValidDrop = true;
	}
	else if (Operation->IsOfType<FAssetDragDropOp>())
	{
		bValidDrop = true;
		DroppedObjects.Empty();

		TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);

		ActorFactory = DragDropOp->GetActorFactory();

		bAllAssetWereLoaded = true;
		for (const FAssetData& AssetData : DragDropOp->GetAssets())
		{
			UObject* Asset = AssetData.GetAsset();
			if (Asset != nullptr)
			{
				DroppedObjects.Add(Asset);
			}
			else
			{
				bAllAssetWereLoaded = false;
			}
		}
	}
	// OLE drops are blocking which causes problem when positioning and maintaining the drop preview
	// Drop preview is disabled when dragging from external sources
	else if (!bCreateDropPreview && Operation->IsOfType<FExternalDragOperation>())
	{
		bValidDrop = true;
		DroppedObjects.Empty();

		TArray<FAssetData> DroppedAssetDatas;
		GetAssetsFromDrag(DragDropEvent, DroppedAssetDatas);

		bAllAssetWereLoaded = true;
		for (int32 AssetIdx = 0; AssetIdx < DroppedAssetDatas.Num(); ++AssetIdx)
		{
			const FAssetData& AssetData = DroppedAssetDatas[AssetIdx];

			UObject* Asset = AssetData.GetAsset();
			if (Asset != nullptr)
			{
				DroppedObjects.Add(Asset);
			}
			else
			{
				bAllAssetWereLoaded = false;
			}
		}
	}
	else if (Operation->IsOfType<FExportTextDragDropOp>())
	{
		bValidDrop = true;

		TSharedPtr<FExportTextDragDropOp> DragDropOp = StaticCastSharedPtr<FExportTextDragDropOp>(Operation);

		// Check if the asset is loaded, used to see if the context menu should be available
		bAllAssetWereLoaded = true;
		DroppedObjects.Empty();

		// Create a container object to hold the export text and pass it into the actor placement code
		UExportTextContainer* NewContainer = NewObject<UExportTextContainer>();
		NewContainer->ExportText = DragDropOp->ActorExportText;
		DroppedObjects.Add(NewContainer);
	}
	else if (Operation->IsOfType<FBrushBuilderDragDropOp>())
	{
		bValidDrop = true;
		DroppedObjects.Empty();

		TSharedPtr<FBrushBuilderDragDropOp> DragDropOp = StaticCastSharedPtr<FBrushBuilderDragDropOp>(Operation);

		if (DragDropOp->GetBrushBuilder().IsValid())
		{
			DroppedObjects.Add(DragDropOp->GetBrushBuilder().Get());
		}
	}

	if (bValidDrop)
	{
		// Grab the hit proxy, used for the (potential) context menu
		HHitProxy* HitProxy = LevelViewportClient->Viewport->GetHitProxy(CachedOnDropLocalMousePos.X, CachedOnDropLocalMousePos.Y);

		// If Ctrl is down, pop in the context menu
		const bool bShowDropContextMenu = !bCreateDropPreview && DragDropEvent.IsControlDown() && (!HitProxy || !(HitProxy->IsA(HWidgetAxis::StaticGetType())));
		bool bDropSuccessful = false;

		// Make sure the drop preview is destroyed
		LevelViewportClient->DestroyDropPreviewActors();

		if (!bShowDropContextMenu || !bCreateDropPreview)
		{
			// Otherwise just attempt to drop the object(s)
			TArray< AActor* > TemporaryActors;
			// Only select actor on drop
			const bool SelectActor = !bCreateDropPreview;
			bDropSuccessful = LevelViewportClient->DropObjectsAtCoordinates(CachedOnDropLocalMousePos.X, CachedOnDropLocalMousePos.Y, DroppedObjects, TemporaryActors, false, bCreateDropPreview, SelectActor, ActorFactory);
		}
		else if (bAllAssetWereLoaded && DroppedObjects.Num() > 0)
		{
			FWidgetPath WidgetPath = DragDropEvent.GetEventPath() != nullptr ? *DragDropEvent.GetEventPath() : FWidgetPath();

			FSlateApplication::Get().PushMenu(
				SharedThis(this),
				WidgetPath,
				BuildViewportDragDropContextMenu(),
				DragDropEvent.GetScreenSpacePosition(),
				FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

			bDropSuccessful = true;
		}

		// Give the editor focus (quick Undo/Redo support after a drag drop operation)
		if (ParentLevelEditor.IsValid())
		{
			FGlobalTabmanager::Get()->DrawAttentionToTabManager(ParentLevelEditor.Pin()->GetTabManager().ToSharedRef());
		}

		if (bDropSuccessful)
		{
			SetKeyboardFocusToThisViewport();
		}

		return bDropSuccessful;
	}

	return false;
}

void SMapMakerPreviewViewport::GetAssetsFromDrag(const FDragDropEvent& DragDropEvent, TArray<FAssetData>& AssetDataArray)
{
	AssetDataArray = AssetUtil::ExtractAssetDataFromDrag(DragDropEvent);

	// Did we get anything?
	if (AssetDataArray.Num() == 0)
	{
		// Get files from the drag/drop.
		const TSharedPtr<FDragDropOperation>& Operation = DragDropEvent.GetOperation();
		if (Operation.IsValid())
		{
			if (Operation->IsOfType<FExternalDragOperation>())
			{
				TSharedPtr<FExternalDragOperation> DragDropOp = StaticCastSharedPtr<FExternalDragOperation>(Operation);
				if (DragDropOp->HasFiles())
				{
					const TArray<FString>& Files = DragDropOp->GetFiles();
					if (Files.Num() > 0)
					{
						// See if anyone else can get us an asset.
						UEditorAssetSubsystem* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
						if (EditorAssetSubsystem != nullptr)
						{
							EditorAssetSubsystem->GetOnExtractAssetFromFile().Broadcast(Files, AssetDataArray);
						}
					}
				}
			}
		}
	}
}

FReply SMapMakerPreviewViewport::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (DragDropEvent.GetOperation()->IsOfType<FInViewportUIDragOperation>())
	{
		FVector2D ScreenSpaceDropLocation = DragDropEvent.GetScreenSpacePosition() - DragDropEvent.GetOperationAs<FInViewportUIDragOperation>()->GetDecoratorOffsetFromCursor();
		DragDropEvent.GetOperationAs<FInViewportUIDragOperation>()->BroadcastDropEvent(MyGeometry.AbsoluteToLocal(ScreenSpaceDropLocation));
	}
	else
	{
		ULevel* CurrentLevel = (GetWorld()) ? GetWorld()->GetCurrentLevel() : nullptr;

		if (CurrentLevel && !FLevelUtils::IsLevelLocked(CurrentLevel))
		{
			return HandlePlaceDraggedObjects(MyGeometry, DragDropEvent, /*bCreateDropPreview=*/false) ? FReply::Handled() : FReply::Unhandled();
		}
		else
		{
			FNotificationInfo Info(LOCTEXT("Error_OperationDisallowedOnLockedLevel", "The requested operation could not be completed because the level is locked."));
			Info.ExpireDuration = 3.0f;
			FSlateNotificationManager::Get().AddNotification(Info);
			return FReply::Handled();
		}
	}
	return FReply::Unhandled();
}

#undef LOCTEXT_NAMESPACE