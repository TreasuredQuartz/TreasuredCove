// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/Actors/Items/KatanaCreator.h"
#include "RuntimeMeshComponentStatic.h"

UKatanaCreator::UKatanaCreator()
{
}

void UKatanaCreator::GenerateKatana()
{
	NewObject<UPart>();

	UPart* CurPart = Parts[0];
	int32 PartVertNum = 0;
	int32 LODDepth = 0;
	FRuntimeMeshCollisionVertexStream CollisionVerts;
	FRuntimeMeshCollisionTriangleStream CollisionTris;

	while (CurPart != nullptr)
	{
		if (CurPart->HasInnerParts())
		{
			LODDepth += 1;
			// Recursion
		}

		// Bottom Part
		FProceduralMeshSection Section = CurPart->GenerateSection();

		for (FVector Vert : Section.Vertices)
		{
			CollisionVerts.Add(Vert);
		}

		for (int32 i = 0; i < Section.Triangles.Num(); i += 3)
		{
			CollisionTris.Add(Section.Triangles[i] + PartVertNum, Section.Triangles[i + 1] + PartVertNum, Section.Triangles[i + 2] + PartVertNum);
		}

		PartVertNum += Section.Vertices.Num();
	}

	for (UPart* Part : Parts)
	{
		FProceduralMeshSection Section = Part->GenerateSection();

		for (FVector Vert : Section.Vertices)
		{
			CollisionVerts.Add(Vert);
		}

		for (int32 i = 0; i < Section.Triangles.Num(); i += 3)
		{
			CollisionTris.Add(Section.Triangles[i] + PartVertNum, Section.Triangles[i + 1] + PartVertNum, Section.Triangles[i + 2] + PartVertNum);
		}

		PartVertNum += Section.Vertices.Num();
	}

	FRuntimeMeshCollisionData CollisionData;
	CollisionData.Vertices = CollisionVerts;
	CollisionData.Triangles = CollisionTris;
	Mesh->SetCollisionMesh(CollisionData);
}

FProceduralMeshSection UPart::GenerateSection()
{
	FProceduralMeshSection Section;

	for (int32 i = 0; i < SectionRange.Minimum.Num(); ++i)
	{
		const FPartTriangleData& MinTriData = SectionRange.Minimum[i];
		const FPartTriangleData& MaxTriData = SectionRange.Maximum[i];

		for (int32 VertIndex = 0; VertIndex < 3; ++VertIndex)
		{
			const FPartVertexData& MinVertData = MinTriData.ABC[VertIndex];
			const FPartVertexData& MaxVertData = MaxTriData.ABC[VertIndex];

			const float Alpha = FMath::FRand();
			const FVector Location =	FMath::Lerp(MinVertData.Location, MaxVertData.Location, Alpha);
			const FVector Normal =		FMath::Lerp(MinVertData.Normal, MaxVertData.Normal, Alpha);
			const FVector2D UV =		FMath::Lerp(MinVertData.UV, MaxVertData.UV, Alpha);
			const FLinearColor Color =	FMath::Lerp(MinVertData.Color, MaxVertData.Color, Alpha);

			Section.Vertices.Add(Location);
			Section.Triangles.Add(Section.NumTriangles + VertIndex);
			Section.Normals.Add(Normal);
			Section.UVs.Add(UV);
			Section.VertexColors.Add(Color);
			Section.Tangents.Add(FRuntimeMeshTangent());
		}
	}

	return Section;
}

