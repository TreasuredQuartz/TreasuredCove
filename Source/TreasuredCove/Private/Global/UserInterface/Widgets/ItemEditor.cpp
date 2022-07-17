// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/UserInterface/Widgets/ItemEditor.h"
#include "GACharacter.h"
#include "RepositoryInterface.h"
#include "Kismet/GameplayStatics.h"

void UItemEditor::OnMenuOpened_Implementation()
{

}

void UItemEditor::OnAddedToCharacter(bool bInIsHeldItem)
{
	this->bIsHeldItem = bInIsHeldItem;
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Pawn) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Player pawn is valid!!!"));
		if (Pawn->GetClass()->ImplementsInterface(URepositoryInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player pawn implements interface!!!"));
			OwningCharacter = IRepositoryInterface::Execute_GetRepositoryComponent(Pawn);
		} else UE_LOG(LogTemp, Warning, TEXT("Player Pawn does not implement interface..."));
	} else UE_LOG(LogTemp, Warning, TEXT("Player Pawn is nullptr..."));

	AGACharacter* Character = Cast<AGACharacter>(Pawn);
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGACharacter is valid!!!"));
		AActor* Item = bInIsHeldItem ? Character->GetHeldItem() : Character->GetStowedItem();

		if (Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("Item is valid!!!"));
			if (Item->GetClass()->ImplementsInterface(URepositoryInterface::StaticClass())) 
			{
				UE_LOG(LogTemp, Warning, TEXT("Item implements interface!!!"));
				OwningItem = IRepositoryInterface::Execute_GetRepositoryComponent(Item);
				if (OwningItem)
				{
					UE_LOG(LogTemp, Warning, TEXT("Owning Item successfully set to the repository!!!"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Owning item is still nullptr..."));

					IRepositoryInterface* ItemInterface = Cast<IRepositoryInterface>(Item);
					if (ItemInterface)
					{
						UE_LOG(LogTemp, Warning, TEXT("Owning Item successfully cast to interface!"));

						OwningItem = ItemInterface->GetRepositoryComponent();

						if (OwningItem)
						{
							UE_LOG(LogTemp, Warning, TEXT("Owning Item is finally valid!!!"));
						} else UE_LOG(LogTemp, Warning, TEXT("Item is STILL nullptr..."))
					} else UE_LOG(LogTemp, Warning, TEXT("Item failed cast to interface..."))
				}
			} else UE_LOG(LogTemp, Warning, TEXT("Item does not implement interface.."));
		} else UE_LOG(LogTemp, Warning, TEXT("Item is nullptr..."));
	} else UE_LOG(LogTemp, Warning, TEXT("AGACharacter is nullptr..."));
}
