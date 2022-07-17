// Fill out your copyright notice in the Description page of Project Settings.


#include "GAFluidSimulator.h"
#include "DrawingCanvas.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"

// Sets default values
AGAFluidSimulator::AGAFluidSimulator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGAFluidSimulator::BeginPlay()
{
	Super::BeginPlay();
	
	myCanvas = NewObject<UDrawingCanvas>(this, UDrawingCanvas::StaticClass());
	myCube = FluidCubeCreate(FluidSize, FluidDiffusion, FluidViscosity, FluidDT);
}

// Called every frame
void AGAFluidSimulator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FluidDT = DeltaTime;

	if (bIsSimulating)
	{
		//get_from_UI(density_previous, u_previous, v_previous);
		//vel_step(Size, u, v, w, u_previous, v_previous, w_previous, diff, dt);
		//dens_step(Size, density, density_previous, u, v, w, diff, dt);
		//draw_dens(Size, density);
	}
}

void AGAFluidSimulator::SimulationStep_GPU()
{

}

void AGAFluidSimulator::Advect_GPU(UTextureRenderTarget2D* SourceVelocity, UTextureRenderTarget2D* SourceTexture, UTextureRenderTarget2D* DestinationTexture, bool bIsVelocity, UTextureRenderTarget2D* ColorTexture)
{
	UMaterialInstanceDynamic* LocAdvectMID = bIsVelocity ? AdvectMID : AdvectColorMID;
	LocAdvectMID->SetTextureParameterValue(FName("Color"), ColorTexture);
	LocAdvectMID->SetTextureParameterValue(FName("Velocity"), SourceVelocity);
	LocAdvectMID->SetTextureParameterValue(FName("Source"), SourceTexture);
	LocAdvectMID->SetScalarParameterValue(FName("IsVelocity"), (float)(bIsVelocity));
	UKismetRenderingLibrary::ClearRenderTarget2D(this, DestinationTexture);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, DestinationTexture, LocAdvectMID);
}

void AGAFluidSimulator::Divergence_GPU(UTextureRenderTarget2D* SourceTexture, UTextureRenderTarget2D* DestinationTexture)
{
	DivergenceMID->SetTextureParameterValue(FName("Velocity"), SourceTexture);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, DestinationTexture, DivergenceMID);
}

void AGAFluidSimulator::PressureIteration_GPU(UTextureRenderTarget2D* Pressure, UTextureRenderTarget2D* ScratchRT, UTextureRenderTarget2D* Divergence, float Divisor, int Iterations, FLinearColor UVScaleAndOffset, FVector2D RTSize, FVector2D XYFrames)
{
	int i;

	PressureIterationMID->SetVectorParameterValue(FName("UVScaleAndOffset"), UVScaleAndOffset);
	PressureIterationMID->SetVectorParameterValue(FName("XYFrames"), FLinearColor(XYFrames.X, XYFrames.Y, 0, 1));
	PressureIterationMID->SetVectorParameterValue(FName("Resolution"), FLinearColor(RTSize.X, RTSize.Y, 0, 1));
	PressureIterationMID->SetScalarParameterValue(FName("Divisor"), Divisor);
	PressureIterationMID->SetVectorParameterValue(FName("SampleXYFrames"), FLinearColor(XYFrames.X, XYFrames.Y, 0, 1));

	for (i = 0; i < Iterations; i++)
	{
		PressureIndex++;
		PressureIterationMID->SetTextureParameterValue(FName("Divergence"), Divergence);
		PressureIterationMID->SetTextureParameterValue(FName("Pressure"), Pressure);

		UCanvas* Canvas;
		FVector2D Size;
		FDrawToRenderTargetContext Context;
		UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, ScratchRT, Canvas, Size, Context);
		Canvas->K2_DrawMaterial(PressureIterationMID, FVector2D(), RTSize, FVector2D(), FVector2D(1, 1));
		UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
		Swap_Render_Targets(Pressure, ScratchRT);
	}
}

void AGAFluidSimulator::GradientSubtraction_GPU(UTextureRenderTarget2D* SourceVelocity, UTextureRenderTarget2D* SourcePressureGradient, UTextureRenderTarget2D* DestinationVelocity)
{
	GradientSubtractionMID->SetTextureParameterValue(FName("Velocity"), SourceVelocity);
	GradientSubtractionMID->SetTextureParameterValue(FName("Pressure"), SourcePressureGradient);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, DestinationVelocity);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, DestinationVelocity, GradientSubtractionMID);
}

