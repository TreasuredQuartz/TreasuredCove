// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"

/*
 *
 */
class FMapMakerStyle
{
public:

	static void Initialize();
	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

private:

	static FString InContent(const FString& RelativePath, const TCHAR* Extension);

private:

	static TSharedPtr< class FSlateStyleSet > StyleSet;
};