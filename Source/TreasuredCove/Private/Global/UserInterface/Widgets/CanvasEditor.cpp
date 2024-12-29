// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/UserInterface/Widgets/CanvasEditor.h"
#include "Global/UserInterface/TextureCreator.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h"

void UCanvasEditor::NativeConstruct()
{
	Super::NativeConstruct();

}

void UCanvasEditor::ConfirmPressed()
{
	bShouldDraw = !bShouldDraw;
}

void UCanvasEditor::ConfirmReleased()
{

}

void UCanvasEditor::OnCursorMove(FVector2D Location)
{
	Super::OnCursorMove(Location);
	if (bShouldDraw)
	{
		uint8 X = roundf(Location.X + CanvasWidth / 2);
		uint8 Y = roundf(Location.Y + CanvasHeight / 2);

		if (X != CurrentMouseX || Y != CurrentMouseY)
		{
			CurrentMouseX = X;
			CurrentMouseY = Y;
			Draw();
		}
	}
}

void UCanvasEditor::Draw()
{
	DrawingCanvas->DrawDot(CursorLocation->X, CursorLocation->Y);
}

void UCanvasEditor::Bake()
{
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, TargetToRenderTo, CanvasMID);
}

void UCanvasEditor::UpdateDrawTools()
{
}

void UCanvasEditor::SetBrushRadius(uint8 InRadius)
{
}

void UCanvasEditor::SetBrushColor(FLinearColor InColor)
{
}