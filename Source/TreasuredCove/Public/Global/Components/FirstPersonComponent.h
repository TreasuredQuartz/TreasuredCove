// Copyright 2018 - 2023 B. M. Biggs. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FirstPersonComponent.generated.h"

class USkeletalMesh;
class USkeletalMeshComponent;
class USpringArmComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TREASUREDCOVE_API UFirstPersonComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bFirstPerson : 1;
	uint8 bLockedViewpoint : 1;
	uint8 bDetectedCrouch : 1;

	USceneComponent* AffectedComponent;
	float DipStrength;
	float DipAlpha;
public:	
	// Sets default values for this component's properties
	UFirstPersonComponent();

	// Called when the game starts.
	void BeginPlay() override;

	// Called every frame.
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnRegister() override;
	void OnUnregister() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* FirstPersonMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ThirdPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* OwnerMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> FirstPersonAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> ThirdPersonAnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ThirdPersonArmLength = 300.f;

	UFUNCTION(BlueprintPure, Category = "Camera")
	bool IsFirstPerson() const { return bFirstPerson; };

	// Changes between first person and third person camera types
	// @TODO Add enum camera type
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ChangeViewpoint(bool bInFirstPerson);

	void CheckCrouching(const UPawnMovementComponent* InMoveComp, USkeletalMeshComponent* InMeshComp);
};
