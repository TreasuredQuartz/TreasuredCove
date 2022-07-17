// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "Engine/Texture2D.h"
#include "ItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemSlot : 
	public UGAWidget
{
	GENERATED_BODY()
	
public:
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "UI")
	uint8 ItemSlot;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UTexture2D* ItemImage;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UDraggedItemSlot> DraggedItemSlotClass;
	//
	//UItemInventory* OwningInventory;

	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_OnMoveItem();

	//
	virtual void ConfirmPressed() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& MyGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void NativeOnMoveItem(UTexture2D* Image);
	virtual void NativeConstruct() override;
};
