// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/UserInterface/Widgets/ItemSlot.h"
#include "Global/UserInterface/Widgets/DraggedItemSlot.h"
#include "Global/UserInterface/Widgets/ItemDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Global/UserInterface/Widgets/ItemInventory.h"

//
void UItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
	//OwningInventory = Cast<UItemInventory>(GetParent());
}

//
void UItemSlot::ConfirmPressed()
{
	//UDragDropOperation* Operation;

	if (!UWidgetBlueprintLibrary::IsDragDropping())
	{
		//OnDragDetected(Operation);
	}
	else
	{
		//Operation = UWidgetBlueprintLibrary::GetDragDroppingContent();
		//OnDrop(Operation);
	}
}

FReply UItemSlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseButtonDown(MyGeometry, MouseEvent);
	FReply reply = UWidgetBlueprintLibrary::DetectDragIfPressed(MouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	
	return reply;
}

//
void UItemSlot::NativeOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(MyGeometry, PointerEvent, OutOperation);

	UDraggedItemSlot* DraggedVisual = CreateWidget<UDraggedItemSlot>(GetOwningPlayer(), DraggedItemSlotClass);
	DraggedVisual->ItemImage = ItemImage;

	UItemDragDropOperation* DragDropOp = Cast<UItemDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragDropOperation::StaticClass()));
	DragDropOp->DefaultDragVisual = DraggedVisual;
	DragDropOp->Slot = ItemSlot;
	DragDropOp->Image = ItemImage;
	DragDropOp->Source = this;

	OutOperation = DragDropOp;
}

//
bool UItemSlot::NativeOnDrop(const FGeometry& MyGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(MyGeometry,InDragDropEvent, InOperation);

	UItemDragDropOperation* DragDropOp = Cast<UItemDragDropOperation>(InOperation);

	if (!DragDropOp) 
	{
		return true;
	}

	if(DragDropOp->Source)
		if (ItemImage)
		{
			DragDropOp->Source->NativeOnMoveItem(nullptr);
		}
		else 
		{
			DragDropOp->Source->NativeOnMoveItem(ItemImage);
		}

	//NativeOnMoveItem(DragDropOp->Image);
	//OwningInventory->NativeOnMoveItem(DragDropOp->Slot, ItemSlot);

	FEventReply Reply(FEventReply::FEventReply(true));
	UWidgetBlueprintLibrary::EndDragDrop(Reply);

	return true;
}

//
void UItemSlot::NativeOnMoveItem(UTexture2D* Image)
{
	ItemImage = Image;
	BP_OnMoveItem();
}
