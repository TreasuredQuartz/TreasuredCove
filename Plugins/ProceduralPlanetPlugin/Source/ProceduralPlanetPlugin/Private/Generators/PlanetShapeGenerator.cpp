// Fill out your copyright notice in the Description page of Project Settings.


#include "Generators/PlanetShapeGenerator.h"
#include "PlanetNoise.h"
#include "Engine/Engine.h"
 
FPlanetShapeGenerator::FPlanetShapeGenerator(FPlanetShapeSettings inSettings) : Settings(inSettings)
{
	PlanetNoises.SetNumUninitialized(inSettings.noiseLayers.Num());

	for(int i = 0; i < inSettings.noiseLayers.Num(); ++i)
		PlanetNoises[i] = new FPlanetNoise(inSettings.noiseLayers[i].noiseSettings);

	elevationMinMax = FMinMax();
}

void FPlanetShapeGenerator::UpdateSettings(FPlanetShapeSettings inSettings)
{
	Settings = inSettings;

	PlanetNoises.SetNumUninitialized(inSettings.noiseLayers.Num());

	for (int i = 0; i < inSettings.noiseLayers.Num(); ++i)
		PlanetNoises[i] = new FPlanetNoise(inSettings.noiseLayers[i].noiseSettings);

	elevationMinMax = FMinMax();
}

FVector FPlanetShapeGenerator::CalculatePointOnPlanet(FVector pointOnUnitSphere)
{
	float elevation = 0;

	for (int i = 0; i < Settings.noiseLayers.Num(); ++i)
	{
		if (Settings.noiseLayers.IsValidIndex(i))
		{
			if (Settings.noiseLayers[i].bIsEnabled)
				elevation += PlanetNoises[i]->Evaluate(pointOnUnitSphere); // returns a value between 0 and 1
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Settings.noiseLayers[i] is invalid."));
		}
	}
	
	elevationMinMax.Add(++elevation); // Add the value "elevation + 1" to "elevationMinMax"
	return pointOnUnitSphere * elevation; // We are multiplying by elevation, but "Evaluate" could return a value of 0. Add one before this then.
}





// #begin Depreciated //



void FPlanetShapeGenerator::GeneratePoints(int numPoints, TArray<FVector>& Points)
{
	float index = 0.5,
		goldenRatio = (1.f + FMath::Sqrt(5.f)) * PI, /*UE_GOLDEN_RATIO*/
		x, y, z, theta;
	Points.SetNumZeroed(numPoints);

	UE_GOLDEN_RATIO;

	while (index < numPoints)
	{
		theta = FMath::Acos(1.0 - (index * 2.0) / numPoints);
		x = FMath::Cos(index * goldenRatio) * FMath::Sin(theta);
		y = FMath::Sin(index * goldenRatio) * FMath::Sin(theta);
		z = FMath::Cos(theta);
		Points[index] = FVector(x, y, z);
		++index;
	}
}

void FPlanetShapeGenerator::GenerateUVs(TArray<FVector>const& Points,
	TArray<FVector2D>& inUVs)
{
	// Local variables //
	int flipIndex = 0,
		Index = 0;
	float GoldenRatio = 1.0 + sqrt(5.0),
		curIndex = 0,
		curRadius = 0,
		curTheta = 0,
		u = 0,
		v = 0;

	// An attempt at converting the 3D vertices into a 2D uv space...
	/*for (Index = 0; Index < Points.Num(); ++Index)
	{
		u = FMath::Atan2(Points[Index].Y, Points[Index].X);
		v = FMath::Acos(Points[Index].Z *
			FMath::Sqrt((Points[Index].X * Points[Index].X) + (Points[Index].Y * Points[Index].Y) + (Points[Index].Z * Points[Index].Z)));
		UVs.Add(FVector2D(u, v));
	}*/

	// 2D Fibonacci spiral which is mirrored across the Z axis
	for (Index = 0; Index < Points.Num(); ++Index)
	{
		if (Points[Index].Z < 0)
		{
			if (flipIndex == 0)
				flipIndex = Index;

			curIndex = Points.Num() - Index + 0.525;
			curTheta = -PI * GoldenRatio * curIndex;
		}
		else if (Points[Index].Z > 0)
		{
			curIndex = Index + 0.5;
			curTheta = (PI * GoldenRatio * curIndex);
		}

		curRadius = FMath::Sqrt(curIndex / Points.Num());
		u = cosf(curTheta) * curRadius;
		v = sinf(curTheta) * curRadius;
		inUVs.Add(FVector2D(u, v));
	}
}

void FPlanetShapeGenerator::GenerateNormals(TArray<FVector>const& Points,
	TArray<FVector>& inNormals)
{
	for (FVector Normal : Points)
	{
		inNormals.Add(Normal.GetSafeNormal());
	}
}

