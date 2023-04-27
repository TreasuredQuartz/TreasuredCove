// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTileData.h"

// DEFINE_ENUM_TO_STRING(EFaceOrientation);

UGameplayTileData::UGameplayTileData()
{
	// for (int i = 0; i < EFaceOrientation::Num; ++i)
	//	StaticFaces[i] = FQuad();
}

FPrimaryAssetId UGameplayTileData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("VoxelTileData", GetFName());
}


