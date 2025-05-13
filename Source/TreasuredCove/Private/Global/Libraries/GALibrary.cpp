// Fill out your copyright notice in the Description page of Project Settings.

#include "Global/Libraries/GALibrary.h"
#include "Global/Framework/GAGameState.h"
#include "TownSystem/GameTimeInterface.h"

#include "AbilitySystemComponent.h"
#include "GameplayTileData.h"

#include "Components/PointLightComponent.h"
#include "Components/RectLightComponent.h"
#include "Components/SpotLightComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/RectLight.h"
#include "Engine/SpotLight.h"
#include "Engine/Engine.h"

#include "Editor.h"
#include "EditorViewportClient.h"

bool UGALibrary::GetIsShadowed(const AActor* InActor)
{
	if (IGameTimeInterface* WorldTime = Cast<IGameTimeInterface>(UGameplayStatics::GetGameState(InActor)))				// 7 AM to 19 (7) PM is night
	{
		float CurrentTime = WorldTime->GetTimeOfDay();

		if (CurrentTime > 7 && CurrentTime < 19) // Day time
		{
			TArray<AActor*> DirectLights;
			UGameplayStatics::GetAllActorsOfClass(InActor, ADirectionalLight::StaticClass(), DirectLights);

			if (DirectLights.IsValidIndex(0))
			{
				if (AActor* Sun = DirectLights[0]) // The Sun
				{
					FVector Start;
					FVector End;
					FRotator ActorRotation;
					FRotator SunDirection = Sun->GetActorRotation();
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(InActor);
					Params.bTraceComplex = true;

					InActor->GetActorEyesViewPoint(Start, ActorRotation);

					End = Start + (SunDirection.Vector() * 999999);														// Impossibly long distance because sun is infinitely far away

					FHitResult Hit;
					if (!InActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))		// Nothing is blocking the sun
					{
						return false;
					}
					else
					{
						Start = InActor->GetActorLocation();
						End = Start + (SunDirection.Vector() * 999999);													// Impossibly long distance because sun is infinitely far away
						if (!InActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))	// Nothing is blocking the sun
						{
							return false;
						}
					}
				}
			}
		}
	}

	// We are shadowed from the sun at this point.

	TArray<AActor*> PointLights;
	TArray<AActor*> RectLights;
	TArray<AActor*> SpotLights;

	UGameplayStatics::GetAllActorsOfClass(InActor, APointLight::StaticClass(), PointLights);
	UGameplayStatics::GetAllActorsOfClass(InActor, ARectLight::StaticClass(), RectLights);
	UGameplayStatics::GetAllActorsOfClass(InActor, ASpotLight::StaticClass(), SpotLights);

	for (const AActor* PointLight : PointLights)
	{
		if (const APointLight* Light = Cast<APointLight>(PointLight))
		{
			float Distance = (Light->GetActorLocation() - InActor->GetActorLocation()).Size();
			if (Distance < Light->PointLightComponent->AttenuationRadius)
			{
				FVector Start = InActor->GetActorLocation();
				FVector End = Light->GetActorLocation();
				FCollisionQueryParams Params;
				Params.AddIgnoredActor(InActor);
				Params.bTraceComplex = true;

				FHitResult Hit;
				if (!InActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))			// Nothing is blocking the light
				{
					return false;
				}
			}
		}
	}

	for (const AActor* RectLight : RectLights)
	{
		if (const ARectLight* Light = Cast<ARectLight>(RectLight))
		{
			const FVector Direction = (Light->GetActorLocation() - InActor->GetActorLocation());
			float Distance = Direction.Size();
			if (Distance < Light->RectLightComponent->AttenuationRadius)
			{
				float Angle = FVector::DotProduct(Light->GetActorRotation().Vector(), Direction);
				if (Angle > 0 && Angle > (1 - Light->RectLightComponent->BarnDoorAngle / 90))
				{
					FVector Start = InActor->GetActorLocation();
					FVector End = Light->GetActorLocation();
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(InActor);
					Params.bTraceComplex = true;

					FHitResult Hit;
					if (!InActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))			// Nothing is blocking the light
					{
						return false;
					}
				}
			}
		}
	}

	for (const AActor* SpotLight : SpotLights)
	{
		if (const ASpotLight* Light = Cast<ASpotLight>(SpotLight))
		{
			const FVector Direction = (Light->GetActorLocation() - InActor->GetActorLocation());
			float Distance = Direction.Size();
			if (Distance < Light->SpotLightComponent->AttenuationRadius)
			{
				float Angle = FVector::DotProduct(Light->GetActorRotation().Vector(), Direction);
				if (Angle > 0 && Angle > (1 - Light->SpotLightComponent->OuterConeAngle / 90))
				{
					FVector Start = InActor->GetActorLocation();
					FVector End = Light->GetActorLocation();
					FCollisionQueryParams Params;
					Params.AddIgnoredActor(InActor);
					Params.bTraceComplex = true;

					FHitResult Hit;
					if (!InActor->GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))			// Nothing is blocking the light
					{
						return false;
					}
				}
			}
		}
	}

	// We are shadowed from all light sources at this point.

	return true;
}

