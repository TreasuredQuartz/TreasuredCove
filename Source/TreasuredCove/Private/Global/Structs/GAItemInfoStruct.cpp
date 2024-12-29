// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Structs/GAItemInfoStruct.h"

FGAItemInfo::FGAItemInfo()
	: Name("")
	, Texture(nullptr)
	, EditMenuClass(nullptr)
{
}

FGAItemInfo::FGAItemInfo(FName InName, UTexture2D* inTexture, 
	TSubclassOf<UItemEditor> InItemWidgetEditMenuClass)
	: Name(InName)
	, Texture(inTexture)
	, EditMenuClass(InItemWidgetEditMenuClass)
{
}