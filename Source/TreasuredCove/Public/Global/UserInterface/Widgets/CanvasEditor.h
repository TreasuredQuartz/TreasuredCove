// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/GAWidget.h"
#include "CanvasEditor.generated.h"

class UTextureRenderTarget2D;
class UTextureCreator;

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UCanvasEditor : public UGAWidget
{
	GENERATED_BODY()

public:
	FVector2D* CursorLocation;
	//
	virtual void NativeConstruct() override;

	//
	virtual void ConfirmPressed() override;

	//
	virtual void ConfirmReleased() override;

	//
	virtual void OnCursorMove(FVector2D Location) override;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	UMaterialInterface* CanvasMaterial;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	UTextureRenderTarget2D* TargetToRenderTo;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	UTextureCreator* DrawingCanvas;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	UMaterialInstanceDynamic* CanvasMID;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	uint8 CanvasWidth;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	uint8 CanvasHeight;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	uint8 BrushRadius;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Canvas")
	FLinearColor BrushColor;

	//
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Canvas")
	void InitializeDrawingCanvas();
private:
	//
	FVector2D MouseLocation;

	//
	uint8 CurrentMouseX;

	//
	uint8 CurrentMouseY;

	//
	bool bShouldDraw = false;

	//
	UFUNCTION(BlueprintCallable, Category= "Canvas")
	void Draw();

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void Bake();

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void UpdateDrawTools();

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void SetBrushColor(FLinearColor InColor);

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void SetBrushRadius(uint8 InRadius);
};
