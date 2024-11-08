// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.


#include "MapMakerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

#define RootToContentDir FMapMakerStyle::InContent
// #define RootToCoreContentDir StyleSet->RootToCoreContentDir

#include "Styling/SlateStyleMacros.h"

FString FMapMakerStyle::InContent(const FString& RelativePath, const TCHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("MapMaker"))->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FMapMakerStyle::StyleSet = NULL;
TSharedPtr< class ISlateStyle > FMapMakerStyle::Get() { return StyleSet; }

void FMapMakerStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon9x19(9.0f, 19.0f);
	const FVector2D Icon10x10(10.0f, 10.0f);
	const FVector2D Icon12x12(12.0f, 12.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon22x22(22.0f, 22.0f);
	const FVector2D Icon24x24(24.0f, 24.0f);
	const FVector2D Icon27x31(27.0f, 31.0f);
	const FVector2D Icon26x26(26.0f, 26.0f);
	const FVector2D Icon32x32(32.0f, 32.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FVector2D Icon75x82(75.0f, 82.0f);
	const FVector2D Icon360x32(360.0f, 32.0f);
	const FVector2D Icon171x39(171.0f, 39.0f);
	const FVector2D Icon170x50(170.0f, 50.0f);
	const FVector2D Icon267x140(170.0f, 50.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet("MapMakerStyle"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const FButtonStyle& BaseButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("Button");
	StyleSet->Set("MMEditor.Graph.MMNode.Clear", 
		FButtonStyle(BaseButtonStyle)
		.SetNormal(IMAGE_BRUSH("T_Trashcan", Icon16x16, FStyleColors::White))
		.SetHovered(IMAGE_BRUSH("T_Trashcan", Icon16x16, FStyleColors::Hover))
		.SetPressed(IMAGE_BRUSH("T_Trashcan", Icon16x16, FStyleColors::Header))
		.SetNormalPadding(FMargin(2, 2, 2, 2))
		.SetPressedPadding(FMargin(2, 3, 2, 1)));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
}

void FMapMakerStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

#undef RootToContentDir