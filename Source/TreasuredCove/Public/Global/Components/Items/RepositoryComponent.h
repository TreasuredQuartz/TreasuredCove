// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Repository.h"
#include "Structs/ItemKeyStruct.h"
#include "RepositoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepositoryChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API URepositoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URepositoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	// Called on any changes to member properties in engine
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& e) override;
	// Called by editor for changes made to UPROPERTYs in TArrays
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e) override;
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items|Repository")
	TMap<FString, FRepository> Repository;

	UPROPERTY(BlueprintAssignable)
	FOnRepositoryChanged OnRepositoryChanged;

	// Called to add to item in Repository
	// - Only pass positive numbers. 
	// - (Use SubtractItem if you wish to subtract instead)
	void AddItem(const FString Category, const FItemKey Item);
	// Called to add to item in Repository
	// - Overloaded to accept non-structs
	// - Only pass positive numbers. 
	// - (Use SubtractItem if you wish to subtract instead)
	UFUNCTION(BlueprintCallable, Category = "Items|Repository")
	void AddItem(const FString Category, const FName Name, const uint8 Quantity);

	// Called to subtract from item in repository
	// - Only pass positive numbers. 
	void SubtractItem(const FString Category, const FItemKey Item);
	// Called to subtract from item in repository
	// - Overloaded to accept non-structs
	// - Only pass positive numbers. 
	UFUNCTION(BlueprintCallable, Category = "Items|Repository")
	void SubtractItem(const FString Category, const FName Name, const uint8 Quantity);
	//
	UFUNCTION(BlueprintCallable, Category = "Items|Repository")
	void SubtractItemAtIndex(const FString Category, const uint8 Index, const uint8 Quantity);

	// Called when Quantity reaches zero.
	// - Removes item from array
	// - Removes Category if array is empty
	void RemoveItem(const FString Category, const uint8 Index);

	// Called to find specific item in category.
	// Returns false if unable.
	bool GetItemQuantity(const FString Category, const FName Name, int32& Quantity) const;
	// Called to find specific item in category.
	// - Overloaded for index version
	// Returns false if unable.
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Items|Repository")
	bool GetItemQuantity(const FString Category, const uint8 Index, int32& Quantity) const;
	// Called to search all categories for specific item.
	// Returns false if unable.
	bool GetItemQuantity(const FName Name, int32& Quantity) const;
	// Called to search all categories for specific item.
	// - Overloaded for index version
	// Returns false if unable.
	bool GetItemQuantity(const uint8 Index, int32& Quantity) const;

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Items|Repository")
	int32 GetItemIndex(const FString Category, const FName Name);

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Items|Repository")
	bool GetAllItems(const FString Category, TArray<FItemKey>& Items);
};
