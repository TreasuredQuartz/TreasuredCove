// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UPauseMenu : 
	public UGAWidget
{
	GENERATED_BODY()

public:
	//
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void BP_OnMoveItem(int FromSlot, int ToSlot);

	//
	void NativeOnMoveItem(uint8 FromSlot, uint8 ToSlot);

	//
	virtual void NativeConstruct() override;

};
