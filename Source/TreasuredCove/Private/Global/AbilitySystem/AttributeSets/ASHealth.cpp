// Fill out your copyright notice in the Description page of Project Settings.

#include "ASHealth.h"
#include "HealthComponent.h"

#include "AbilitySystemComponent.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Engine/Engine.h"

UASHealth::UASHealth()
	: MaxHealth(10.f)
	, Health(10.f)
	, MaxStamina(10.f)
	, Stamina(10.f)
	, MaxMana(10.f)
	, Mana(10.f)
	, Defense(0.f)
	, DefenseMultiplier(1.f)
{
}

void UASHealth::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Get all property members here that should be checked after modification
	FProperty* HealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Health));
	FProperty* StaminaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Stamina));
	FProperty* ManaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Mana));
	FProperty* MaxHealthProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxHealth));
	FProperty* MaxStaminaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxStamina));
	FProperty* MaxManaProperty = FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxMana));

	// Now we need to increment through all property members that could be modified
	if (Attribute == HealthProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
		// UE_LOG(LogTemp, Warning, TEXT("Health Value:%f"), NewValue);

		UHealthComponent* HealthComp = Cast<UHealthComponent>(GetOwningActor()->GetComponentByClass(UHealthComponent::StaticClass()));
		if (HealthComp)
		{
			if (Health.GetCurrentValue() == MaxHealth.GetCurrentValue())
			{
				HealthComp->AddFullHealthTag();
			}
			else
			{
				HealthComp->RemoveFullHealthTag();
			}
		}
	}
	else if (Attribute == StaminaProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	}
	else if (Attribute == ManaProperty)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxMana.GetCurrentValue());
	}
	else if (Attribute == MaxHealthProperty)
	{
		NewValue = FMath::Clamp(NewValue, Health.GetCurrentValue(), NewValue);
	}
	else if (Attribute == MaxStaminaProperty)
	{
		NewValue = FMath::Clamp(NewValue, Stamina.GetCurrentValue(), NewValue);
	} 
	else if (Attribute == MaxManaProperty)
	{
		NewValue = FMath::Clamp(NewValue, Mana.GetCurrentValue(), NewValue);
	}
}

bool UASHealth::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	Super::PreGameplayEffectExecute(Data);

	// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "PreGameplayEffectExecute");

	bool bModifiedAttributeIsMaxHealth = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxHealth));
	bool bModifiedAttributeIsHealth = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Health));

	bool bModifiedAttributeIsMaxStamina = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxStamina));
	bool bModifiedAttributeIsStamina = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Stamina));

	bool bModifiedAttributeIsMaxMana = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, MaxMana));
	bool bModifiedAttributeIsMana = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UASHealth, Mana));

	// Is the attribute being modified is one of our attributes
	if (bModifiedAttributeIsHealth || bModifiedAttributeIsMaxHealth || bModifiedAttributeIsStamina || bModifiedAttributeIsMaxStamina || bModifiedAttributeIsMana || bModifiedAttributeIsMaxMana)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "An Attribute was modified");

		bool bDamagedHealth = Data.EvaluatedData.Magnitude < Health.GetCurrentValue();
		bool bDamagedMaxHealth = Data.EvaluatedData.Magnitude < MaxHealth.GetCurrentValue();

		bool bDamagedStamina = Data.EvaluatedData.Magnitude < Stamina.GetCurrentValue();
		bool bDamagedMaxStamina = Data.EvaluatedData.Magnitude < MaxStamina.GetCurrentValue();

		bool bDamagedMana = Data.EvaluatedData.Magnitude < Mana.GetCurrentValue();
		bool bDamagedMaxMana = Data.EvaluatedData.Magnitude < MaxMana.GetCurrentValue();

		// Is the attribute affected by the gameplay effect less than our current attribute 
		// (IE: Damaging)
		if (bDamagedMaxHealth || bDamagedHealth || bDamagedMaxStamina || bDamagedStamina  || bDamagedMaxMana || bDamagedMana)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "An Attribute was damaged");

			bool bHasDamageImmunity = Data.EffectSpec.CapturedSourceTags.GetActorTags().HasTag(FGameplayTag::RequestGameplayTag("immunity.damage"));

			// Do we have a tag that makes us immune to Damage
			// return bHasDamageImmunity; // The below explains what occurs based on this value
			if (bHasDamageImmunity)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Cannot Damage");

				return false;
				// Because we are immune to damage, and this gameplay effect would damage us
				// , we do not allow this effect to apply.
			}
			else
			{
				// We are not immune to damage, therefore this effect applies to us.
				if (bDamagedHealth)
				{
					// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage Health");

					AttributeType = EAttributeType::Health;
					DeltaAmount = 0 - Data.EvaluatedData.Magnitude;
				}
				else if (bDamagedStamina)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage Stamina");

					AttributeType = EAttributeType::Stamina;
					DeltaAmount = Stamina.GetCurrentValue() - Data.EvaluatedData.Magnitude;
				}
				else if (bDamagedMana)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage Mana");

					AttributeType = EAttributeType::Mana;
					DeltaAmount = Mana.GetCurrentValue() - Data.EvaluatedData.Magnitude;
				}
				else if (bDamagedMaxHealth)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage MaxHealth");

					AttributeType = EAttributeType::MaxHealth;
					DeltaAmount = MaxHealth.GetCurrentValue() - Data.EvaluatedData.Magnitude;
				}
				else if (bDamagedMaxStamina)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage MaxStamina");

					AttributeType = EAttributeType::MaxStamina;
					DeltaAmount = MaxStamina.GetCurrentValue() - Data.EvaluatedData.Magnitude;
				}
				else if (bDamagedMaxMana)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Damage MaxMana");

					AttributeType = EAttributeType::MaxMana;
					DeltaAmount = MaxMana.GetCurrentValue() - Data.EvaluatedData.Magnitude;
				}

				bDamaged = true;
				bHealed = false;
				return true;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "An Attribute was healed");

			// The Attribute affected is more than our current Attribute 
			// (IE: Healing)
			bool bHasHealingImmunity = Data.EffectSpec.CapturedSourceTags.GetActorTags().HasTag(FGameplayTag::RequestGameplayTag("immunity.heal"));
			
			if (bHasHealingImmunity)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Cannot heal");

				return false;
			}
			else
			{
				bool bIsHealth = Data.EvaluatedData.Magnitude > Health.GetCurrentValue();
				bool bIsStamina = Data.EvaluatedData.Magnitude > Stamina.GetCurrentValue();
				bool bIsMana = Data.EvaluatedData.Magnitude > Mana.GetCurrentValue();

				if (bIsHealth)
				{
					AttributeType = EAttributeType::Health;
					DeltaAmount = Data.EvaluatedData.Magnitude - Health.GetCurrentValue();
				}
				else if (bIsStamina)
				{
					AttributeType = EAttributeType::Stamina;
					DeltaAmount = Data.EvaluatedData.Magnitude - Stamina.GetCurrentValue();
				}
				else if (bIsMana)
				{
					AttributeType = EAttributeType::Mana;
					DeltaAmount = Data.EvaluatedData.Magnitude - Mana.GetCurrentValue();
				}

				bDamaged = false;
				bHealed = true;
				return true;
			}
		}
	}
	else
	{
		AttributeType = EAttributeType::MaxHealth;
		DeltaAmount = 0;
		bDamaged = false;
		bHealed = false;
	}

	return true;
}

