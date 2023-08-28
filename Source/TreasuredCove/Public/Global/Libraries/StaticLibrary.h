

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BulletStruct.h"
#include "ItemKeyStruct.h"
#include "StaticLibrary.generated.h"

UCLASS()
class TREASUREDCOVE_API UStaticLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Mass Of Components", CompactNodeTitle = "Mass", Keywords = "mass"), Category = "Physics")
	static float GetSummedMassOfComponents(const AActor* Actor);

	static FName GetTagLeafName(const struct FGameplayTag& Tag);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FBulletStruct", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
	static bool EqualEqual_FBulletStructFBulletStruct(const FBullet& arg1, const FBullet& arg2);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FBulletStruct", CompactNodeTitle = "==", Keywords = "== equal"), Category = "Math")
	static bool EqualEqual_FItemKeyFItemKey(const FItemKey& arg1, const FItemKey& arg2);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Find FItemKey", CompactNodeTitle = "FIND", Keywords = "find"), Category = "Arrays")
	static int Find_FItemKeyFItemKey(const TArray<FItemKey> Array, const FItemKey ItemToFind);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Add FItemKey", CompactNodeTitle = "ADD", Keywords = "+ Add Addition add addition"), Category = "Math")
	static FItemKey Addition_FItemKeyFItemKey(const FItemKey Item1, const FItemKey Item2);

	/** Get Current Planet through Game State. Null if none or invalid game state cast. */
	static TObjectPtr<UObject> GetCurrentPlanet(TObjectPtr<UObject> CallingObject);
};