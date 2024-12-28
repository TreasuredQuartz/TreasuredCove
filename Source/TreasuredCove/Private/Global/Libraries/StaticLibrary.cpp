// Blah, Blah, Blah Copywrite notice etc.





#include "Global/Libraries/StaticLibrary.h"
#include "GameplayTagContainer.h"
#include "PhysicsEngine/BodySetup.h"

float UStaticLibrary::GetSummedMassOfComponents(const AActor* Actor)
{
	float TotalMass = 0.f;
	if (Actor == nullptr) return TotalMass; // Guard against null values

	TInlineComponentArray<UPrimitiveComponent*> Comps;
	Actor->GetComponents(Comps);
	for (const UPrimitiveComponent* PrimComp : Comps)
	{
		TotalMass += PrimComp->GetMass();
	}

	return TotalMass;
}

FName UStaticLibrary::GetTagLeafName(const struct FGameplayTag& Tag)
{
	FString TagNameAsString = Tag.ToString();

	FString Left;
	FString Right;

	if (TagNameAsString.Split(FString(TEXT(".")), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		return FName(*Right);
	}
	else
	{
		return Tag.GetTagName();
	}
}

bool UStaticLibrary::EqualEqual_FBulletStructFBulletStruct(const FBullet& arg1, const FBullet& arg2)
{
	return arg1 == arg2;
}

bool UStaticLibrary::EqualEqual_FItemKeyFItemKey(const FItemKey& arg1, const FItemKey& arg2)
{
	return arg1 == arg2;
}

int UStaticLibrary::Find_FItemKeyFItemKey(const TArray<FItemKey> Array, const FItemKey ItemToFind)
{
	int value = -1;

	for (int i = 0; i < Array.Num(); i++)
	{
		if (Array[i] == ItemToFind)
		{
			value = i;
			break;
		}
	}

	return value;
}

FItemKey UStaticLibrary::Addition_FItemKeyFItemKey(const FItemKey Item1, const FItemKey Item2)
{
	return Item1 + Item2;
}

TObjectPtr<UObject> UStaticLibrary::GetCurrentPlanet(TObjectPtr<UObject> CallingObject)
{
	if (CallingObject == nullptr) return nullptr;

	return nullptr;
}

