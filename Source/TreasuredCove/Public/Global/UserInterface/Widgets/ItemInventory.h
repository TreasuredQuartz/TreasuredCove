// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "ItemInventory.generated.h"

class UItemSlot;
class UPauseMenu;
class UGridPanel;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemInventory : 
	public UGAWidget
{
	GENERATED_BODY()
	
private:
	//
	UPauseMenu* PauseMenu;
protected:
	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_OnMoveItem(int FromSlot, int ToSlot);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TArray<UItemSlot*> InventoryButtons;

	//
	UFUNCTION(BlueprintCallable, Category = "UI")
	void NativeOnDropItem();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void NativeOnEquipItem();
	UFUNCTION(BlueprintCallable, Category = "UI")
	void NativeOnAddItem();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_OnAddItem(int InSlot, UTexture2D* InImage);

	void NativeOnMoveItem(uint8 FromSlot, uint8 ToSlot);
	virtual void NativeConstruct() override;
};
