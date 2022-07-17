// Fill out your copyright notice in the Description page of Project Settings.

#include "GAItemInfoStruct.h"

FGAItemInfo::FGAItemInfo()
	: ItemActor(nullptr)
	, Name("")
	, Texture(nullptr)
	, ActiveMenuClass(nullptr)
	, EditMenuClass(nullptr)
{
}

FGAItemInfo::FGAItemInfo(AActor* InItemActor, FName InName, UTexture2D* inTexture, 
	TSubclassOf<UCommandMenu> InItemWidgetActiveMenuClass, TSubclassOf<UItemEditor> InItemWidgetEditMenuClass)
	: ItemActor(InItemActor)
	, Name(InName)
	, Texture(inTexture)
	, ActiveMenuClass(InItemWidgetActiveMenuClass)
	, EditMenuClass(InItemWidgetEditMenuClass)
{
}