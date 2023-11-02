// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingCanvas.h"

UDrawingCanvas::UDrawingCanvas()
{
	//canvasWidth = 1024;
	//canvasHeight = 1024;
	//int32 InSizeX = canvasWidth;
	//int32 InSizeY = canvasHeight;

	//UTexture2D* NewTexture = NewObject<UTexture2D>(
	//	GetTransientPackage(),
	//	FName(),
	//	RF_Transient
	//	);

	//EPixelFormat InFormat = EPixelFormat::PF_R8G8B8A8;
	//NewTexture->PlatformData = new FTexturePlatformData();
	//NewTexture->PlatformData->SizeX = InSizeX;
	//NewTexture->PlatformData->SizeY = InSizeY;
	//NewTexture->PlatformData->PixelFormat = InFormat;

	//// Allocate first mipmap.
	//int32 NumBlocksX = InSizeX / GPixelFormats[InFormat].BlockSizeX;
	//int32 NumBlocksY = InSizeY / GPixelFormats[InFormat].BlockSizeY;
	//FTexture2DMipMap* Mip = new FTexture2DMipMap();
	//NewTexture->PlatformData->Mips.Add(Mip);
	//Mip->SizeX = InSizeX;
	//Mip->SizeY = InSizeY;
	//Mip->BulkData.Lock(LOCK_READ_WRITE);
	//Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[InFormat].BlockBytes);
	//Mip->BulkData.Unlock();

	//dynamicCanvas = NewTexture;
}

UDrawingCanvas::~UDrawingCanvas()
{
	canvasBrushMask.Reset();
	canvasPixelData.Reset();
	echoUpdateTextureRegion.Reset();
}

void UDrawingCanvas::InitializeCanvas(const int32 pixelsH, const int32 pixelsV)
{
	// dynamic Texture initialization
	canvasWidth = pixelsH;
	canvasHeight = pixelsV;

	dynamicCanvas = UTexture2D::CreateTransient(canvasWidth, canvasHeight);
#if WITH_EDITORONLY_DATA
	dynamicCanvas->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	dynamicCanvas->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	dynamicCanvas->SRGB = 0;
	dynamicCanvas->Filter = TextureFilter::TF_Nearest;
	dynamicCanvas->UpdateResource();

	echoUpdateTextureRegion = TUniquePtr<FUpdateTextureRegion2D>(new FUpdateTextureRegion2D(0,0,0,0, canvasWidth, canvasHeight));

	// buffers intialization
	bytesPerPixel = 4;
	bufferPitch = canvasWidth * bytesPerPixel;
	bufferSize = canvasWidth * canvasHeight * bytesPerPixel;
	canvasPixelData = TUniquePtr<uint8[]>(new uint8[bufferSize]);

	ClearCanvas();
}

void UDrawingCanvas::InitializeCanvasFromTexture(UTexture2D* InTexture)
{
	//dynamicCanvas = InTexture;

	uint8* canvasPixelPtr = canvasPixelData.Get();
	FTexture2DMipMap* myMipMap = &InTexture->GetPlatformData()->Mips[0];
	FBulkData* RawImageData = &myMipMap->BulkData;

	canvasWidth = myMipMap->SizeX;
	canvasHeight = myMipMap->SizeY;

	const FColor* FormattedImageData = static_cast<const FColor*>(RawImageData->Lock( LOCK_READ_ONLY ));

	for (int32 x = 0; x < canvasWidth; x++)
	for (int32 y = 0; y < canvasHeight; y++)
	{
		uint8 index = (x + y * canvasWidth) * bytesPerPixel;
		// Formatted Image Data can also be casted to a uint8 array (*)
		/*FColor PixelColor = FColor(FormattedImageData[index], 
			FormattedImageData[index + 1],
			FormattedImageData[index + 2],
			FormattedImageData[index + 3]);*/
		FColor PixelColor = FormattedImageData[index];

		canvasPixelPtr += index;
		setPixelColor(canvasPixelPtr, PixelColor.R, PixelColor.G, PixelColor.B, PixelColor.A);
	}

	RawImageData->Unlock();
	InTexture->UpdateResource();

	UpdateCanvas();
}

void UDrawingCanvas::UpdatePsuedoVolume(int Size, float* density)
{
	uint8* canvasPixelPtr = canvasPixelData.Get();
	int SizeSQ = Size * Size;

	for (int32 z = 0; z < Size; z++)
	for (int32 y = 0; y < Size; y++)
	for (int32 x = 0; x < Size; x++)
	{
		uint8 p_index = x + y * Size + z * SizeSQ;
		uint8 c_index = p_index * bytesPerPixel;

		canvasPixelPtr += c_index;
		*density += p_index;

		setPixelColor(canvasPixelPtr, *density, *density, *density, *density);
	}

	UpdateCanvas();
}

void UDrawingCanvas::setPixelColor(uint8*& pointer, uint8 red, uint8 green, uint8 blue, uint8 alpha)
{
	*pointer = blue;
	*(pointer + 1) = green;
	*(pointer + 2) = red;
	*(pointer + 3) = alpha;
}

