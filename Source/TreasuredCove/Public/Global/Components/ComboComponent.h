// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"
#include "ComboComponent.generated.h"

/** Container for combo logic. Designed to be used in tandem
* with an ability system component.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UComboComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool bInCombo = false;
	uint8 ComboCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	uint8 MaxComboCount = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float EndComboCooldown = 2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float ComboContinueTime = 0.5f;

	FTimerHandle ComboTimerHandle;
	FTimerHandle EndComboTimerHandle;
public:	
	// Sets default values for this component's properties
	UComboComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	int DetermineCombo(const FGameplayTag& ComboCooldownTag);
	UFUNCTION(BlueprintCallable)
	void ComboCooldown(const FGameplayTag& CooldownTag);
	UFUNCTION(BlueprintCallable)
	void EndCombo();
};
