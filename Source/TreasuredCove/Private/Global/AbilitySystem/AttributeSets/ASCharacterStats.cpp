// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/AbilitySystem/AttributeSets/ASCharacterStats.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

#include "Gameframework/Character.h"
#include "Gameframework/CharacterMovementComponent.h"

UASCharacterStats::UASCharacterStats() :
	MaxGroundMovementSpeed(500)
{

}

void UASCharacterStats::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	bool bModifiedAttributeIsGroundMovement = Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UASCharacterStats::StaticClass(), GET_MEMBER_NAME_CHECKED(UASCharacterStats, MaxGroundMovementSpeed));

	if (bModifiedAttributeIsGroundMovement)
	{
		if (GetOwningActor())
		{
			if (ACharacter* Character = Cast<ACharacter>(GetOwningActor()))
			{
				if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
				{
					MovementComponent->MaxWalkSpeed = MaxGroundMovementSpeed.GetCurrentValue();
				}
			}
		}
	}
}
