// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Items/InventoryComponent.h"
#include "EquipmentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEquipped, const FString&, EquipSlotName, const AActor*, EquippedItem);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UEquipmentComponent : public UInventoryComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items | Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FString> EquipmentSlotNames;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items | Inventory", meta = (AllowPrivateAccess = "true"))
	TArray<FTransform> EquipmentSlotTransforms;

protected:
	virtual void InitializeComponent() override;

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeEquipmentSlotTransforms(const TArray<FTransform>& Transforms);

private:
	int32 FindSlotWithName(FString& SlotName) const;

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AGAActor* GetItemInNameSlot(FString SlotName);
	virtual bool AddItem(AGAActor* Item, FString& SlotName);
	virtual void RemoveItem(FString& SlotName);
	virtual bool AddItem(AGAActor* Item, int32 Slot = -1) override;
	virtual void MoveItem(uint8 From, uint8 To) override;
	virtual void RemoveItem(uint8 Slot) override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnItemEquipped OnItemEquipped;
};
