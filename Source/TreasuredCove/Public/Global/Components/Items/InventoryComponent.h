// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items | Inventory", meta=(AllowPrivateAccess="true"))
	TArray<AGAActor*> Inventory;

	bool InventoryCheck();
	uint8 FindEmptySlot();

public:
	void SetInventorySize(uint8 Size);
	bool AddItem(AGAActor* Item);
	void MoveItem(uint8 From, uint8 To);
	void RemoveItem(uint8 Slot);
	AGAActor* GetItem(uint8 Slot);
		
};
