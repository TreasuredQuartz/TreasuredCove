// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Components/Characters/FloatingTextComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "Global/Actors/FloatingTextActor.h"

// Sets default values for this component's properties
UFloatingTextComponent::UFloatingTextComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UFloatingTextComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UFloatingTextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (!GetOwner<APawn>()->IsLocallyControlled())
		return;

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(this);

	// Reject bad viewport sizes
	if (ViewportSize.X <= 0.0f || ViewportSize.Y <= 0.0f)
		return;

	const int32 viewportX = FGenericPlatformMath::FloorToInt(ViewportSize.X);
	const int32 viewportY = FGenericPlatformMath::FloorToInt(ViewportSize.Y);

	const float ViewportScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportX, viewportY));

	// Reject bad viewport scales
	if (ViewportScale <= 0.0f)
		return;

	const auto* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!PlayerCameraManager)
		return;

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();

	// Adjust active text actors' locations based on viewport size, scale, and current DPI scaling.
	for (int32 i = 1; i < ActiveTextActors.Num(); ++i)
	{
		if (ActiveTextActors[i]->DoesSelfUpdateLocation()) continue;

		// Calculate Offset between text actors
		AFloatingTextActor* text = ActiveTextActors[i];
		const float distance = FVector::Dist(CameraLocation, text->GetAnchorLocation());
		const float verticalOffset = distance / (ViewportSize.X / ViewportScale);

		text->SetActorLocation(text->GetAnchorLocation() + FVector(0.0f, 0.0f, i * verticalOffset * TextVerticalSpacing));
	}
}

void UFloatingTextComponent::AddFloatingText_Client_Implementation(const FText& Text, const FVector& WorldLocation)
{
	// We need the class specified before we can spawn text actors!
	if (!FloatingTextActorClass)
		return;

	const FTransform spawnTransform = FTransform(FRotator::ZeroRotator, WorldLocation + FVector(0.0f, 0.0f, TextVerticalOffset));
	AFloatingTextActor* newTextActor = GetWorld()->SpawnActorDeferred<AFloatingTextActor>(FloatingTextActorClass, spawnTransform, GetOwner());

	// Possible to get bad spawns and this checks that we did in fact spawn
	if (!newTextActor)
		return;

	newTextActor->OnDestroyed.AddDynamic(this, &UFloatingTextComponent::OnTextDestroyed);
	UGameplayStatics::FinishSpawningActor(newTextActor, spawnTransform);

	newTextActor->Initialize(Text);
	ActiveTextActors.Insert(newTextActor, 0);

	UE_LOG(LogTemp, Warning, TEXT("Spawned Floating Text Actor"));
}

void UFloatingTextComponent::OnTextDestroyed(AActor* DestroyedActor)
{
	ActiveTextActors.Pop();
}

void UFloatingTextComponent::DestroyAllTexts()
{
	for (AFloatingTextActor* Actor : ActiveTextActors)
	{
		if (Actor) Actor->Destroy();
	}

	ActiveTextActors.Empty();
}
