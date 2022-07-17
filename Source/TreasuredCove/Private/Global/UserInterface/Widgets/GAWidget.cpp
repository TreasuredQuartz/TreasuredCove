// Fill out your copywrite description in the Description Page in Project Settings

#include "GAWidget.h"
#include "NavType.h"

void UGAWidget::SetHUD(AGAHUD* HUD)
{
	if (HUD)
	{
		OwningHUD = HUD;
	}

	BP_OnSetHUD();
}

void UGAWidget::RecieveNavRequest(ENavType Val)
{
	switch (Val)
	{
	case ENavType::Up:
		RecieveNavUp();
		UpdateMainNavAppearence();
		break;

	case ENavType::Down:
		RecieveNavDown();
		UpdateMainNavAppearence();
		break;

	case ENavType::Right:
		RecieveNavRight();
		UpdateMainNavAppearence();
		break;

	case ENavType::Left:
		RecieveNavLeft();
		UpdateMainNavAppearence();
		break;
	}
}

void UGAWidget::RecieveTabRequest(bool bNavRight)
{
	if (bNavRight)
	{
		RecieveTabRight();
		UpdateSecondNavAppearence();
	}
	else
	{
		RecieveTabLeft();
		UpdateSecondNavAppearence();
	}
}

void UGAWidget::ConfirmPressed()
{
	BP_ConfirmPressed();
	OnConfirmed.Broadcast();
}

void UGAWidget::ConfirmReleased()
{
	BP_ConfirmReleased();
}

void UGAWidget::ReturnPressed()
{
	BP_ReturnPressed();
}

void UGAWidget::ReturnReleased()
{
	BP_ReturnReleased();
}

void UGAWidget::OnCursorMove(FVector2D Location)
{
	CursorPosition = Location;
}