void UDrawingCanvas::SetCanvasFromIntArray(TArray<int> intArray)
{
	uint8* canvasPixelPtr = canvasPixelData.Get();

	for (int i = 0; i < canvasWidth * canvasHeight; i++)
	{
		if (i > intArray.Num()) break;

		if (intArray.IsValidIndex(i))
		{
			setPixelColor(canvasPixelPtr, intArray[i],   intArray[i], intArray[i], intArray[i]);
		}

		canvasPixelPtr += bytesPerPixel;
	}

	UpdateCanvas();
}

void UDrawingCanvas::SetCanvasFromColorArray(TArray<FColor> colorArray)
{
	uint8* canvasPixelPtr = canvasPixelData.Get();

	for (int i = 0; i < canvasWidth * canvasHeight; i++)
	{
		if (i > colorArray.Num()) break;

		if (colorArray.IsValidIndex(i))
		{
			setPixelColor(canvasPixelPtr, colorArray[i].R, colorArray[i].G, colorArray[i].B, colorArray[i].A);
		}

		canvasPixelPtr += bytesPerPixel;
	}

	UpdateCanvas();
}

void UDrawingCanvas::ClearCanvas()
{
	uint8* canvasPixelPtr = canvasPixelData.Get();
	for (int i = 0; i < canvasWidth * canvasHeight; i++)
	{
		setPixelColor(canvasPixelPtr, 0, 0, 0, 0); // Blank
		canvasPixelPtr += bytesPerPixel;
	}

	UpdateCanvas();
}

void UDrawingCanvas::UpdateCanvas()
{
	if (echoUpdateTextureRegion)
		dynamicCanvas->UpdateTextureRegions((int32)0, (uint32)1, echoUpdateTextureRegion.Get(), (uint32)bufferPitch, (uint32)bytesPerPixel, canvasPixelData.Get());
}

void UDrawingCanvas::InitializeDrawingTools(const int32 BrushRadius, const FColor BrushColor)
{
	radius = BrushRadius;
	brushBufferSize = radius * radius * 4 * bytesPerPixel;
	canvasBrushMask = TUniquePtr<uint8[]>(new uint8[brushBufferSize]);
	uint8* canvasBrushPixelPtr = canvasBrushMask.Get();
	for(int py = -radius; py < radius; ++py)
	for(int px = -radius; px < radius; ++px)
	{
		int32 tx = px + radius;
		int32 ty = py + radius;
		canvasBrushPixelPtr = canvasBrushMask.Get() + (tx + ty * 2 * radius) * bytesPerPixel;
		if (px * px + py * py < radius * radius)
			setPixelColor(canvasBrushPixelPtr, BrushColor.R, BrushColor.G, BrushColor.B, BrushColor.A);
		else
			setPixelColor(canvasBrushPixelPtr, 0, 0, 0, 0); // Blank
	}
}

void UDrawingCanvas::DrawDot(const int32 pixelCoordX, const int32 pixelCoordY)
{
	uint8* canvasPixelPtr = canvasPixelData.Get();
	const uint8* canvasBrushPixelPtr = canvasBrushMask.Get();
	for(int py = -radius; py < radius; ++py)
	for(int px = -radius; px < radius; ++px)
	{
		int32 tbx = px + radius;
		int32 tby = py + radius;
		canvasBrushPixelPtr = canvasBrushMask.Get() + (tbx + tby * 2 * radius) * bytesPerPixel;

		if (*(canvasBrushPixelPtr + 3) == 255) // Only paint if Brush mask is visible
		{
			int32 tx = pixelCoordX + px;
			int32 ty = pixelCoordY + py;
			if ((tx >= 0 && tx < canvasWidth) && (ty >= 0 && ty < canvasHeight))
			{
				canvasPixelPtr = canvasPixelData.Get() + (tx + ty * canvasWidth) * bytesPerPixel;
				setPixelColor(canvasPixelPtr, *(canvasBrushPixelPtr + 2), *(canvasBrushPixelPtr + 1), *(canvasBrushPixelPtr), *(canvasBrushPixelPtr + 3));
			}
		}
	}

	UpdateCanvas();
}

void UDrawingCanvas::DrawSquare(const int32 pixelCoordX, const int32 pixelCoordY)
{
	uint8* canvasPixelPtr = canvasPixelData.Get();
	const uint8* canvasBrushPixelPtr = canvasBrushMask.Get();
	for (int py = -radius; py < radius; ++py)
		for (int px = -radius; px < radius; ++px)
		{
			int32 tbx = px + radius;
			int32 tby = py + radius;
			canvasBrushPixelPtr = canvasBrushMask.Get() + (tbx + tby * 2 * radius) * bytesPerPixel;

			if (*(canvasBrushPixelPtr + 3) == 255) // Only paint if Brush mask is visible
			{
				int32 tx = pixelCoordX + px;
				int32 ty = pixelCoordY + py;
				if ((tx >= 0 && tx < canvasWidth) && (ty >= 0 && ty < canvasHeight))
				{
					canvasPixelPtr = canvasPixelData.Get() + (tx + ty * canvasWidth) * bytesPerPixel;
					setPixelColor(canvasPixelPtr, *(canvasBrushPixelPtr + 2), *(canvasBrushPixelPtr + 1), *(canvasBrushPixelPtr), *(canvasBrushPixelPtr + 3));
				}
			}
		}

	UpdateCanvas();
}