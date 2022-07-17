// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTown.generated.h"

class UTownSystemComponent;

class AGameplayTownManager;
class AGameplayTownState;
class AGameplayJobSite;
class AGameplayResidence;

/** A Gameplay Town is comprised of several:
*
*	- Citizens
*	- Job Sites
*	- Residencies
*
*	A Citizen is a Resident of the town that has a Job.
*
*	The Job Sites are locations for Citizens to complete assigned Job Tasks.
*	Citizens can have multiple Jobs, and Jobs may require multiple Citizens.
*	Job Sites can also have multiple Jobs at its location.
*
*	A Residency is a location for Residents to return to after they have finished their Job Tasks.
*/
UCLASS()
class GAMEPLAYTOWNS_API AGameplayTown : 
	public AActor
{
	GENERATED_BODY()
	
	// Override function from ability system interface
	
public:	
	// Sets default values for this actor's properties
	AGameplayTown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Town System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns", meta = (AllowPrivateAccess = "true"))
	UTownSystemComponent* TownSystem;

	// 
	// TSubclassOf<AGameplayTownManager> TownManagerClass;
	// AGameplayTownManager* TownManager;

	// State of the town to spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay|Towns", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGameplayTownState> TownStateClass;
	// State of the town
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns", meta = (AllowPrivateAccess = "true"))
	AGameplayTownState* TownState;

	// Job Sites
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns|Jobs")
	TArray<AGameplayJobSite*> JobSites;

	// Residencies
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns|Residence")
	TArray<AGameplayResidence*> Residencies;

	// Everyone who has entered the town
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns")
	TArray<AActor*> Occupents;
	// Anyone in the town that works but does not live there
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns|Jobs")
	TArray<AActor*> Contractors;
	// Everyone who lives in the town
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns|Residence")
	TArray<AActor*> Residents;
	// Residents of the town who have a job
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns")
	TArray<AActor*> Citizens;
	// The max possible residents
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay|Towns|Residence")
	uint8 MaxResidents;

	void AddOccupent(AActor* NewOccupent);
	void AddContractor(AActor* NewContractor);
	void AddResident(AActor* NewResident);
	void AddCitizen(AActor* NewCitizen);

	void RemoveOccupent(AActor* OldOccupent);
	void RemoveContractor(AActor* OldContractor);
	void RemoveResident(AActor* OldResident);
	void RemoveCitizen(AActor* OldCitizen);

	//
	void CreateJobSite(AGameplayJobSite* InJobSite);
	//
	void CreateResidence(AGameplayResidence* InResidence);
	//
	void DestroyJobSite(AGameplayJobSite* InJobSite);
	//
	void DestroyResidence(AGameplayResidence* InResidence);
	//
	void AddJobSiteToTown(AGameplayJobSite* InJobSite);
	//
	void AddResidenceToTown(AGameplayResidence* InResidence);
	//
	void RemoveJobSiteFromTown(AGameplayJobSite* InJobSite);
	//
	void RemoveResidenceFromTown(AGameplayResidence* InResidence);

	void UpdateResources();
};