void UASHealth::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData &Data)
{
	Super::PostGameplayEffectExecute(Data);

	// GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "PostGameplayEffectExecute");

	UAbilitySystemComponent* SourceAbilitySystem = Data.EffectSpec.GetContext().GetInstigatorAbilitySystemComponent();

	if (SourceAbilitySystem)
	{
		AActor* SourceActor = SourceAbilitySystem->GetOwner();
		UHealthComponent* HealthComp = Cast<UHealthComponent>(GetOwningActor()->GetComponentByClass(UHealthComponent::StaticClass()));

		if (SourceActor)
		{
			switch (AttributeType)
			{
			case EAttributeType::Health:
				// UE_LOG(LogTemp, Warning, TEXT("Health Modified event was broadcasted!"));
				OnHealthModified.Broadcast(Health.GetCurrentValue(), MaxHealth.GetCurrentValue());
				break;
			case EAttributeType::Stamina:
				OnStaminaModified.Broadcast(Stamina.GetCurrentValue(), MaxStamina.GetCurrentValue());
				break;
			case EAttributeType::Mana:
				OnManaModified.Broadcast(Mana.GetCurrentValue(), MaxMana.GetCurrentValue());
				break;
			}

			if (bDamaged)
			{
				float DamageRecieved = DeltaAmount;
				FOnHealthDamagedResult Result = FOnHealthDamagedResult(Health.GetCurrentValue(), DamageRecieved, FName("None"), GetOwningActor(), SourceActor, NewObject<UDamageType>());
				HealthComp->OnHealthDamaged.Broadcast(Result);
				if (Health.GetCurrentValue() == 0) HealthComp->Die(GetOwningActor(), SourceActor);
				// OnDamaged.Broadcast(SourceActor, AttributeType, DeltaAmount, Data.EvaluatedData.Magnitude);

				/*AGACharacter* OwningCharacter = Cast<AGACharacter>(GetOwningActor());

				if (OwningCharacter)
				{
					OwningCharacter->OnDamaged(SourceActor, AttributeType, DeltaAmount, Data.EvaluatedData.Magnitude);
				} 

				UE_LOG(LogTemp, Warning, TEXT("Damage event was broadcasted!"));*/
			}
			else if (bHealed)
			{
				OnHealed.Broadcast(SourceActor, AttributeType, DeltaAmount, Data.EvaluatedData.Magnitude); 
				/*AGACharacter* SourceCharacter = Cast<AGACharacter>(GetOwningActor());

				if (SourceCharacter)
				{
					SourceCharacter->OnHealed(SourceActor, AttributeType, DeltaAmount, Data.EvaluatedData.Magnitude);
				}

				UE_LOG(LogTemp, Warning, TEXT("Heal event was broadcasted!"));*/
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Both are false; which is weird");
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "SourceActor is null");
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "Source AbilitySystem is null");
	}
}
