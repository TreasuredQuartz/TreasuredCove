// Fill out your copyright notice in the Description page of Project Settings.

#include "GAHUD.h"
#include "GAPlayerController.h"
#include "GAGameInstance.h"
#include "PauseAreaComponent.h"

#include "GameFramework/PlayerInput.h"

#define CURSOR_DRAW_OFFSET 3
#define CONFIRMPRESSED PC->WasInputKeyJustPressed(EKeys::Gamepad_FaceButton_Bottom) || PC->WasInputKeyJustPressed(EKeys::LeftMouseButton)
#define CONFIRMRELEASED PC->WasInputKeyJustReleased(EKeys::Gamepad_FaceButton_Bottom) || PC->WasInputKeyJustReleased(EKeys::LeftMouseButton)
#define DENYPRESSED PC->WasInputKeyJustPressed(EKeys::LeftControl) || PC->WasInputKeyJustPressed(EKeys::Gamepad_FaceButton_Right)
#define DENYRELEASED PC->WasInputKeyJustReleased(EKeys::LeftControl) || PC->WasInputKeyJustReleased(EKeys::Gamepad_FaceButton_Right)
#define NAVUP PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Up) || PC->WasInputKeyJustPressed(EKeys::W) || PC->WasInputKeyJustPressed(EKeys::Up)
#define NAVRIGHT PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Right) || PC->PlayerInput->GetKeyValue(EKeys::D) || PC->PlayerInput->GetKeyValue(EKeys::Right)
#define NAVDOWN PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Down) || PC->WasInputKeyJustPressed(EKeys::S) || PC->WasInputKeyJustPressed(EKeys::Down)
#define NAVLEFT PC->WasInputKeyJustPressed(EKeys::Gamepad_DPad_Left) || PC->WasInputKeyJustPressed(EKeys::A) || PC->WasInputKeyJustPressed(EKeys::Left)
#define CURSORUP PC->WasInputKeyJustPressed(EKeys::Gamepad_RightStick_Up) || PC->PlayerInput->GetKeyValue(EKeys::MouseY) == 1
#define CURSORRIGHT PC->WasInputKeyJustPressed(EKeys::Gamepad_RightStick_Right) || PC->PlayerInput->GetKeyValue(EKeys::MouseX) == 1
#define CURSORDOWN PC->WasInputKeyJustPressed(EKeys::Gamepad_RightStick_Down) || PC->PlayerInput->GetKeyValue(EKeys::MouseY) == -1
#define CURSORLEFT PC->WasInputKeyJustPressed(EKeys::Gamepad_RightStick_Left) || PC->PlayerInput->GetKeyValue(EKeys::MouseX) == -1
#define PAUSE PC->WasInputKeyJustPressed(EKeys::Gamepad_Special_Right) || PC->WasInputKeyJustPressed(EKeys::Tab)
#define OPENQUICK PC->WasInputKeyJustPressed(EKeys::Gamepad_Special_Left) || PC->WasInputKeyJustPressed(EKeys::Q)
#define CLOSEQUICK PC->WasInputKeyJustReleased(EKeys::Gamepad_Special_Left) || PC->WasInputKeyJustReleased(EKeys::Q)

// Construct
AGAHUD::AGAHUD(const FObjectInitializer& ObjectInitailizer)
	: AHUD(ObjectInitailizer)
{
	// Initialize Values
	bIsInMenu = false;
	bIsQuickMenuUp = false;
}

// After Intializing our menus and self
void AGAHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

//
void AGAHUD::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<AGAPlayerController>(PlayerOwner);
	GA = Cast<UGAGameInstance>(GetGameInstance());
}