void UGALibrary::GetEditorViewLocRot(FVector& OutLocation, FRotator& OutRotation)
{
	FViewport* activeViewport = GEditor->GetActiveViewport();

	FEditorViewportClient* editorViewClient = (activeViewport != nullptr) ? (FEditorViewportClient*) activeViewport->GetClient() : nullptr;

	if (editorViewClient)
	{
		OutLocation = editorViewClient->GetViewLocation();
		OutRotation = editorViewClient->GetViewRotation();
	}
}

void UGALibrary::ApplyDynamicGameplayEffect(UAbilitySystemComponent* Source, TSubclassOf<UGameplayEffect> EffectClass, FName EffectName, FGameplayAttribute Attribute, EGameplayModOp::Type ModifierType, float ModifierValue)
{
	// Create a dynamic Gameplay Effect
	UGameplayEffect* GE = NewObject<UGameplayEffect>(GetTransientPackage(), EffectClass, EffectName);

	int32 Idx = GE->Modifiers.Num();
	GE->Modifiers.SetNum(Idx + 1);

	FGameplayModifierInfo& InfoXP = GE->Modifiers[Idx];
	InfoXP.ModifierMagnitude = FScalableFloat(ModifierValue);
	InfoXP.ModifierOp = ModifierType;
	InfoXP.Attribute = Attribute;

	Source->ApplyGameplayEffectToSelf(GE, 1.0f, Source->MakeEffectContext());
}

void UGALibrary::ApplyGESpecHandleToTargetDataSpecsHandle(const FGameplayEffectSpecHandle& GESpecHandle, const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	for(TSharedPtr<FGameplayAbilityTargetData> Data : TargetDataHandle.Data)
	{
		if (Data)
		{
			Data->ApplyGameplayEffectSpec(*GESpecHandle.Data.Get());
			GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "We applyed an effect!");
		}

	}
}

