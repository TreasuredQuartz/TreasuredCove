// Fill out your copyright notice in the Description page of Project Settings.

#include "SBaseWidget.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

// Needed to use macro LOCTEXT
#define LOCTEXT_NAMESPACE "SBaseWidget"

void SBaseWidget::Construct(const FArguments &InArgs)
{

}

// Undefine before finishing widget
#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION


