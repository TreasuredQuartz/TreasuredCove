// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Items/RepositoryComponent.h"

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
	OnRepositoryChanged.Broadcast();
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
	OnRepositoryChanged.Broadcast();
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
	OnRepositoryChanged.Broadcast();
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
	TArray<FString> Keys;
	Repository.GetKeys(Keys);
	for (const FString& Key : Keys)
	{
		FString CurKey = Key;
		FString CurSearchCategory = Category;

		while (true)
		{
			// If we cannot split anymore...
			if (!CurSearchCategory.Contains("|"))
			{
				// If the Key can be split further...
				if (CurKey.Contains("|"))
				{
					FString LKey;
					FString RKey;
					FString Splitter = "|";
					CurKey.Split(Splitter, &LKey, &RKey);

					// Set the current key to the non-split part
					CurKey = LKey;
				}

				// If the last category matches the current key...
				if (CurSearchCategory.Compare(CurKey) == 0)
				{
					if (FRepository* CurRepo = Repository.Find(Key))
					{
						// Append the repository to our return value.
						Items.Append(CurRepo->Repository);
					}
				}

				// The search category cannot be split any further, so the loop ends here regardless.
				break;
			}

			FString LKey;
			FString RKey;
			FString Splitter = "|";
			CurKey.Split(Splitter, &LKey, &RKey);

			FString LSearch;
			FString RSearch;
			CurSearchCategory.Split(Splitter, &LSearch, &RSearch);

			// If the left strings are the same...
			if (LKey.Compare(LSearch) == 0)
			{
				CurKey = RKey;
				CurSearchCategory = RSearch;
			}
			else
			{
				// If the categories don't match, then move on.
				break;
			}
		}
	}

	return Items.IsEmpty() == false;
}