bool UGALibrary::SwitchOnAttributeName(FString InAttributeName, EAbilityCostType& OutCostType)
{
	// This is the reason why I chose not to seperate attributes with Normal and Damage versions
	// This would be too much to type out
	bool bSucceeded = true;

	if (InAttributeName == "Ammo")
	{
		OutCostType = EAbilityCostType::Ammo;
	}
	else if (InAttributeName == "Experience")
	{
		OutCostType = EAbilityCostType::Experience;
	}
	else if (InAttributeName == "Health")
	{
		OutCostType = EAbilityCostType::Health;
	}
	else if (InAttributeName == "Mana")
	{
		OutCostType = EAbilityCostType::Mana;
	}
	else if (InAttributeName == "SkillPoints")
	{
		OutCostType = EAbilityCostType::SkillPoints;
	}
	else if (InAttributeName == "Spirit")
	{
		OutCostType = EAbilityCostType::Spirit;
	}
	else if (InAttributeName == "Stamina")
	{
		OutCostType = EAbilityCostType::Stamina;
	}
	else if (InAttributeName == "StatPoints")
	{
		OutCostType = EAbilityCostType::StatPoints;
	}

	else if (InAttributeName == "Bronze")
	{
		OutCostType = EAbilityCostType::Bronze;
	}
	else if (InAttributeName == "Silver")
	{
		OutCostType = EAbilityCostType::Silver;
	}
	else if (InAttributeName == "Gold")
	{
		OutCostType = EAbilityCostType::Gold;
	}
	else if (InAttributeName == "Platnum")
	{
		OutCostType = EAbilityCostType::Platnum;
	}

	else if (InAttributeName == "Point22LongRifleProperty")
	{
		OutCostType = EAbilityCostType::Point22LongRifle;
	}
	else if (InAttributeName == "Point22MagnumProperty")
	{
		OutCostType = EAbilityCostType::Point22Magnum;
	}
	else if (InAttributeName == "Point25ACPProperty")
	{
		OutCostType = EAbilityCostType::Point25ACP;
	}
	else if (InAttributeName == "Point32ACPProperty")
	{
		OutCostType = EAbilityCostType::Point32ACP;
	}
	else if (InAttributeName == "Point380ACPProperty")
	{
		OutCostType = EAbilityCostType::Point380ACP;
	}
	else if (InAttributeName == "Point38SpecialProperty")
	{
		OutCostType = EAbilityCostType::Point38Special;
	}
	else if (InAttributeName == "mm9NATOProperty")
	{
		OutCostType = EAbilityCostType::mm9NATO;
	}
	else if (InAttributeName == "Point357MagnumProperty")
	{
		OutCostType = EAbilityCostType::Point357Magnum;
	}
	else if (InAttributeName == "Point45ACPProperty")
	{
		OutCostType = EAbilityCostType::Point45ACP;
	}
	else if (InAttributeName == "Point44MagnumProperty")
	{
		OutCostType = EAbilityCostType::Point44Magnum;
	}

	else if (InAttributeName == "Point223RemingtonProperty")
	{
		OutCostType = EAbilityCostType::Point223Remington;
	}
	else if (InAttributeName == "mm39x7Point62Property")
	{
		OutCostType = EAbilityCostType::mm39x7Point62;
	}
	else if (InAttributeName == "Point30_30WinchesterProperty")
	{
		OutCostType = EAbilityCostType::Point30_30Winchester;
	}
	else if (InAttributeName == "Point308WinchesterProperty")
	{
		OutCostType = EAbilityCostType::Point308Winchester;
	}
	else if (InAttributeName == "R54x7Point62Property")
	{
		OutCostType = EAbilityCostType::R54x7Point62;
	}
	else if (InAttributeName == "Point30_06SpringfieldProperty")
	{
		OutCostType = EAbilityCostType::Point30_06Springfield;
	}

	else if (InAttributeName == "Point50BMGProperty")
	{
		OutCostType = EAbilityCostType::Point50BMG;
	}
	else if (InAttributeName == "mm102x20Property")
	{
		OutCostType = EAbilityCostType::mm102x20;
	}

	else if (InAttributeName == "Guage12Property")
	{
		OutCostType = EAbilityCostType::Guage12;
	}
	else if (InAttributeName == "Guage20Property")
	{
		OutCostType = EAbilityCostType::Guage20;
	}
	else if (InAttributeName == "Guage28Property")
	{
		OutCostType = EAbilityCostType::Guage28;
	}

	else
	{
		bSucceeded = false;
	}

	return bSucceeded;
}

bool UGALibrary::CheckAttributeCost(UAbilitySystemComponent* InAbilitySystem, FString InAttributeName, float InCost)
{
	bool bSucceeded = false;
	EAbilityCostType CostType;

	if (SwitchOnAttributeName(InAttributeName, CostType))
	{
		FGameplayAttribute Health;
		FGameplayAttribute Mana;
		FGameplayAttribute Stamina;
		FGameplayAttribute Ammo;
		FGameplayAttribute Experience;

		switch (CostType)
		{
		case EAbilityCostType::Health:
			if (InAbilitySystem->HasAttributeSetForAttribute(Health))
			{
				float CurrentAmount = InAbilitySystem->GetNumericAttribute(Health);
				if (CurrentAmount >= InCost)
				{
					bSucceeded = true;
				}
			}
			break;
		case EAbilityCostType::Mana:
			if (InAbilitySystem->HasAttributeSetForAttribute(Mana))
			{
				float CurrentAmount = InAbilitySystem->GetNumericAttribute(Mana);
				if (CurrentAmount >= InCost)
				{
					bSucceeded = true;
				}
			}
			break;
		case EAbilityCostType::Stamina:
			if (InAbilitySystem->HasAttributeSetForAttribute(Stamina))
			{
				float CurrentAmount = InAbilitySystem->GetNumericAttribute(Stamina);
				if (CurrentAmount >= InCost)
				{
					bSucceeded = true;
				}
			}
		case EAbilityCostType::Ammo:
			if (InAbilitySystem->HasAttributeSetForAttribute(Ammo))
			{
				float CurrentAmount = InAbilitySystem->GetNumericAttribute(Ammo);
				if (CurrentAmount >= InCost)
				{
					bSucceeded = true;
				}
			}
			break;
		case EAbilityCostType::Experience:
			if (InAbilitySystem->HasAttributeSetForAttribute(Experience))
			{
				float CurrentAmount = InAbilitySystem->GetNumericAttribute(Experience);
				if (CurrentAmount >= InCost)
				{
					bSucceeded = true;
				}
			}
			break;
		}
	}

	return bSucceeded;
}

