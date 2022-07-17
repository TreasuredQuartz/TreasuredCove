//

#include "SPlayerInfo.h"
#include "SlateOptMacros.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Runtime/Slate/Public/SlateMaterialBrush.h"
#include "UObject/ConstructorHelpers.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

// Needed to use macro LOCTEXT
#define LOCTEXT_NAMESPACE "SPlayerInfo"

//
void SPlayerInfo::Construct(const FArguments &InArgs)
{
	Owner = InArgs._Owner;
	ImageSize = InArgs._ImageSize;
	ProgressBarMaterial = InArgs._Material;

	if (!ProgressBarMaterial)
	{
		static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("/Game/GameplayAbilities/Materials/Originals/M_UI_RadialBar"));
		if (Material.Object)
		{
			ProgressBarMaterial = Cast<UMaterialInterface>(Material.Object);
		}
	}

	ProgressBarMaterialD = UMaterialInstanceDynamic::Create(ProgressBarMaterial, Owner);
	FSlateBrush NewBrush;
	NewBrush.SetResourceObject(ProgressBarMaterialD);
	Brush = TAttribute<const FSlateBrush*>::TAttribute( &NewBrush );

	ChildSlot
		[
			SAssignNew(ProgressBar, SImage)
			.Image(Brush.Get())
		];
}

//
void SPlayerInfo::UpdateProgressBar(float Val)
{
	Progress = Val;
	FName Alpha = "Alpha";
	ProgressBarMaterialD->SetScalarParameterValue(Alpha, Progress / MaxProgress);
}

//
void SPlayerInfo::SetMaxProgressBar(float Val)
{
	MaxProgress = Val;
	FName CompleteRange = "CompleteRange";
	ProgressBarMaterialD->SetScalarParameterValue(CompleteRange, MaxProgress);
}

//
void SPlayerInfo::SetProgressBarDensity(float Val)
{
	FName Density = "Density";
	ProgressBarMaterialD->SetScalarParameterValue(Density, Val);
}

//
void SPlayerInfo::SetProgressBarStartingPoint(float Val)
{
	FName StartingPoint = "StartingPoint";
	ProgressBarMaterialD->SetScalarParameterValue(StartingPoint, Val);
}

//
void SPlayerInfo::SetProgressBarRadius(float Val)
{
	FName Radius = "Radius";
	ProgressBarMaterialD->SetScalarParameterValue(Radius, Val);
}

//
void SPlayerInfo::SetImage(TAttribute<const FSlateBrush*> InImage)
{
	if (!Brush.IdenticalTo(InImage))
	{
		Brush = InImage;
		Invalidate(EInvalidateWidget::LayoutAndVolatility);
	}
}
#undef LOCTEXT_NAMESPACE 

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
