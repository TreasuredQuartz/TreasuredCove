// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UniquePtr.h"
#include "DrawingCanvas.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TREASUREDCOVE_API UDrawingCanvas : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "DrawingCanvas")
	UTexture2D* GetTexture()
	{
		return dynamicCanvas;
	}

	UFUNCTION(BlueprintPure, Category = "DrawingCanvas")
	UTexture2D* DuplicateTexture()
	{
		return DuplicateObject<UTexture2D>(dynamicCanvas, this);
	}

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas")
	void InitializeCanvas(const int32 pixelsH, const int32 pixelsV);

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas")
	void InitializeCanvasFromTexture(UTexture2D* InTexture);

	void UpdatePsuedoVolume(int Size, float* density);

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas")
	void UpdateCanvas();

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas")
	void ClearCanvas();

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas|Tools")
	void InitializeDrawingTools(const int32 brushRadius, const FColor BrushColor);

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas|Tools")
	void DrawDot(const int32 pixelCoordX, const int32 pixelCoordY);

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas|Tools")
	void DrawSquare(const int32 pixelCoordX, const int32 pixelCoordY);

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas")
	void SetCanvasFromIntArray(TArray<int> intArray);

	UFUNCTION(BlueprintCallable, Category = "DrawingCanvas")
	void SetCanvasFromColorArray(TArray<FColor> ColorArray);

	UDrawingCanvas();
	~UDrawingCanvas();

private:
	UTexture2D* dynamicCanvas;

	// draw brush tool
	TUniquePtr<uint8[]> canvasBrushMask;
	int radius;
	int brushBufferSize;

	// canvas
	TUniquePtr<uint8[]> canvasPixelData;
	int canvasWidth;
	int canvasHeight;
	int bytesPerPixel;
	int bufferPitch;
	int bufferSize;

	TUniquePtr<FUpdateTextureRegion2D> echoUpdateTextureRegion;
	void setPixelColor(uint8*& pointer, uint8 red, uint8 green, uint8 blue, uint8 alpha);
};
