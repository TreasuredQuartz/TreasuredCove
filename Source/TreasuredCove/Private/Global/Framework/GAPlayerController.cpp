// Fill out your copyright notice in the Description page of Project Settings.

#include "GAPlayerController.h"
#include "GAHUD.h"
#include "GAActor.h"
#include "GACharacter.h"

#include "DlgContext.h"
#include "DlgManager.h"
#include "Engine/Engine.h"

//void AGAPlayerController::SetupInputComponent()
//{
//	Super::SetupInputComponent();
//
//	InputComponent->BindAction("Pause", IE_Pressed, this, &AGAPlayerController::BeginPause);
//	// InputComponent->BindAction("Pause", IE_Released, this, &AGAPlayerController::EndPause);
//}

void AGAPlayerController::ShouldDisableInput(bool ShouldDisable)
{
	if (Player)
	{
		Player = Cast<AGACharacter>(this->GetCharacter());
	}

	if (ShouldDisable)
	{
		Player->DisableInput(this);
	}
	else
	{
		Player->EnableInput(this);
	}
}

//
void AGAPlayerController::AddAbilityToUI(FAbilityInfo AbilityInfo, EAbilityType AbilityType, FGameplayAbilitySpecHandle InHandle, bool bFromItem)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->AddAbilityToUI(AbilityInfo, AbilityType, InHandle, bFromItem);
}

void AGAPlayerController::RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->RemoveAbilityFromUI(InHandle);
}

/* We are not using this right now
*
void AGAPlayerController::OnControllerChanged(bool bIsConnected)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}
}
*/

//
void AGAPlayerController::OnDeath()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
		HUD->OnDeath();
	}

}

//
void AGAPlayerController::OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
		HUD->OnDamaged(DeltaAmount);
	}

}

//
void AGAPlayerController::OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
		HUD->OnHealed(DeltaAmount);
	}

}

#pragma region PlayerInformation
//
void AGAPlayerController::OnHealthModified(float Health, float MaxHealth)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
		HUD->OnHealthModified(Health, MaxHealth);
	}

}

//
void AGAPlayerController::OnStaminaModified(float Stamina, float MaxStamina)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnStaminaModified(Stamina, MaxStamina);
}

//
void AGAPlayerController::OnManaModified(float Mana, float MaxMana)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnManaModified(Mana, MaxMana);
}

//
void AGAPlayerController::OnExperienceModified(float Experience, float MaxExperience)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnExperienceModified(Experience, MaxExperience);
}

//
void AGAPlayerController::OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnStatModified(Charisma, Constitution, Dexterity, Intelligence, Strength, Wisdom);
}

//
void AGAPlayerController::OnCurrentWeaponAmmoModified(float Ammo, float MaxAmmo)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnCurrentWeaponAmmoModified(Ammo, MaxAmmo);
}

//
void AGAPlayerController::OnAmmoStockModified(FBullet Bullet)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnAmmoStockModified(Bullet);
}

void AGAPlayerController::OnDesiredLocationCleared()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnDesiredLocationCleared();
}

void AGAPlayerController::OnDesiredLocationSet(FVector DesiredLocation)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnDesiredLocationSet(DesiredLocation);
}

void AGAPlayerController::OnUpdateCurrentBuilding(FName CurrentBuildingName)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnUpdateCurrentBuilding(CurrentBuildingName);
}

void AGAPlayerController::OnUpdateTargetBuilding(FName TargetBuildingName)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnUpdateTargetBuilding(TargetBuildingName);
}
#pragma endregion

/** User Interface */

#pragma region UIInputHandling
void AGAPlayerController::ReturnMainMenu()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OpenMainMenu();
}

void AGAPlayerController::BeginPause()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	if (!bIsPaused) {
		bIsPaused = true;
		bShowMouseCursor = true;

		//FInputModeUIOnly InputMode;
		//SetInputMode(InputMode);
		HUD->OpenPauseMenu();
	}
	else {
		EndPause();
	}
}

void AGAPlayerController::EndPause()
{
	bIsPaused = false;

	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}
	bShowMouseCursor = false;

	//FInputModeGameOnly InputMode;
	//SetInputMode(InputMode);
	HUD->ClosePauseMenu();
}

void AGAPlayerController::ConfirmPressed()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ConfirmPressed();
}

void AGAPlayerController::ConfirmReleased()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ConfirmReleased();
}

void AGAPlayerController::ReturnPressed()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ReturnPressed();
}

void AGAPlayerController::ReturnReleased()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ReturnReleased();
}

