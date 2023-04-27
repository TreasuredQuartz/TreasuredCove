// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Libraries/DrawingLibrary.h"

UTexture2D* UDrawingLibrary::CreateTextureFromArray(const TArray<FColor>& Colors)
{
    UTexture2D* NewTexture = NewObject<UTexture2D>();

    if (NewTexture)
    {
        int32 TextureWidth = 32;
        int32 TextureHeight = 32;

        uint8* Pixels = new uint8[TextureHeight * TextureWidth * 4];
        int32 i = 0;
        for (int32 y = 0; y < TextureHeight; ++y)
        {
            for (int32 x = 0; x < TextureHeight; ++x)
            {
                Pixels[4 * i] = Colors[i].R;
                Pixels[4 * i + 1] = Colors[i].G;
                Pixels[4 * i + 2] = Colors[i].B;
                Pixels[4 * i + 3] = Colors[i].A;

                ++i;
            }
        }

        FTexturePlatformData* Data = new FTexturePlatformData();

        NewTexture->SetPlatformData(Data);
        Data->SizeX = TextureWidth;
        Data->SizeY = TextureHeight;
        Data->SetNumSlices(1);
        Data->PixelFormat = EPixelFormat::PF_R8G8B8A8;

        FTexture2DMipMap* Mip = new FTexture2DMipMap();

        // Make mip map
        Data->Mips.Add(Mip);
        Mip->SizeX = TextureWidth;
        Mip->SizeY = TextureHeight;

        //Lock the mipmap data so it can be modified
        Mip->BulkData.Lock(LOCK_READ_WRITE);
        uint8* TextureData = (uint8*)Mip->BulkData.Realloc(TextureHeight * TextureWidth * 4);

        //Copy the pixel data into the Texture data
        FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * TextureHeight * TextureWidth * 4);
        Mip->BulkData.Unlock();

        NewTexture->Source.Init(TextureWidth, TextureHeight, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);
        NewTexture->UpdateResource();

        delete[] Pixels;
    }

    return NewTexture;
}
