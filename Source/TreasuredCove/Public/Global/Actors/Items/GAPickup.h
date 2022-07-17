// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupInterface.h"
#include "GAPickup.generated.h"

class UBoxComponent;

/** Pickups are items that are added to a repository.
* These are generally represented by a texture and a quantity
* in the User Interface, however they also have a mesh 
* representation in the level.
*/
UCLASS(Blueprintable)
class TREASUREDCOVE_API AGAPickup : 
	public AActor,
	public IPickupInterface
{
	GENERATED_BODY()
private:
	bool bBeingPickedUp = false;

public:	
	// Sets default values for this actor's properties
	AGAPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Overlap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemKey PickupInfo;
public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

	bool OnCanBePickedUp_Implementation(AActor* OtherActor) override;
	bool CanBePickedUp_Implementation(AActor* OtherActor) override;
	void OnPickedUp_Implementation(AActor* OtherActor) override;
	FString GetPickupCategory_Implementation() override
	{
		return Category;
	}
	FName GetPickupName_Implementation() override
	{
		return PickupInfo.Name;
	}
	int32 GetPickupQuantity_Implementation() override
	{
		return PickupInfo.Quantity;
	}
	UStaticMeshComponent* GetPickupMesh_Implementation() override
	{
		return Mesh;
	}
};
