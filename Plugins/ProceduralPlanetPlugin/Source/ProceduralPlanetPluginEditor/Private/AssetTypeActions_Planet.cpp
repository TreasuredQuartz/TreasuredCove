// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_Planet.h"
#include "PlanetEditor.h"

#include "ProceduralPlanet.h"



FText FAssetTypeActions_Planet::GetName() const
{
	return FText::FromString("Planet");
}

FColor FAssetTypeActions_Planet::GetTypeColor() const
{
	return FColor::Black;
}

UClass* FAssetTypeActions_Planet::GetSupportedClass() const
{
	return AProceduralPlanet::StaticClass();
}

void FAssetTypeActions_Planet::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Object : InObjects)
	{
		auto NewAsset = Cast<AProceduralPlanet>(Object);
		if (NewAsset != nullptr)
		{
			TSharedRef< FPlanetEditor > Editor(new FPlanetEditor());
			Editor->Init(Mode, EditWithinLevelEditor, NewAsset);
		}
	}
}

uint32 FAssetTypeActions_Planet::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}
