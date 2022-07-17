// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Global/UserInterface/Widgets/GAWidget.h"
#include "ItemEditor.generated.h"

class URepositoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTextureChangedDelegate, UTexture2D*, newTexture);

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UItemEditor : public UGAWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnTextureChangedDelegate OnTextureChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	URepositoryComponent* OwningCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	URepositoryComponent* OwningItem;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMenuOpened();

	// Should input whether this item was equipped or stowed
	// resulting in a cast to the related item.
	UFUNCTION(BlueprintCallable)
	void OnAddedToCharacter(bool bIsHeldItem);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	bool bIsHeldItem;
};
