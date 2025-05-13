// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CommandMenuComponent.generated.h"

class UCommandMenu;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCommandMenuChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UCommandMenuComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TSubclassOf<UCommandMenu> ActiveMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TArray<int> MaxActiveMenuSlots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	TArray<int> MaxSubMenuSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	uint8 CurrentActiveMenuSlot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|UI")
	uint8 CurrentSubMenuSlot;

public:	
	// Sets default values for this component's properties
	UCommandMenuComponent();

	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	void MenuUp();
	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	void MenuDown();
	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	void MenuRight();
	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	void MenuLeft();

	uint8 GetCurrentActiveMenuSlot() const;
	uint8 GetCurrentSubMenuSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	void ResetActiveMenuSelection();
	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	void ResetSubMenuSelection();

	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	int DetermineActiveMenuSelection(int MaxMenuCount, int Direction);
	UFUNCTION(BlueprintCallable, Category = "Command Menu")
	int DetermineSubMenuSelection(int MaxMenuCount, int Direction);

	// 
	UFUNCTION(BlueprintPure, Category = "Command Menu")
	FVector2D GetCurrentMenuSelection()
	{
		return FVector2D(CurrentActiveMenuSlot, CurrentSubMenuSlot);
	}
	/*
	UFUNCTION(BlueprintPure, Category = "Command Menu")
	int GetPreviousActiveMenuIndex()
	{
		return PreviousActiveMenuIndex;
	} // */
public:
	FOnCommandMenuChanged OnCommandMenuChanged;
};
