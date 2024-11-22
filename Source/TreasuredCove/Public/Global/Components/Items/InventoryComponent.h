// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

class AGAActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;

private:
	bool bHasInventorySpace;
	uint8 InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items|Inventory", meta=(AllowPrivateAccess="true"))
	TArray<AGAActor*> Inventory;

	bool InventoryCheck();
	uint8 FindEmptySlot();

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	uint8 GetInventorySize() const { return InventorySize; };
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void SetInventorySize(uint8 Size);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool AddItem(AGAActor* Item, int32 Slot = -1);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void MoveItem(uint8 From, uint8 To);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void RemoveItem(uint8 Slot);
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AGAActor* GetItem(uint8 Slot);
		
public:
	UPROPERTY(BlueprintAssignable, Category = "Items|Inventory")
	FOnInventoryChanged OnInventoryChanged;
};