void AGAPlayerController::RecieveNavRequest(ENavType NavType)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->SendNavigationRequest(NavType);
}

void AGAPlayerController::RecieveTabRequest(bool bNavRight)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->SendTabRequest(bNavRight);
}

void AGAPlayerController::CursorMoveUp(float Val)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	float X = 0;
	float Y = 0;

	if (!GetMousePosition(X, Y))
	{
		return;
	}

	Val += X;

	SetMouseLocation(Val, Y);
}

void AGAPlayerController::CursorMoveRight(float Val)
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	float X = 0;
	float Y = 0;

	if (!GetMousePosition(X, Y))
	{
		return;
	}

	Val += Y;

	SetMouseLocation(X, Val);
}
#pragma endregion

#pragma region ItemHandling
void AGAPlayerController::OnMenuUpdated(FVector2D MenuSelection)
{
	HUD = Cast<AGAHUD>(GetHUD());

	if (HUD)
	{
		HUD->OnMenuUpdated(MenuSelection);
	}

}

void AGAPlayerController::SetItemPriority(bool Priority)
{
	HUD = Cast<AGAHUD>(GetHUD());

	if (HUD)
	{
		HUD->SetItemPriority(Priority);
	}

}

/** Inventory */

//
void AGAPlayerController::NotifyCanInteract(FName ItemName, bool CanPickup)
{
	HUD = Cast<AGAHUD>(GetHUD());
	if (HUD)
	{
		HUD->NotifyCanInteract(ItemName, CanPickup);
	}

}

//
void AGAPlayerController::OnPickupItem(FGAItemInfo& ItemInfo)
{
	HUD = Cast<AGAHUD>(GetHUD());
	if (HUD)
	{
		HUD->OnPickupItem(ItemInfo);
	}

}

void AGAPlayerController::OnAddItemToRepository(const FItemKey& Item)
{
	HUD = Cast<AGAHUD>(GetHUD());
	if (HUD)
	{
		HUD->OnPickupItemIntoRepository(Item);
	}

}

//
void AGAPlayerController::OnEquipItem(UObject* Item, FGAItemInfo& ItemInfo)
{
	HUD = Cast<AGAHUD>(GetHUD());
	if (HUD)
	{
		HUD->OnEquipItem(Item, ItemInfo);
	}
}

//
void AGAPlayerController::OnStowedItem(FGAItemInfo& ItemInfo)
{
	HUD = Cast<AGAHUD>(GetHUD());
	if (HUD)
	{
		HUD->OnStowedItem(ItemInfo);
	}

}

//
void AGAPlayerController::OnEquipItemFromInventory(uint8 Slot)
{
	Player = Cast<AGACharacter>(GetCharacter());
	if (Player)
	{
		Player->EquipItemFromInventory(Slot);
	}

}

//
void AGAPlayerController::OnStowItemFromInventory(uint8 Slot)
{
	Player = Cast<AGACharacter>(GetCharacter());
	if (Player)
	{
		Player->StowItemFromInventory(Slot);
	}

}

//
void AGAPlayerController::OnAddEquippedItemToInventory()
{
	Player = Cast<AGACharacter>(GetCharacter());
	if (Player)
	{
		Player->AddEquippedItemToInventory();
	}

}

//
void AGAPlayerController::OnAddStowedItemToInventory()
{
	Player = Cast<AGACharacter>(GetCharacter());
	if (Player)
	{
		Player->AddStowedItemToInventory();
	}

}

//
void AGAPlayerController::OnMoveItem(uint8 From, uint8 To)
{
	Player = Cast<AGACharacter>(GetCharacter());
	if (Player)
	{
		Player->MoveItemWithinInventory(From, To);
	}
}

void AGAPlayerController::OnDropItem()
{
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}
	HUD->OnDropItem();
}

//
void AGAPlayerController::OnDropItemFromInventory(uint8 Slot)
{
	if (Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}
	Player->DropItemFromInventory(Slot);
}
#pragma endregion

/** Dialogue */


void AGAPlayerController::StartDialogue(UDlgDialogue* Dialogue, UObject* OtherParticipant)
{
	ActiveContext = UDlgManager::StartDialogue2(Dialogue, OtherParticipant, GetPawn());
	
	if (HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnDialogueStarted();
}

//
void AGAPlayerController::SelectDialogueOption(int32 Index)
{
	if (ActiveContext == nullptr || Index < 0 || Index >= ActiveContext->GetOptionsNum())
	{
		return;
	}

	if (!ActiveContext->ChooseOption(Index))
	{
		ActiveContext = nullptr;
	}

	HUD->OnDialogueStarted();
	
}
