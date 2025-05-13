// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Framework/GAPlayerController.h"
#include "Global/AbilitySystem/GASystemComponent.h"
#include "Global/Framework/GAHUD.h"
#include "Global/Actors/GAActor.h"
#include "Global/Actors/GACharacter.h"
#include "Global/Components/Characters/PauseAreaComponent.h"

#include "DlgSystem/DlgContext.h"
#include "DlgSystem/DlgManager.h"
#include "Engine/Engine.h"

void AGAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Pause", IE_Pressed, this, &AGAPlayerController::BeginPause);
	// InputComponent->BindAction("Pause", IE_Pressed, Player, &AGACharacter::BeginPause);
	// InputComponent->BindAction("Pause", IE_Released, this, &AGAPlayerController::EndPause_Client);

	Player = Cast<AGACharacter>(this->GetCharacter());
}

void AGAPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	OnPostProcessInput.Broadcast(DeltaTime, bGamePaused);

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

bool AGAPlayerController::CanAccessHUD()
{
	HUD = GetHUD<AGAHUD>();

	return (
		!HUD ||										// HUD shouldn't be a nullptr
		IsLocalController()							// HUD should never be accessed on a non-owning machine.
		);
}

void AGAPlayerController::ShouldDisableInput(bool ShouldDisable)
{
	Player = Cast<AGACharacter>(this->GetCharacter());
	if (Player)
	{
		if (ShouldDisable)
		{
			Player->DisableInput(this);
		}
		else
		{
			Player->EnableInput(this);
		}
	}
}

//
void AGAPlayerController::AddAbilityToUI_Client_Implementation(FAbilityInfo AbilityInfo, EAbilityType AbilityType, FGameplayAbilitySpecHandle InHandle, bool bFromItem)
{
	if (!CanAccessHUD()) return;

	// HUD->AddAbilityToUI(AbilityInfo, AbilityType, InHandle, bFromItem);
}

void AGAPlayerController::RemoveAbilityFromUI_Client_Implementation(FGameplayAbilitySpecHandle InHandle)
{
	if (!CanAccessHUD()) return;

	// HUD->RemoveAbilityFromUI(InHandle);
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

/*
void AGAPlayerController::OnDeath_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->OnDeath();
}

//
void AGAPlayerController::OnDamaged_Client_Implementation(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	if (!CanAccessHUD()) return;

	HUD->OnDamaged(DeltaAmount);
}

//
void AGAPlayerController::OnHealed_Client_Implementation(AActor* SourceActor, EAttributeType AttributeType, float DeltaAmount, float NewValue)
{
	if (!CanAccessHUD()) return;

	HUD->OnHealed(DeltaAmount);
} // */

#pragma region PlayerInformation
/*
void AGAPlayerController::OnHealthModified_Client_Implementation(float Health, float MaxHealth)
{
	if (!CanAccessHUD()) return;

	HUD->OnHealthModified(Health, MaxHealth);
}

//
void AGAPlayerController::OnStaminaModified_Client_Implementation(float Stamina, float MaxStamina)
{
	if (!CanAccessHUD()) return;

	HUD->OnStaminaModified(Stamina, MaxStamina);
}

//
void AGAPlayerController::OnManaModified_Client_Implementation(float Mana, float MaxMana)
{
	if (!CanAccessHUD()) return;

	HUD->OnManaModified(Mana, MaxMana);
}

//
void AGAPlayerController::OnExperienceModified_Client_Implementation(float Experience, float MaxExperience)
{
	if (!CanAccessHUD()) return;

	HUD->OnExperienceModified(Experience, MaxExperience);
}

//
void AGAPlayerController::OnStatModified_Client_Implementation(float Charisma, float Constitution, float Dexterity, float Intelligence, float Strength, float Wisdom)
{
	if (!CanAccessHUD()) return;

	HUD->OnStatModified(Charisma, Constitution, Dexterity, Intelligence, Strength, Wisdom);
}

//
void AGAPlayerController::OnCurrentWeaponAmmoModified_Client_Implementation(float Ammo, float MaxAmmo)
{
	if (!CanAccessHUD()) return;

	HUD->OnCurrentWeaponAmmoModified(Ammo, MaxAmmo);
}

//
void AGAPlayerController::OnAmmoStockModified_Client_Implementation(FBullet Bullet)
{
	if (!CanAccessHUD()) return;

	HUD->OnAmmoStockModified(Bullet);
}

void AGAPlayerController::OnDesiredLocationCleared_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->OnDesiredLocationCleared();
}

void AGAPlayerController::OnDesiredLocationSet_Client_Implementation(FVector DesiredLocation)
{
	if (!CanAccessHUD()) return;

	HUD->OnDesiredLocationSet(DesiredLocation);
}

void AGAPlayerController::OnUpdateCurrentBuilding_Client_Implementation(FName CurrentBuildingName)
{
	if (!CanAccessHUD()) return;

	HUD->OnUpdateCurrentBuilding(CurrentBuildingName);
}

void AGAPlayerController::OnUpdateTargetBuilding_Client_Implementation(FName TargetBuildingName)
{
	if (!CanAccessHUD()) return;

	HUD->OnUpdateTargetBuilding(TargetBuildingName);
} // */
#pragma endregion

/** User Interface */

#pragma region UIInputHandling
/* void AGAPlayerController::ReturnMainMenu_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->OpenMainMenu();
} // */

void AGAPlayerController::BeginPause()
{
	if (!CanAccessHUD()) return;

	if (!bIsPaused) {
		bIsPaused = true;
		bShowMouseCursor = true;
		ShouldDisableInput(true);

		// FInputModeUIOnly InputMode;
		// SetInputMode(InputMode);
		HUD->OpenPauseMenu();
	}
	else {
		EndPause_Client();
	}
}

void AGAPlayerController::EndPause_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	bIsPaused = false;
	bShowMouseCursor = false;
	ShouldDisableInput(false);

	// FInputModeGameOnly InputMode;
	// SetInputMode(InputMode);
	HUD->ClosePauseMenu();
}

