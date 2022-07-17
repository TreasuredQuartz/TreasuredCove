// Fill out your copywrite description in the Description Page in Project Settings

#include "PlayerInfo.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Materials/MaterialInstanceDynamic.h"


//
void UPlayerInfo::NativeConstruct()
{
	Super::NativeConstruct();

	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	// Construct Health Bar
	HealthBar =
		WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("HealthBar"));
	HealthText =
		WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HealthText"));
	Container =
		WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("PlayerInfoContainer"));

	Container->AddChild(HealthText);
	Container->AddChild(HealthBar);
	RootWidget->AddChild(Container);

	HealthMID = UMaterialInstanceDynamic::Create(HealthBarMaterial, this);
	HealthBar->SetBrushFromMaterial(HealthMID);
}

//
void UPlayerInfo::OnHealthModified(float Health, float MaxHealth)
{
	float Percent = Health/MaxHealth;
	HealthMID->SetScalarParameterValue("Alpha", Percent);

	HealthText->SetText(FText::AsNumber(roundf(Health)));
}

//
void UPlayerInfo::OnManaModified(float Mana, float MaxMana)
{

}

//
void UPlayerInfo::OnStaminaModified(float Stamina, float MaxStamina)
{

}

//
void UPlayerInfo::OnExperienceModified(float Experience, float MaxExperience)
{

}

//
void UPlayerInfo::SetDensity(float Val)
{
	Density = Val;
	HealthMID->SetScalarParameterValue("Density", Density);
}

//
void UPlayerInfo::SetRadius(float Val)
{
	Radius = Val;
	HealthMID->SetScalarParameterValue("Radius", Radius);
}

//
void UPlayerInfo::SetStartingPoint(float Val)
{
	StartingPoint = Val;
	HealthMID->SetScalarParameterValue("StartingPoint", StartingPoint);
}