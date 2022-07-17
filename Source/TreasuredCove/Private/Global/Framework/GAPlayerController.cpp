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
	if (!Player)
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
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->AddAbilityToUI(AbilityInfo, AbilityType, InHandle, bFromItem);
}

void AGAPlayerController::RemoveAbilityFromUI(FGameplayAbilitySpecHandle InHandle)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->RemoveAbilityFromUI(InHandle);
}

/* We are not using this right now
*
void AGAPlayerController::OnControllerChanged(bool bIsConnected)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}
}
*/

//
void AGAPlayerController::OnDamaged(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
}

//
void AGAPlayerController::OnHealed(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
}

#pragma region PlayerInformation
//
void AGAPlayerController::OnHealthModified(float Health, float MaxHealth)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnHealthModified(Health, MaxHealth);
}

//
void AGAPlayerController::OnStaminaModified(float Stamina, float MaxStamina)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnStaminaModified(Stamina, MaxStamina);
}

//
void AGAPlayerController::OnManaModified(float Mana, float MaxMana)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnManaModified(Mana, MaxMana);
}

//
void AGAPlayerController::OnExperienceModified(float Experience, float MaxExperience)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnExperienceModified(Experience, MaxExperience);
}

//
void AGAPlayerController::OnStatModified(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnStatModified(Charisma, Constitution, Dexterity, Intelligence, Strength, Wisdom);
}

//
void AGAPlayerController::OnCurrentWeaponAmmoModified(float Ammo, float MaxAmmo)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnCurrentWeaponAmmoModified(Ammo, MaxAmmo);
}

//
void AGAPlayerController::OnAmmoStockModified(FBullet Bullet)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnAmmoStockModified(Bullet);
}

void AGAPlayerController::OnDesiredLocationCleared()
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnDesiredLocationCleared();
}

void AGAPlayerController::OnDesiredLocationSet(FVector DesiredLocation)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnDesiredLocationSet(DesiredLocation);
}

void AGAPlayerController::OnUpdateCurrentBuilding(FName CurrentBuildingName)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnUpdateCurrentBuilding(CurrentBuildingName);
}

void AGAPlayerController::OnUpdateTargetBuilding(FName TargetBuildingName)
{
	if (!HUD)
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
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OpenMainMenu();
}

void AGAPlayerController::BeginPause()
{
	if (!HUD)
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

	if (!HUD)
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
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ConfirmPressed();
}

void AGAPlayerController::ConfirmReleased()
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ConfirmReleased();
}

void AGAPlayerController::ReturnPressed()
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ReturnPressed();
}

void AGAPlayerController::ReturnReleased()
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->ReturnReleased();
}

void AGAPlayerController::RecieveNavRequest(ENavType NavType)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->SendNavigationRequest(NavType);
}

void AGAPlayerController::RecieveTabRequest(bool bNavRight)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->SendTabRequest(bNavRight);
}

void AGAPlayerController::CursorMoveUp(float Val)
{
	if (!HUD)
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
	if (!HUD)
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
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnMenuUpdated(MenuSelection);
}

void AGAPlayerController::SetItemPriority(bool Priority)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->SetItemPriority(Priority);
}

/** Inventory */

//
void AGAPlayerController::NotifyPickup(FName ItemName, bool CanPickup)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->NotifyPickup(ItemName, CanPickup);
}

//
void AGAPlayerController::OnPickupItem(FGAItemInfo& ItemInfo)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnPickupItem(ItemInfo);
}

void AGAPlayerController::OnAddItemToRepository(const FItemKey& Item)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnPickupItemIntoRepository(Item);
}

//
void AGAPlayerController::OnEquipItem(UObject* Item, FGAItemInfo& ItemInfo)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}
	HUD->OnEquipItem(Item, ItemInfo);
}

//
void AGAPlayerController::OnStowedItem(FGAItemInfo& ItemInfo)
{
	if (!HUD)
	{
		HUD = Cast<AGAHUD>(GetHUD());
	}

	HUD->OnStowedItem(ItemInfo);
}

//
void AGAPlayerController::OnEquipItemFromInventory(uint8 Slot)
{
	if (!Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}

	Player->EquipItemFromInventory(Slot);
}

//
void AGAPlayerController::OnStowItemFromInventory(uint8 Slot)
{
	if (!Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}

	Player->StowItemFromInventory(Slot);
}

//
void AGAPlayerController::OnAddEquippedItemToInventory()
{
	if (!Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}

	Player->AddEquippedItemToInventory();
}

//
void AGAPlayerController::OnAddStowedItemToInventory()
{
	if (!Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}

	Player->AddStowedItemToInventory();
}

//
void AGAPlayerController::OnMoveItem(uint8 From, uint8 To)
{
	if (!Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}
	Player->MoveItemWithinInventory(From, To);
}

//
void AGAPlayerController::OnDropItem(uint8 Slot)
{
	if (!Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}
	Player->DropItemFromInventory(Slot);
}
#pragma endregion

/** Dialogue */

//
void AGAPlayerController::StartDialogue(UDlgDialogue* Dialogue, UObject* OtherParticipant)
{
	ActiveContext = UDlgManager::StartDialogue2(Dialogue, OtherParticipant, GetPawn());
	
	if (!HUD)
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
}
