// Fill out your copyright notice in the Description page of Project Settings.


#include "RepositoryComponent.h"

#pragma region Defaults
// Sets default values for this component's properties
URepositoryComponent::URepositoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	Repository.Add("");
}

// Called when the game starts
void URepositoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

#if WITH_EDITOR
// Called on any changes to member properties in engine
void URepositoryComponent::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	//if (PropertyName == GET_MEMBER_NAME_CHECKED(UCustomClass, PropertyName))
	//{
	//	// Do Stuff to Property
	//}

	Super::PostEditChangeProperty(e);
}

// Called by editor for changes made to UPROPERTYs in TArrays
void URepositoryComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& e)
{
	// Guard Against null properties
	if (e.Property == NULL) return;

	FName MemberPropertyName = e.MemberProperty->GetFName();
	FName PropertyName = e.Property->GetFName();

	/*if (PropertyName == GET_MEMBER_NAME_CHECKED(URepositoryComponent, Repository))
	{
		switch(e.ChangeType)
			case EPropertyChangeType::ArrayAdd:
				Repository[];
				break;
	}*/

	Super::PostEditChangeChainProperty(e);
}
#endif
#pragma endregion

void URepositoryComponent::AddItem(const FString Category, const FName Name, const uint8 Quantity)
{
	AddItem(Category, FItemKey(Name, Quantity));
}
void URepositoryComponent::AddItem(const FString Category, const FItemKey Item)
{
	// If no array is found for the given category...
	if (!Repository.Contains(Category))
	{	
		// ... Then create new array containing Item.
		FRepository newArray;
		newArray.AddItem(Item);
		Repository.Add(Category, newArray);
		return;
	}

	// The array itself
	Repository[Category].AddItem(Item);
}

void URepositoryComponent::SubtractItem(const FString Category, const FName Name, const uint8 Quantity)
{
	SubtractItem(Category, FItemKey(Name, Quantity));
}
void URepositoryComponent::SubtractItem(const FString Category, const FItemKey Item)
{
	// If no array is found for the given category...
	if (!Repository.Contains(Category))
	{
		// ... Then do nothing.
		return;
	}

	// The array itself
	Repository[Category].SubtractItem(Item);
}
void URepositoryComponent::SubtractItemAtIndex(const FString Category, const uint8 Index, const uint8 Quantity)
{
	// If no array is found for the given category...
	if (!Repository.Contains(Category))
	{
		// ... Then do nothing.
		return;
	}

	// The array itself
	Repository[Category].SubtractItem(Index, Quantity);
}

void URepositoryComponent::RemoveItem(const FString Category, const uint8 Index)
{
	if (Repository.Contains(Category))
	{
		Repository[Category].RemoveItem(Index);
		if (Repository[Category].GetRepositorySize() <= 0)
			Repository.Remove(Category);
	}
}

bool URepositoryComponent::GetItemQuantity(const uint8 Index, int32& Quantity) const
{
	TArray<FString> Categories;
	Repository.GetKeys(Categories);

	// For every category...
	for (const FString& Category : Categories)
	{
		// ... Search for item...
		if (GetItemQuantity(Category, Index, Quantity)) return true;
	}

	// ... fail since we did not find item.
	return false;
}
bool URepositoryComponent::GetItemQuantity(const FName Name, int32& Quantity) const
{
	TArray<FString> Categories;
	Repository.GetKeys(Categories);

	// For every category...
	for (const FString& Category : Categories)
	{
		// ... Search for item...
		if (GetItemQuantity(Category, Name, Quantity)) return true;
	}

	// ... fail since we did not find item.
	return false;
}
bool URepositoryComponent::GetItemQuantity(const FString Category, const uint8 Index, int32& Quantity) const
{
	// If it is null then fail
	if (!Repository.Contains(Category)) return false;

	// Get our category array...
	const FRepository* Array = Repository.Find(Category);

	// Reuturn true if found or false otherwise
	return Array->GetItemQuantity(Index, Quantity);
}
bool URepositoryComponent::GetItemQuantity(const FString Category, const FName Name, int32& Quantity) const
{
	// If it is null then fail
	if (!Repository.Contains(Category)) return false;

	// Get our category array...
	const FRepository* Array = Repository.Find(Category);

	// Reuturn true if found or false otherwise
	return Array->GetItemQuantity(Name, Quantity);
}

int32 URepositoryComponent::GetItemIndex(const FString Category, const FName Name)
{
	if (!Repository.Contains(Category)) return -1;

	const FRepository* Array = Repository.Find(Category);

	return Array->GetItemIndex(Name);
}

bool URepositoryComponent::GetAllItems(const FString Category, TArray<FItemKey>& Items)
{
	if (!Repository.Contains(Category)) return false;

	Items = Repository.Find(Category)->Repository;

	return true;
}