void AGAFluidSimulator::Swap_Render_Targets(UTextureRenderTarget2D* RT0, UTextureRenderTarget2D* RT1)
{
	UTextureRenderTarget2D* TmpRT = RT0;
	RT0 = RT1;
	RT1 = TmpRT;
}

#define IX(x, y, z) ((x) + (y * Size) + (z * Size * Size))
#define SWAP(x0, x) {float* tmp=x0;x0=x;x=tmp;}

FluidCube AGAFluidSimulator::FluidCubeCreate(int size, int diffusion, int viscosity, float dt)
{
	FluidCube cube = FFluidCubeStruct();

	int N = size;

	cube.size = size;
	cube.dt = dt;
	cube.diff = diffusion;
	cube.visc = viscosity;

	cube.s = (float*)calloc(N * N * N, sizeof(float));
	cube.density = (float*)calloc(N * N * N, sizeof(float));

	cube.Vx = (float*)calloc(N * N * N, sizeof(float));
	cube.Vy = (float*)calloc(N * N * N, sizeof(float));
	cube.Vz = (float*)calloc(N * N * N, sizeof(float));

	cube.VxO = (float*)calloc(N * N * N, sizeof(float));
	cube.VyO = (float*)calloc(N * N * N, sizeof(float));
	cube.VzO = (float*)calloc(N * N * N, sizeof(float));

	return cube;
}

void AGAFluidSimulator::FluidCubeFree(FluidCube cube)
{
	free(cube.s);
	free(cube.density);

	free(cube.Vx);
	free(cube.Vy);
	free(cube.Vz);

	free(cube.VxO);
	free(cube.VyO);
	free(cube.VzO);

	myCanvas->BeginDestroy();
}

void AGAFluidSimulator::FluidCubeAddDensity(FluidCube cube, int x, int y, int z, float amount)
{
	int Size = cube.size;
	cube.density[IX(x, y, z)] += amount;
}

void AGAFluidSimulator::FluidCubeAddVelocity(FluidCube cube, int x, int y, int z, float amountX, float amountY, float amountZ)
{
	int Size = cube.size;
	int index = IX(x, y, z);

	cube.Vx[index] += amountX;
	cube.Vy[index] += amountY;
	cube.Vz[index] += amountZ;
}

void AGAFluidSimulator::add_source(int Size, float* x, float* s, float dt)
{
	int i, size = (Size + 2) * (Size + 2);

	for (i = 0; i < size; i++) x[i] += dt*s[i];
}

void AGAFluidSimulator::dens_step(int Size, float* density, float* density_previous, float* u, float* v, float* w, float diff, float dt)
{
	add_source(Size, density, density_previous, dt);
	SWAP(density_previous, density); diffuse(Size, 0, density, density_previous, diff, dt, 20);
	SWAP(density_previous, density); advect(Size, 0, density, density_previous, u, v, w, dt);
}

void AGAFluidSimulator::vel_step(int Size, float* u, float* v, float* w, float* u_previous, float* v_previous, float* w_previous, float viscosity, float dt)
{
	add_source(Size, u, u_previous, dt); add_source(Size, v, v_previous, dt); add_source(Size, w, w_previous, dt);
	SWAP(u_previous, u); diffuse(Size, 1, u, u_previous, viscosity, dt, 20);
	SWAP(v_previous, v); diffuse(Size, 2, v, v_previous, viscosity, dt, 20);
	SWAP(w_previous, w); diffuse(Size, 3, w, w_previous, viscosity, dt, 20);
	project(Size, u, v, w, u_previous, v_previous, 20);
	SWAP(u_previous, u); SWAP(v_previous, v); SWAP(w_previous, w);
	advect(Size, 1, u, u_previous, u_previous, v_previous, w_previous, dt); advect(Size, 2, v, v_previous, u_previous, v_previous, w_previous, dt); advect(Size, 3, w, w_previous, u_previous, v_previous, w_previous, dt);
	project(Size, u, v, w, u_previous, v_previous, 20);
}