float UGALibrary::GetAttributeCurrentValue(UAbilitySystemComponent* InAbilitySystem, FString InAttributeName, bool& OutDidSucceed)
{
	bool bSucceeded = false;
	float CurrentAmount = 0.f;
	EAbilityCostType CostType;

	if (SwitchOnAttributeName(InAttributeName, CostType))
	{
		FGameplayAttribute Health;
		FGameplayAttribute Mana;
		FGameplayAttribute Stamina;
		FGameplayAttribute Ammo;
		FGameplayAttribute Experience;

		switch (CostType)
		{
		case EAbilityCostType::Health:
			if (InAbilitySystem->HasAttributeSetForAttribute(Health))
			{
				CurrentAmount = InAbilitySystem->GetNumericAttribute(Health);
				bSucceeded = true;
			}
			break;
		case EAbilityCostType::Mana:
			if (InAbilitySystem->HasAttributeSetForAttribute(Mana))
			{
				CurrentAmount = InAbilitySystem->GetNumericAttribute(Mana);
				bSucceeded = true;
			}
			break;
		case EAbilityCostType::Stamina:
			if (InAbilitySystem->HasAttributeSetForAttribute(Stamina))
			{
				CurrentAmount = InAbilitySystem->GetNumericAttribute(Stamina);
				bSucceeded = true;
			}
			break;
		case EAbilityCostType::Ammo:
			if (InAbilitySystem->HasAttributeSetForAttribute(Ammo))
			{
				CurrentAmount = InAbilitySystem->GetNumericAttribute(Ammo);
				bSucceeded = true;
			}
			break;
		case EAbilityCostType::Experience:
			if (InAbilitySystem->HasAttributeSetForAttribute(Experience))
			{
				CurrentAmount = InAbilitySystem->GetNumericAttribute(Experience);
				bSucceeded = true;
			}
			break;
		}
	}

	OutDidSucceed = bSucceeded;
	return CurrentAmount;
}

//// Modifies mesh section, and returns previous triangle index num + added triangle index num (Current Triangle_Num is returned)
//int32 UGALibrary::CreateFace(FProceduralMeshSection& MeshSection, const FVoxelFaceData& VoxelData)
//{
//	const int32 Triangle_Num = VoxelData.TriangleNum;
//	const int32 InIndex = VoxelData.Index;
//	const auto& Faces = VoxelData.TileData->StaticFaces;
//	if (InIndex < 0 || InIndex > 5 || Faces[InIndex].Triangles.IsEmpty()) return Triangle_Num;
//	return Triangle_Num;
//
//	const int32 VoxelSize = VoxelData.Size;
//	const int32 Scale = FMath::Clamp<int32>(VoxelData.Scale, 1, VoxelData.Scale);
//	const FVector& InLocation = VoxelData.Origin;
//
//	// Each triangle this face has, add the vertex indices to the mesh section
//	for (const auto& Triangle : Faces[InIndex].Triangles)
//		for (int32 Index = 0; Index < 3; ++Index)
//			MeshSection.Triangles.Add(Triangle.VertIndices[Index] + Triangle_Num + MeshSection.ElementID);
//
//	// in Index represents each face of the new voxel we are generating
//	for (const auto& VertexData : Faces[InIndex].VerticesData)
//	{
//		MeshSection.Vertices.Add((VertexData.Position * VoxelSize * Scale * 0.5) + (InLocation * VoxelSize * Scale));
//		MeshSection.UVs.Add(VertexData.UVPosition * Scale);
//		MeshSection.VertexColors.Add(VertexData.Color);
//		MeshSection.Normals.Add(VertexData.Normal);
//		// Tangents.Add(VertexData.Tangent);
//	}
//
//	// Return Current Triangle Index count + The triangle indices we just added
//	return Triangle_Num + Faces[InIndex].VerticesData.Num();
//}

