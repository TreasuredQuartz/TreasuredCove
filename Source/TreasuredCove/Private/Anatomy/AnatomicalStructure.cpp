// Fill out your copyright notice in the Description page of Project Settings.


#include "AnatomicalStructure.h"
#include "Anatomy/Organs/Organ.h"
#include "Anatomy/Systems/OrganSystem.h"

// Sets default values for this component's properties
UAnatomicalStructure::UAnatomicalStructure()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UAnatomicalStructure::BeginPlay()
{
	Super::BeginPlay();

	// ...
	OrganSystems.Reserve(OrganSystemClasses.Num());
	TArray<TSubclassOf<UOrgan>> OrgansToSpawn;
	for (const auto& OrganSystemClass : OrganSystemClasses)
	{
		UOrganSystem* NewSystem = NewObject<UOrganSystem>(this, OrganSystemClass);

		if (NewSystem)
		{
			OrganSystems.Add(NewSystem);
			for (const auto& OrganClass : NewSystem->GetOrganClasses()) 
				OrgansToSpawn.AddUnique(OrganClass);
		}
	}

	Organs.Reserve(OrgansToSpawn.Num());
	for (const auto& OrganClass : OrgansToSpawn)
	{
		UOrgan* NewOrgan = NewObject<UOrgan>(this, OrganClass);
	}

}

// Called every frame
//void UAnatomicalStructure::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}

template<typename T>
T* UAnatomicalStructure::GetOrganSystem() const
{
	for (uint8 i = 0; i < OrganSystems.Num(); ++i)
	{
		if (T* OrganSystem = Cast<T>(OrganSystems[i])) return OrganSystem;
	}

	return nullptr;
}