/*static */void AGAFluidSimulator::set_bnd(int Size, int direction, float* x)
{
	int i, j;

	for (j = 1; j <= Size; j++)
	{
		for (i = 1; i <= Size; i++)
		{
			x[IX(0, i, j)] = direction == 1 ? -x[IX(1, i, j)] : x[IX(1, i, j)];
			x[IX(Size + 1, i, j)] = direction == 1 ? -x[IX(Size, i, j)] : x[IX(Size, i, j)];

			x[IX(i, 0, j)] = direction == 2 ? -x[IX(i, 1, j)] : x[IX(i, 1, j)];
			x[IX(i, Size + 1, j)] = direction == 2 ? -x[IX(i, Size, j)] : x[IX(i, Size, j)];

			x[IX(i, j, 0)] = direction == 3 ? -x[IX(i, j, 1)] : x[IX(i, j, 1)];
			x[IX(i, j, Size + 1)] = direction == 3 ? -x[IX(i, j, Size)] : x[IX(i, j, Size)];
		}
	}

	// Left Front Bottom
	x[IX(0, 0, 0)] = 0.33f *(
		x[IX(1, 0, 0)] +
		x[IX(0, 1, 0)] +
		x[IX(0, 0, 1)]);

	// Right Front Bottom
	x[IX(Size + 1, 0, 0)] = 0.33f *(
		x[IX(Size, 0, 0)] +
		x[IX(Size + 1, 1, 0)] +
		x[IX(Size + 1, 0, 1)]);

	// Left Back Bottom
	x[IX(0, Size + 1, 0)] = 0.33f *(
		x[IX(1, Size + 1, 0)] +
		x[IX(0, Size, 0)] +
		x[IX(0, Size + 1, 1)]);

	// Left Front Top
	x[IX(0, 0, Size + 1)] = 0.33f *(
		x[IX(1, 0, Size + 1)] +
		x[IX(0, 1, Size + 1)] +
		x[IX(0, 0, Size)]);

	// Left Back Top
	x[IX(0, Size + 1, Size + 1)] = 0.33f *(
		x[IX(1, Size + 1, Size + 1)] +
		x[IX(0, Size, Size + 1)] +
		x[IX(0, Size + 1, Size)]);

	
	// Right Back Bottom
	x[IX(Size + 1, Size + 1, 0)] = 0.33f *(
		x[IX(Size, Size + 1, 0)] +
		x[IX(Size + 1, Size, 0)] +
		x[IX(Size + 1, Size + 1, 1)]);

	// Right Front Top
	x[IX(Size + 1, 0, Size + 1)] = 0.33f *(
		x[IX(Size, 0, Size + 1)] +
		x[IX(Size + 1, 1, Size + 1)] +
		x[IX(Size + 1, 0, Size)]);

	// Right Back Top
	x[IX(Size + 1, Size + 1, Size + 1)] = 0.33f *(
		x[IX(Size, Size + 1, Size + 1)] +
		x[IX(Size + 1, Size, Size + 1)] +
		x[IX(Size + 1, Size + 1, Size)]);
}

void AGAFluidSimulator::lin_solve(int Size, int direction, float* x, float* xO, float c, int iter)
{
	float cRecip = 1.0 / c;

	int i, j, k, m;

	for (k = 0; k < iter; k++)
	{
		for (m = 1; m < Size - 1; m++)
			for (j = 1; j < Size - 1; j++)
				for (i = 1; i < Size - 1; i++)
				{
					x[IX(i, j, m)] =
						(xO[IX(i, j, m)] +
						x[IX(i - 1, j, m)] +
						x[IX(i + 1, j, m)] +
						x[IX(i, j - 1, m)] +
						x[IX(i, j + 1, m)] +
						x[IX(i, j, m - 1)] +
						x[IX(i, j, m + 1)]
						) * cRecip;
				}
		set_bnd(Size, direction, x);
	}
}

/*static */void AGAFluidSimulator::diffuse(int Size, int direction, float* x, float* xO, float diff, float dt, int iter)
{
	int i, j, k, l;
	float a = dt * diff * Size * Size;

	for (l = 0; l < iter; l++)
	{
		for (k = 0; k <= Size; k++)
		{
			for (i = 1; i <= Size; i++)
			{
				for (j = 1; j <= Size; j++)
				{
					x[IX(i, j, k)] = (xO[IX(i, j, k)] + a * (
						x[IX(i - 1, j, k)] + x[IX(i + 1, j, k)] +
						x[IX(i, j - 1, k)] + x[IX(i, j + 1, k)] +
						x[IX(i, j, k - 1)] + x[IX(i, j, k + 1)])
						) / (1 + 6 * a);
				}
			}
		}
		set_bnd(Size, direction, x);
	}
}

