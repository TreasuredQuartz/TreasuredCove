// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Global/Structs/FluidCubeStruct.h"
#include "GAFluidSimulator.generated.h"

#define FluidCube FFluidCubeStruct

UCLASS()
class TREASUREDCOVE_API AGAFluidSimulator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGAFluidSimulator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//
	class UDrawingCanvas* myCanvas;
	FFluidCubeStruct myCube;
	bool bIsCanvasInitialized;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Simulation", Meta = (ExposeOnSpawn = true))
	int FluidSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Simulation", Meta = (ExposeOnSpawn = true))
	int FluidDiffusion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Simulation", Meta = (ExposeOnSpawn = true))
	int FluidViscosity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Simulation", Meta = (ExposeOnSpawn = true))
	float FluidDT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fluid Simulation")
	bool bIsSimulating;

	UMaterialInstanceDynamic* AdvectColorMID;
	UMaterialInstanceDynamic* AdvectMID;
	UMaterialInstanceDynamic* DivergenceMID;
	UMaterialInstanceDynamic* PressureIterationMID;
	UMaterialInstanceDynamic* GradientSubtractionMID;

	int PressureIndex;

	//UFUNCTION(BlueprintCallable, Category="Fluid Simulation")
	void SimulationStep_GPU();
	void Advect_GPU(UTextureRenderTarget2D* SourceVelocity, UTextureRenderTarget2D* SourceTexture, UTextureRenderTarget2D* DestinationTexture, bool bIsVelocity, UTextureRenderTarget2D* ColorTexture);
	void Divergence_GPU(UTextureRenderTarget2D* SourceTexture, UTextureRenderTarget2D* DestinationTexture);
	void PressureIteration_GPU(UTextureRenderTarget2D* Pressure, UTextureRenderTarget2D* ScratchRT, UTextureRenderTarget2D* Divergence, float Divisor, int Iterations, FLinearColor UVScaleAndOffset, FVector2D RTSize, FVector2D XYFrames);
	void GradientSubtraction_GPU(UTextureRenderTarget2D* SourceVelocity, UTextureRenderTarget2D* SourcePressureGradient, UTextureRenderTarget2D* DestinationVelocity);
	void BindRenderTargetsToMIDs();
	void Swap_Render_Targets(UTextureRenderTarget2D* RT0, UTextureRenderTarget2D* RT1);

	// Modern UE4 related Code
	UFUNCTION(BlueprintCallable, Category = "Fluid Simulation")
	FFluidCubeStruct FluidCubeCreate(int size, int diffusion, int viscosity, float dt);
	UFUNCTION(BlueprintCallable, Category = "Fluid Simulation")
	void FluidCubeFree(FFluidCubeStruct cube);
	UFUNCTION(BlueprintCallable, Category = "Fluid Simulation")
	void FluidCubeAddDensity(FFluidCubeStruct cube, int x, int y, int z, float amount);
	UFUNCTION(BlueprintCallable, Category = "Fluid Simulation")
	void FluidCubeAddVelocity(FFluidCubeStruct cube, int x, int y, int z, float amountX, float amountY, float amountZ);
	UFUNCTION(BlueprintCallable, Category = "Fluid Simulation")
	void FluidCubeStep(FFluidCubeStruct cube);

	// Original C Code
	void add_source(int Size, float* x, float* s, float dt);
	/*static */void set_bnd(int Size, int b, float* x);
	void lin_solve(int Size, int b, float* x, float* xO, float c, int iter);
	/*static */void diffuse(int Size, int b, float* density, float* density_previous, float diff, float dt, int iter);
	/*static */void project(int Size, float* u, float* v, float* w, float* p, float* div, int iter);
	/*static */void advect(int Size, int b, float* d, float* d0, float* u, float* v, float* w, float dt);
	void dens_step(int Size, float* x, float* xO, float* u, float* v, float* w, float diff, float dt);
	void vel_step(int Size, float* u, float* v, float* w, float* uO, float* vO, float* wO, float visc, float dt);
	void draw_dens(int Size, float* density);

	// Balance between UE4 and C Code
	UFUNCTION(BlueprintPure, Category = "Fluid Simulation")
	UTexture2D* GetTexture();
	void CreatePsuedoVolumeTextureFromDensity(int SizeType, float* density);
	void UpdatePsuedoVolumeTextureFromDensity(int Size, float* density);
};