int32 UGALibrary::CreateFace(FProceduralMeshSection& MeshSection, const UGameplayTileData* InTileData, const FVector& InLocation, int32 InIndex, int32 CurTriangleNum, int32 InVoxelSize, int32 InScale)
{
	if (InIndex < 0 || InIndex > 5 || !InTileData) return CurTriangleNum;
	const auto& Faces = InTileData->StaticFaces;
	if (Faces[InIndex].Triangles.IsEmpty()) return CurTriangleNum;
	// return CurTriangleNum;

	// const int32 Scale = FMath::Clamp<int32>(InScale, 1, InScale);

	// Each triangle this face has, add the vertex indices to the mesh section
	for (const FTriangle& Triangle : Faces[InIndex].Triangles)
		for (int32 Index = 0; Index < 3; ++Index)
			MeshSection.Triangles.Add(Triangle.VertIndices[Index] + CurTriangleNum + MeshSection.ElementID);

	// in Index represents each face of the new voxel we are generating
	for (const FVertexData& VertexData : Faces[InIndex].VerticesData)
	{
		MeshSection.Vertices.Add((VertexData.Position * InVoxelSize * InScale * 0.5) + (InLocation * InVoxelSize));
		MeshSection.UVs.Add(VertexData.UVPosition * InScale);
		MeshSection.VertexColors.Add(VertexData.Color);
		MeshSection.Normals.Add(VertexData.Normal);
		// Tangents.Add(VertexData.Tangent);
	}

	// Return Current Triangle Index count + The triangle indices we just added
	return CurTriangleNum + Faces[InIndex].VerticesData.Num();
}

int32 UGALibrary::CreateFaceFromTransform(FProceduralMeshSection& MeshSection, const UGameplayTileData* InTileData, const FTransform& InTransform, int32 InIndex, int32 CurTriangleNum, int32 InVoxelSize)
{
	if (InIndex < 0 || InIndex > 5 || !InTileData) return CurTriangleNum;
	const auto& Faces = InTileData->StaticFaces;
	if (Faces[InIndex].Triangles.IsEmpty()) return CurTriangleNum;

	// const int32 Scale = FMath::Clamp<int32>(InScale, 1, InScale);

	// Each triangle this face has, add the vertex indices to the mesh section
	MeshSection.Triangles.Reserve(Faces[InIndex].Triangles.Num());
	for (const FTriangle& Triangle : Faces[InIndex].Triangles)
		for (int32 Index = 0; Index < 3; ++Index)
			MeshSection.Triangles.Add(Triangle.VertIndices[Index] + CurTriangleNum + MeshSection.ElementID);

	int32 VertNum = Faces[InIndex].VerticesData.Num();
	MeshSection.Vertices.Reserve(VertNum);
	MeshSection.UVs.Reserve(VertNum);
	MeshSection.Normals.Reserve(VertNum);
	MeshSection.VertexColors.Reserve(VertNum);
	MeshSection.Tangents.Reserve(VertNum);
	for (auto& VertexData : Faces[InIndex].VerticesData)
	{
		// FQuat Angle = FQuat::ToAxisAndAngle(FVector::UpVector, InTransform.GetRotation().GetAngle());
		FVector NewVert = (VertexData.Position * InVoxelSize * InTransform.GetScale3D()); 
		FVector RotVert = InTransform.GetRotation().RotateVector(UKismetMathLibrary::InverseTransformLocation(FTransform(FRotator(90, 0, 0), FVector::ZeroVector), NewVert));
		FVector NewNorm = InTransform.GetRotation().RotateVector(UKismetMathLibrary::InverseTransformLocation(FTransform(FRotator(90, 0, 0), FVector::ZeroVector), -VertexData.Normal )).GetSafeNormal();
		// FVector NewNorm = RotVert.GetSafeNormal();
		FVector NewNormAxisA;
		FVector NewNormAxisB;
		NewNorm.FindBestAxisVectors(NewNormAxisA, NewNormAxisB);

		MeshSection.Vertices.Add(RotVert + (InTransform.GetLocation() * InVoxelSize));
		MeshSection.UVs.Add(VertexData.UVPosition);
		MeshSection.Normals.Add(NewNorm);
		MeshSection.VertexColors.Add(VertexData.Color);
		MeshSection.Tangents.Add(NewNormAxisA);
	}

	// Return Current Triangle Index count + The triangle indices we just added
	return CurTriangleNum + VertNum;
}