/*static */void AGAFluidSimulator::project(int Size, float* u, float* v, float* w, float* p, float* div, int iter)
{
	int i, j, k;
	float h;

	h = 1.0 / Size;
	for (k = 1; k < Size - 1; k++)
	for (j = 1; j < Size - 1; j++)
	for (i = 1; i < Size - 1; i++)
	{
		// Setting previous velocity
		div[IX(i, j, k)] = -0.5f * h * (
			u[IX(i + 1, j, k)] - u[IX(i - 1, j, k)] +
			v[IX(i, j + 1, k)] - v[IX(i, j - 1, k)] +
			w[IX(i, j, k + 1)] - w[IX(i, j, k - 1)]
			);
		// Setting
		p[IX(i, j, k)] = 0;
	}

	set_bnd(Size, 0, div);
	set_bnd(Size, 0, p);
	lin_solve(Size, 0, p, div, 6, iter);

	// 
	for (k = 1; k < Size - 1; k++)
	for (j = 1; j < Size - 1; j++)
	for (i = 1; i < Size - 1; i++)
	{
		u[IX(i, j, k)] -= 0.33f * (
			p[IX(i - 1, j, k)] -
			p[IX(i + 1, j, k)]
			) * Size;

		v[IX(i, j, k)] -= 0.33f * (
			p[IX(i, j - 1, k)] -
			p[IX(i, j + 1, k)]
			) * Size;

		w[IX(i, j, k)] -= 0.33f * (
			p[IX(i, j, k - 1)] -
			p[IX(i, j, k + 1)]
			) * Size;
	}

	set_bnd(Size, 1, u);
	set_bnd(Size, 2, v);
	set_bnd(Size, 3, w);
}

/*static */void AGAFluidSimulator::advect(int Size, int direction, float* d, float* d0, float* u, float* v, float* w, float dt)
{
	int i, i0, i1, j, j0, j1, k, k0, k1;

	float x, y, z, s0, s1, t0, t1, u0, u1, dt0;

	dt0 = dt*Size;

	for (k = 1; k <= Size; k++)
	{
		for (j = 1; j <= Size; j++)
		{
			for (i = 1; i <= Size; i++)
			{
				x = i - dt0 * u[IX(i,j,k)]; 
				y = j - dt0 * v[IX(i,j,k)];
				z = k - dt0 * w[IX(i,j,k)];

				if (x < 0.5f) x = 0.5f;
				if (x > Size + 0.5f) x = Size + 0.5f;

				i0 = (int)x;
				i1 = i0 + 1;

				if (y < 0.5f) y = 0.5f;
				if (y > Size + 0.5f) y = Size + 0.5f;

				j0 = (int)y;
				j1 = j0 + 1;

				if (z < 0.5f) z = 0.5f;
				if (z > Size + 0.5f) z = Size + 0.5f;

				k0 = (int)z;
				k1 = k0 + 1;

				s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1; u1 = z - k0; u0 = 1 - u1;

				d[IX(i, j, k)] =
					(
						s0 * (t0 * (u0 * d0[IX(i0, j0, k0)] + (u1 * d0[IX(i0, j0, k1)]))) + 
							(t1 * (u0 * d0[IX(i0, j1, k0)] + u1 * d0[IX(i0, j1, k1)]))
					) 
						+
					(
						s1 * (t0 * (u0 * d0[IX(i1, j0, k0)] + (u1 * d0[IX(i1, j0, k1)]))) + 
							(t1 * (u0 * d0[IX(i1, j1, k0)] + u1 * d0[IX(i1, j1, k1)]))
					);
			}
		}
	}
	set_bnd(Size, direction, d);
}