//
void AGAHUD::PlayerInputChecks()
{
	if (bIsInMenu)
	{
		// Event to close Pause Menu
		if (PAUSE)
		{
			ClosePauseMenu();
			bIsInMenu = false;
			PC->EndPause_Client();
			return;
		}

		// Event to click or select buttons or other ui elements
		if (CONFIRMPRESSED)
		{
			ConfirmPressed();
			return;
		}

		if (CONFIRMRELEASED)
		{
			ConfirmReleased();
			return;
		}

		// Event to back out of a menu or action in the ui
		if (DENYPRESSED)
		{
			ReturnPressed();
			return;
		}

		if (DENYRELEASED)
		{
			ReturnReleased();
			return;
		}

		/* Navigation in ui Events */
		// Up
		if (NAVUP)
		{
			SendNavigationRequest(ENavType::Up);
			return;
		}

		// Right
		if (NAVRIGHT)
		{
			SendNavigationRequest(ENavType::Right);
			SendNavigationRequest(ENavType::Left);
			return;
		}

		// Down
		if (NAVDOWN)
		{
			SendNavigationRequest(ENavType::Down);
			return;
		}

		// Left
		if (NAVLEFT)
		{
			SendNavigationRequest(ENavType::Left);
			return;
		}

		/* Navigation On Top of UI */
		// Up
		if (CURSORUP)
		{
			CursorMoveUp(1);
			return;
		}

		// Down
		if (CURSORDOWN)
		{
			CursorMoveUp(-1);
			return;
		}

		// Right
		if (CURSORRIGHT)
		{
			CursorMoveRight(1);
			return;
		}

		// Left
		if (CURSORLEFT)
		{
			CursorMoveRight(-1);
			return;
		}
	}
	else
	{
		if (PAUSE)
		{
			OpenPauseMenu();
			bIsInMenu = true;
			PC->ShouldDisableInput(true);
			return;
		}
			/* if (bIsQuickMenuUp)
			{
				CloseQuickMenu();
				bIsQuickMenuUp = false;
				return;
			}
		}

		if (OPENQUICK)
		{
			OpenQuickMenu();
			bIsQuickMenuUp = true;
			return;
		}
		else if(CLOSEQUICK)
		{
			CloseQuickMenu();
			bIsQuickMenuUp = false;
			return;
		} */
	}

	if (bIsQuickMenuUp) 
	{
		// Get angle value for selection
		if (CURSORUP || CURSORRIGHT || CURSORDOWN || CURSORLEFT)
		{
			float x = PC->PlayerInput->GetKeyValue("LookUp");
			float y = PC->PlayerInput->GetKeyValue("LookRight");
			FVector2D Direction(x,y);

			QuickMenuNavRequest(Direction);
			return;
		}
	}
}

//
void AGAHUD::DrawHUD_Reset()
{

}

//
void AGAHUD::DrawHUD()
{
	/*

	if (!PlayerOwner)
	{
		PlayerOwner = GetOwningPlayerController();
	}

	if (!PC)
	{
		PC = Cast<AGAPlayerController>(PlayerOwner);
	}

	if (!PC->PlayerInput)
	{
		return;
	}

	PlayerInputChecks();

	if (!bDrawHUD)
	{
		return;
	}

	AHUD::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	// DrawHUD_Reset();

	//	PC->GetMousePosition(MouseLocation.X, MouseLocation.Y);

	// Draw Menu */
}

//
void AGAHUD::OnEquipItemFromInventory(uint8 Slot)
{
	PC->OnEquipItemFromInventory(Slot);
}

//
void AGAHUD::OnStowItemFromInventory(uint8 Slot)
{
	PC->OnStowItemFromInventory(Slot);
}

//
void AGAHUD::OnAddStowedItemToInventory()
{
	PC->OnAddStowedItemToInventory();
}

//
void AGAHUD::OnMoveItem(uint8 From, uint8 To)
{
	PC->OnMoveItem(From, To);
}

//
void AGAHUD::OnDropItemFromInventory(uint8 Slot)
{
	PC->OnDropItemFromInventory(Slot);
}

void AGAHUD::SetCursorMoveOnly(bool CursorOnly)
{
	if (!PC)
	{
		return;
	}

	PlayerOwner->SetIgnoreLookInput(CursorOnly);
}

void AGAHUD::EnterPauseStudio()
{
	if (!GetOwningPawn())
	{
		return;
	}

	if (PauseArea)
	{
		// FInputModeGameOnly InputMode;
		// PC->SetInputMode(InputMode);
		PauseArea->EnterPauseArea_Client();
	}
	else
	{
		PauseArea = Cast<UPauseAreaComponent>(GetOwningPawn()->GetComponentByClass(UPauseAreaComponent::StaticClass()));
		if (PauseArea) PauseArea->EnterPauseArea_Client();
	}
}

void AGAHUD::ExitPauseStudio()
{
	if (PauseArea)
	{
		// FInputModeUIOnly InputMode;
		// PC->SetInputMode(InputMode);
		PauseArea->ExitPauseArea();
	}
}
