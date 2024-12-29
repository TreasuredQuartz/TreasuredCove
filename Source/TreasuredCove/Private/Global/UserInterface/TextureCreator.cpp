// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/UserInterface/TextureCreator.h"

// Sets default values
UTextureCreator::UTextureCreator()
{
	
}

//
void UTextureCreator::InitializeCanvas(const int32 PixelsH, const int32 PixelsV)
{
	//
	CanvasWidth = PixelsH;
	CanvasHeight = PixelsV;
	DynamicCanvas = UTexture2D::CreateTransient(CanvasWidth, CanvasHeight);
#if WITH_EDITORONLY_DATA
	DynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	DynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicCanvas->SRGB = 1;
	DynamicCanvas->AddToRoot();
	DynamicCanvas->Filter = TextureFilter::TF_Nearest;
	DynamicCanvas->UpdateResource();

	EchoUpdateTextureRegion = std::unique_ptr<FUpdateTextureRegion2D>(new FUpdateTextureRegion2D(0, 0, 0, 0, CanvasWidth, CanvasHeight));

	//
	BytesPerPixel = 4;
	BufferPitch = CanvasWidth * BytesPerPixel;
	BufferSize = CanvasWidth * CanvasHeight * BytesPerPixel;
	CanvasPixelData = std::unique_ptr<uint8[]>(new uint8[BufferSize]);

	ClearCanvas();
}

//
void UTextureCreator::SetPixelColor(uint8*& Pointer, uint8 Red, uint8 Green, uint8 Blue, uint8 Alpha)
{
	*Pointer = Blue;
	*(Pointer + 1) = Green;
	*(Pointer + 2) = Red;
	*(Pointer + 3) = Alpha;
}

//
void UTextureCreator::ClearCanvas()
{
	uint8* CanvasPixelPtr = CanvasPixelData.get();
	for (int i = 0; i < CanvasWidth*CanvasHeight; i++)
	{
		SetPixelColor(CanvasPixelPtr, 0, 0, 0, 0);
		CanvasPixelPtr += BytesPerPixel;
	}
	UpdateCanvas();
}

//
void UTextureCreator::UpdateCanvas()
{
	if (EchoUpdateTextureRegion)
	{
		DynamicCanvas->UpdateTextureRegions((uint32)0, (uint32)1, EchoUpdateTextureRegion.get(), (uint32)BufferPitch, (uint32)BytesPerPixel, CanvasPixelData.get());
	}
}

//
void UTextureCreator::InitializeDrawingTools(const int32 BrushRadius)
{
	Radius = BrushRadius;
	BrushBufferSize = Radius * Radius * 4 * BytesPerPixel;
	CanvasBrushMask = std::unique_ptr<uint8[]>(new uint8[BrushBufferSize]);
	uint8* CanvasBrushPixelPtr = CanvasBrushMask.get();
	for (int px = -Radius; px < Radius; px++)
	{
		for (int py = -Radius; py < Radius; py++)
		{
			int32 tx = px + Radius;
			int32 ty = py + Radius;
			CanvasBrushPixelPtr = CanvasBrushMask.get() + (tx + ty * 2 * Radius) * BytesPerPixel;
			if (px * px + py * py < Radius*Radius)
			{
				SetPixelColor(CanvasBrushPixelPtr, 0, 0, 0, 255);
			}
			else
			{
				SetPixelColor(CanvasBrushPixelPtr, 0, 0, 0, 0);
			}
		}
	}
}

//
void UTextureCreator::DrawDot(const int32 PixelCoordX, const int32 PixelCoordY)
{
	uint8* CanvasPixelPtr = CanvasPixelData.get();
	const uint8* CanvasBrushPixelPtr = CanvasBrushMask.get();
	for (int px = -Radius; px < Radius; px++)
	{
		for (int py = -Radius; py < Radius; py++)
		{
			int32 tbx = px + Radius;
			int32 tby = py + Radius;
			CanvasBrushPixelPtr = CanvasBrushMask.get() + (tbx + tby * 2 * Radius) * BytesPerPixel;
			if (*(CanvasBrushPixelPtr + 3) == 255)
			{
				int32 tx = PixelCoordX;
				int32 ty = PixelCoordY;
				if (tx >= 0 && tx < CanvasWidth && ty >= 0 && ty < CanvasHeight)
				{
					CanvasPixelPtr = CanvasPixelData.get() + (tx + ty * CanvasWidth) * BytesPerPixel;
					SetPixelColor(CanvasPixelPtr, *(CanvasBrushPixelPtr + 2), *(CanvasBrushPixelPtr + 1), *(CanvasBrushPixelPtr), *(CanvasBrushPixelPtr + 3));
				}
			}
		}
	}

	UpdateCanvas();
}
