// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "UObject/NoExportTypes.h"
#include "TextureCreator.generated.h"

class UTextureRenderTarget2D;
class UMaterialInterface;

UCLASS(Blueprintable, BlueprintType)
class TREASUREDCOVE_API UTextureCreator : public UObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UTextureCreator();

protected:

public:
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Canvas")
	UTexture2D* DynamicCanvas;

	/* Canvas Variables */

	//
	std::unique_ptr<uint8[]> CanvasPixelData;
	int CanvasWidth;
	int CanvasHeight;
	int BytesPerPixel;
	int BufferPitch;
	int BufferSize;

	std::unique_ptr<FUpdateTextureRegion2D> EchoUpdateTextureRegion;
	void SetPixelColor(uint8*& Pointer, uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha);

	/* Drawing Tools Variables */

	//
	std::unique_ptr<uint8[]> CanvasBrushMask;
	int Radius;
	int BrushBufferSize;

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void InitializeCanvas(const int32 PixelsH, const int32 PixelsV);

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void UpdateCanvas();

	//
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void ClearCanvas();

	//
	UFUNCTION(BlueprintCallable, Category = "Drawing Tools")
	void InitializeDrawingTools(const int32 BrushRadius);

	//
	UFUNCTION(BlueprintCallable, Category = "Drawing Tools")
	void DrawDot(const int32 PixelCoordX, const int32 PixelCoordY);
};
