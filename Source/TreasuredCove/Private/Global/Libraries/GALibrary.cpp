// Fill out your copyright notice in the Description page of Project Settings.

#include "GALibrary.h"

#include "AbilitySystemComponent.h"

#include "Editor.h"
#include "EditorViewportClient.h"
#include "Engine/Engine.h"

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

void UGALibrary::ApplyGESpecHandleToTargetDataSpecsHandle(const FGameplayEffectSpecHandle& GESpecHandle, const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	for(TSharedPtr<FGameplayAbilityTargetData> Data : TargetDataHandle.Data)
	{
		Data->ApplyGameplayEffectSpec(*GESpecHandle.Data.Get());

		// GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "We applyed an effect!");
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