FVector2D FPlanetShapeGenerator::SterographicProjection(const FVector& xyz)
{
	return FVector2D(xyz.X / (1 - xyz.Z), xyz.Y / (1 - xyz.Z));
}

TArray<FVector2D> FPlanetShapeGenerator::MassSterographicProjection(TArray<FVector>const& xyz)
{
	int numPoints = xyz.Num();
	TArray<FVector2D> XY;

	for (int i = 0; i < numPoints; ++i)
	{
		XY.Push(SterographicProjection(xyz[i]));
	}

	return XY;
}

bool FPlanetShapeGenerator::IsClockwise(TArray<FVector>const& vertices,
	int index1, int index2, int index3) {

	int cnt = 0;

	//Use only the x and z coordinates to "project" the vertices in a 2D plane
	FVector2D* p1 = new FVector2D(vertices[index1].X, vertices[index1].Z);
	FVector2D* p2 = new FVector2D(vertices[index2].X, vertices[index2].Z);
	FVector2D* p3 = new FVector2D(vertices[index3].X, vertices[index3].Z);

	float determinant =
		p1->X * p2->Y +
		p3->X * p1->Y +
		p2->X * p3->Y -
		p1->X * p3->Y -
		p3->X * p2->Y -
		p2->X * p1->Y;

	//If any of the vertices is lower than the origin point, increment the counter
	if (vertices[index1].Y < 0) cnt++;
	if (vertices[index2].Y < 0) cnt++;
	if (vertices[index3].Y < 0) cnt++;

	//If at least 2 vertices are lower than the origin point, change the condition for determining
	//clockwise direction
	if (cnt > 1) return determinant > 0.f;
	return determinant < 0.f;
}

void FPlanetShapeGenerator::AddTrianglesInClockwiseDirection(TArray<int>& Triangles, TArray<FVector>const& Vertices,
	int index1, int index2, int index3) {

	//Checking for all 6 possible edge combinations in a set of 3 vertices.

	//First permutation
	if (!IsClockwise(Vertices, index1, index2, index3)) {
		Triangles.Add(index1);
		Triangles.Add(index2);
		Triangles.Add(index3);
		return;
	}

	////Second permutation
	if (!IsClockwise(Vertices, index1, index3, index2)) {
		Triangles.Add(index1);
		Triangles.Add(index3);
		Triangles.Add(index2);
		return;
	}

	////Third permutation
	if (!IsClockwise(Vertices, index3, index1, index2)) {
		Triangles.Add(index3);
		Triangles.Add(index1);
		Triangles.Add(index2);
		return;
	}

	////Fourth permutation
	if (!IsClockwise(Vertices, index3, index2, index1)) {
		Triangles.Add(index3);
		Triangles.Add(index2);
		Triangles.Add(index1);
		return;
	}

	////Fifth permutation
	if (!IsClockwise(Vertices, index2, index1, index3)) {
		Triangles.Add(index2);
		Triangles.Add(index1);
		Triangles.Add(index3);
		return;
	}

	////Sixth permutation
	if (!IsClockwise(Vertices, index2, index3, index1)) {
		Triangles.Add(index2);
		Triangles.Add(index3);
		Triangles.Add(index1);
		return;
	}
}

void FPlanetShapeGenerator::GenerateTriangles(TArray<FVector>const& Vertices,
	TArray<int>& Triangles)
{
	//Calculate an "average" distance between points (I used the first two points for this purpose)
	float averageDistance = FVector::Distance(Vertices[0], Vertices[1]) + 7; // (User tested value) Catches some holes

	//Loop through all vertices and for each find two other points that are at least 
	//1.2 * averageDistance away from that specific vertex.
	for (int i = 0; i < Vertices.Num(); ++i) {
		int idx2 = 0, idx3 = 0, cnt = 0;
		if (FMath::Abs(Vertices[i].Y) < 0.5) ++cnt; // 0.5 is (1 / radius)

		TArray<int> closeVertices = TArray<int>();
		for (int j = 0; j < Vertices.Num(); ++j) {
			if (i != j) {
				float dist = FVector::Distance(Vertices[i], Vertices[j]);
				if (cnt > 0) {
					if (dist <= 1.7f * averageDistance) {
						closeVertices.Add(j);
					}
				}
				else {
					if (dist <= 1.2f * averageDistance) {
						closeVertices.Add(j);
					}
				}

			}
		}

		cnt = 0;

		for (int k = 1; k < closeVertices.Num(); ++k) {
			idx2 = closeVertices[k - 1];
			idx3 = closeVertices[k];
			if (idx2 == INDEX_NONE || idx3 == INDEX_NONE) continue;
			AddTrianglesInClockwiseDirection(Triangles, Vertices, i, idx2, idx3);
		}

		//After 3 vertices have been found, add them in the triangle list in clockwise direction
		// AddTrianglesInClockwiseDirection(Triangles, vertices, i, idx2, idx3);
	}
}



// #end Depreciated //
