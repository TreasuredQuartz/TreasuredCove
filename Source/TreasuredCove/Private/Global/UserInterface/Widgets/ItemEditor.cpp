// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/UserInterface/Widgets/ItemEditor.h"
#include "Global/Actors/GACharacter.h"
#include "Global/Actors/GAActor.h"
#include "Global/Interfaces/RepositoryInterface.h"
#include "Kismet/GameplayStatics.h"

void UItemEditor::OnMenuOpened_Implementation()
{

}

void UItemEditor::OnAddedToCharacter()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Pawn) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Player pawn is valid!!!"));
		if (Pawn->GetClass()->ImplementsInterface(URepositoryInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player pawn implements interface!!!"));
			OwningCharacter = Pawn;
		} else UE_LOG(LogTemp, Warning, TEXT("Player Pawn does not implement interface..."));
	} else UE_LOG(LogTemp, Warning, TEXT("Player Pawn is nullptr..."));

	AGACharacter* Character = Cast<AGACharacter>(Pawn);
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("AGACharacter is valid!!!"));
		OwningItem = bIsHeldItem ? Character->GetStowedItem() : Character->GetHeldItem();
		if (Character->GetStowedItem()) UE_LOG(LogTemp, Warning, TEXT("StowedItem is valid!"));
		if (Character->GetHeldItem()) UE_LOG(LogTemp, Warning, TEXT("HeldItem is valid!"));
		if (!OwningItem) UE_LOG(LogTemp, Warning, TEXT("Owning Item is nullptr..."));

	} else UE_LOG(LogTemp, Warning, TEXT("AGACharacter is nullptr..."));
}
