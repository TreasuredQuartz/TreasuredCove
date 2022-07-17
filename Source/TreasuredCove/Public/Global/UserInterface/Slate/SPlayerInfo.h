// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SBaseWidget.h"

class SImage;
class UObject;
class UMaterialInterface;
class UMaterialInstanceDynamic;

/**
*
*/
class TREASUREDCOVE_API SPlayerInfo :
	public SBaseWidget
{
public:
	SLATE_BEGIN_ARGS(SPlayerInfo)
	{}
	SLATE_ARGUMENT(UObject*, Owner)

	SLATE_ARGUMENT(UMaterialInterface*, Material)

	SLATE_ARGUMENT(const FVector2D*, ImageSize)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments &InArgs);
private:
	//
	UObject* Owner;
public:
	// The radial bar
	TSharedPtr<SImage> ProgressBar;

	// The radial Material
	UMaterialInterface* ProgressBarMaterial;

	// The dynamic version of our material
	UMaterialInstanceDynamic* ProgressBarMaterialD;

	//
	const FVector2D* ImageSize;

	// the brush we use
	TAttribute<const FSlateBrush*> Brush;

	// this is meant to determine the amount that the material is filled based on a value
	float Progress;

	// this is the maximum amount that the material can be filled
	float MaxProgress;

	//
	void SetImage(TAttribute<const FSlateBrush*> InImage);

	//
	void UpdateProgressBar(float Val);

	//
	void SetMaxProgressBar(float Val);

	//
	void SetProgressBarDensity(float Val);

	//
	void SetProgressBarStartingPoint(float Val);

	//
	void SetProgressBarRadius(float Val);
};