/*
void AGAPlayerController::ConfirmPressed_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->ConfirmPressed();
}

void AGAPlayerController::ConfirmReleased_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->ConfirmReleased();
}

void AGAPlayerController::ReturnPressed_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->ReturnPressed();
}

void AGAPlayerController::ReturnReleased_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	HUD->ReturnReleased();
}

void AGAPlayerController::RecieveNavRequest_Client_Implementation(ENavType NavType)
{
	if (!CanAccessHUD()) return;

	HUD->SendNavigationRequest(NavType);
}

void AGAPlayerController::RecieveTabRequest_Client_Implementation(bool bNavRight)
{
	if (!CanAccessHUD()) return;

	HUD->SendTabRequest(bNavRight);
}

void AGAPlayerController::CursorMoveUp_Client_Implementation(float Val)
{
	if (!CanAccessHUD()) return;

	float X = 0;
	float Y = 0;

	if (!GetMousePosition(X, Y))
	{
		return;
	}

	Val += X;

	SetMouseLocation(Val, Y);
}

void AGAPlayerController::CursorMoveRight_Client_Implementation(float Val)
{
	if (!CanAccessHUD()) return;

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
void AGAPlayerController::OnMenuUpdated_Client_Implementation(FVector2D MenuSelection)
{
	if (!CanAccessHUD()) return;

	HUD->OnMenuUpdated(MenuSelection);
}

void AGAPlayerController::SetItemPriority_Client_Implementation(bool Priority)
{
	if (!CanAccessHUD()) return;

	HUD->SetItemPriority(Priority);
} // */

/** Inventory */

/*
void AGAPlayerController::NotifyCanInteract_Client_Implementation(FName ItemName, bool CanPickup)
{
	if (!CanAccessHUD()) return;
	
	HUD->NotifyCanInteract(ItemName, CanPickup);
}

//
void AGAPlayerController::OnPickupItem_Client_Implementation(const FGAItemInfo& ItemInfo)
{
	if (!CanAccessHUD()) return;

	HUD->OnPickupItem(ItemInfo);
}

void AGAPlayerController::OnAddItemToRepository_Client_Implementation(const FItemKey& Item)
{
	if (!CanAccessHUD()) return;
	
	HUD->OnPickupItemIntoRepository(Item);
}

//
void AGAPlayerController::OnEquipItem_Client_Implementation(UObject* Item, const FGAItemInfo& ItemInfo)
{
	if (!CanAccessHUD()) return;

	HUD->OnEquipItem(Item, ItemInfo);
}

//
void AGAPlayerController::OnStowedItem_Client_Implementation(const FGAItemInfo& ItemInfo)
{
	if (!CanAccessHUD()) return;
		
	HUD->OnStowedItem(ItemInfo);
} // 

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

void AGAPlayerController::OnDropItem_Client_Implementation()
{
	if (!CanAccessHUD()) return;

	// HUD->OnDropItem();
}

//
void AGAPlayerController::OnDropItemFromInventory(uint8 Slot)
{
	if (Player)
	{
		Player = Cast<AGACharacter>(GetCharacter());
	}
	Player->DropItemFromInventory(Slot);
} // */
#pragma endregion

/** Dialogue */


void AGAPlayerController::StartDialogue_Client_Implementation(UDlgDialogue* Dialogue, UObject* OtherParticipant)
{
	if (!CanAccessHUD()) return;

	ActiveContext = UDlgManager::StartDialogue2(Dialogue, OtherParticipant, GetPawn());

	// HUD->OnDialogueStarted();
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

	if (!CanAccessHUD()) return;

	// HUD->OnDialogueStarted();
}