void AGAFluidSimulator::FluidCubeStep(FluidCube cube)
{
	int N = cube.size;
	float visc = cube.visc;
	float diff = cube.diff;
	float dt = cube.dt;

	float* Vx = cube.Vx;
	float* Vy = cube.Vy;
	float* Vz = cube.Vz;

	float* VxO = cube.VxO;
	float* VyO = cube.VyO;
	float* VzO = cube.VzO;

	float* s = cube.s;
	float* density = cube.density;

	diffuse(N, 1, VxO, Vx, visc, dt, 4);
	diffuse(N, 2, VxO, Vx, visc, dt, 4);
	diffuse(N, 3, VxO, Vx, visc, dt, 4);

	project(N, VxO, VyO, VzO, Vx, Vy, 4);

	advect(N, 1, Vx, VxO, VxO, VyO, VzO, dt);
	advect(N, 2, Vy, VyO, VxO, VyO, VzO, dt);
	advect(N, 3, Vz, VzO, VxO, VyO, VzO, dt);

	project(N, Vx, Vy, Vz, VxO, VyO, 4);

	diffuse(N, 0, s, density, diff, dt, 4);
	advect(N, 0, density, s, Vx, Vy, Vz, dt);
}

void AGAFluidSimulator::draw_dens(int Size, float* density)
{
	int SizeType = 0;

	switch (Size)
	{
	case 16:
		SizeType = 0;
		break;
	case 32:
		SizeType = 1;
		break;
	case 64:
		SizeType = 2;
		break;
	case 128:
		SizeType = 3;
		break;
	case 256:
		SizeType = 4;
		break;
	}

	if (bIsCanvasInitialized)
	{
		UpdatePsuedoVolumeTextureFromDensity(Size, density);
	}
	else
	{
		CreatePsuedoVolumeTextureFromDensity(SizeType, density);
	}
}

void AGAFluidSimulator::CreatePsuedoVolumeTextureFromDensity(int SizeType, float* density)
{
	int TotalResolution = 0, bufferSize = 0;

	switch (SizeType)
	{
		case 0:
			// 16 cubed
			// Individual Resolution = 16;
			// GridSize = 4x4;
			TotalResolution = 64;
			// bytesPerPixel = 4
			// bufferSize = TotalResolution * TotalResolution * bytesPerPixel;
			bufferSize = 16384;
			break;
		case 1:
			// 32 cubed
			// Indiviual Resolution = 32;
			// Grid size = 6x6;
			// * Not Perfect * //
			TotalResolution = 192;
			// bytesPerPixel = 4
			// bufferSize = TotalResolution * TotalResolution * bytesPerPixel;
			bufferSize = 147456;
			break;
		case 2:
			// 64 cubed
			// Individual Resolution = 64;
			// Grid Size = 8x8;
			TotalResolution = 512;
			// bytesPerPixel = 4
			// bufferSize = TotalResolution * TotalResolution * bytesPerPixel;
			bufferSize = 1048576;
			break;
		case 3:
			// 128 cubed
			// Individual Resolution = 128;
			// Grid Size = 12x12;
			// * Not Perfect * //
			TotalResolution = 1450;
			// bytesPerPixel = 4
			// bufferSize = TotalResolution * TotalResolution * bytesPerPixel;
			bufferSize = 8410000;
			break;
		case 4:
			// 256 cubed
			// Individual Resolution = 256;
			// Grid Size = 16x16;
			TotalResolution = 4096;
			// bytesPerPixel = 4
			// bufferSize = TotalResolution * TotalResolution * bytesPerPixel;
			bufferSize = 67108864;
			// 67,108,864
			break;
		case 5:
			// 512 cubed
			// Individual Resolution = 512;
			// Grid Size = 22x23;
			// * Insane- NEVER USE - Not Perfect -enasnI * //
			TotalResolution = 11590;
			break;
		case 6:
			// 1024 cubed
			// Individual Resolution = 1024;
			// Grid Size = 32x32;
			// * Insane- NEVER USE -enasnI * //
			TotalResolution = 32768;
			break;
	}

	myCanvas->InitializeCanvas(TotalResolution, TotalResolution);
	bIsCanvasInitialized = true;
}

void AGAFluidSimulator::UpdatePsuedoVolumeTextureFromDensity(int Size, float* density)
{
	myCanvas->UpdatePsuedoVolume(Size, density);
}

UTexture2D* AGAFluidSimulator::GetTexture()
{
	if (myCanvas)
	{
		UTexture2D* myTexture = myCanvas->GetTexture();
		if (myTexture)
		{
			return myTexture;
		}
	}

	return NULL;
}