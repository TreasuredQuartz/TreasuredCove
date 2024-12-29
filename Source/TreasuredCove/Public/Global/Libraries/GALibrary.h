// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Global/Structs/AbilityInfoStruct.h"
#include "Global/Structs/ProceduralMeshSectionStruct.h"
#include "GALibrary.generated.h"

class UGameplayTileData;

USTRUCT(BlueprintType)
struct TREASUREDCOVE_API FVoxelFaceData
{
	GENERATED_BODY()

public:
	const UGameplayTileData* TileData;
	FVector Origin;
	int32 Size;
	int32 Scale;
	int32 Index;
	int32 TriangleNum;

public:
	FVoxelFaceData()
		: TileData(nullptr)
		, Origin(FVector::ZeroVector)
		, Size(1)
		, Scale(1)
		, Index(-1)
		, TriangleNum(0) {};

	FVoxelFaceData(const UGameplayTileData* InTileData, const FVector& InLocation, int32 InIndex, int32 CurTriangleNum, int32 InVoxelSize, int32 InScale)
		: TileData(InTileData)
		, Origin(FVector(InLocation))
		, Size(InVoxelSize)
		, Scale(InScale)
		, Index(InIndex)
		, TriangleNum(CurTriangleNum) {};
};

/**
 * 
 */
UCLASS()
class TREASUREDCOVE_API UGALibrary : public UAbilitySystemBlueprintLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "AI")
	static bool GetIsShadowed(const AActor* InActor);

	UFUNCTION(BlueprintCallable, Category = "Editor")
	static void GetEditorViewLocRot(FVector& OutLocation, FRotator& OutRotation);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	static void ApplyDynamicGameplayEffect(UAbilitySystemComponent* Source, TSubclassOf<UGameplayEffect> EffectClass, FName EffectName, FGameplayAttribute Attribute, EGameplayModOp::Type ModifierType, float ModifierValue);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	static void ApplyGESpecHandleToTargetDataSpecsHandle(const FGameplayEffectSpecHandle& GESpecHandle, const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static bool SwitchOnAttributeName(FString InAttributeName, EAbilityCostType& OutCostType);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static bool CheckAttributeCost(class UAbilitySystemComponent* InAbilitySystem, FString InAttributeName, float Cost);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static float GetAttributeCurrentValue(class UAbilitySystemComponent* InAbilitySystem, FString InAttributeName, bool& OutDidSucceed);

	/*static int32 CreateFace(FProceduralMeshSection& MeshSection, const FVoxelFaceData& VoxelData);
	UFUNCTION(BlueprintCallable, Category = "Voxel")*/
	UFUNCTION(BlueprintCallable, Category = "Voxel")
	static int32 CreateFace(FProceduralMeshSection& MeshSection, const UGameplayTileData* InTileData, const FVector& InLocation, int32 InIndex, int32 CurTriangleNum, int32 InVoxelSize, int32 InScale);

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	static int32 CreateFaceFromTransform(FProceduralMeshSection& MeshSection, const UGameplayTileData* InTileData, const FTransform& InTransform, int32 InIndex, int32 CurTriangleNum, int32 InVoxelSize);
};
