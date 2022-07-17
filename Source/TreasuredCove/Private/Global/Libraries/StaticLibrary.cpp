

#include "StaticLibrary.h"

#include "GameplayTagContainer.h"

#include "PhysicsEngine/BodySetup.h"
#include "GeomTools.h"

FName UStaticLibrary::GetTagLeafName(const struct FGameplayTag& Tag)
{
	FString TagNameAsString = Tag.ToString();

	FString Left;
	FString Right;

	if (TagNameAsString.Split(FString(TEXT(".")), &Left, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd))
	{
		return FName(*Right);
	}
	else
	{
		return Tag.GetTagName();
	}
}

bool UStaticLibrary::EqualEqual_FBulletStructFBulletStruct(const FBullet& arg1, const FBullet& arg2)
{
	return arg1 == arg2;
}

bool UStaticLibrary::EqualEqual_FItemKeyFItemKey(const FItemKey& arg1, const FItemKey& arg2)
{
	return arg1 == arg2;
}

int UStaticLibrary::Find_FItemKeyFItemKey(const TArray<FItemKey> Array, const FItemKey ItemToFind)
{
	int value = -1;

	for (int i = 0; i < Array.Num(); i++)
	{
		if (Array[i] == ItemToFind)
		{
			value = i;
			break;
		}
	}

	return value;
}

FItemKey UStaticLibrary::Addition_FItemKeyFItemKey(const FItemKey Item1, const FItemKey Item2)
{
	return Item1 + Item2;
}

int32 UStaticLibrary::BoxPlaneCompareLimited(FBox InBox, const FPlane& InPlane, float Radius, FVector Center)
{
	FVector BoxCenter, BoxExtents;
	InBox.GetCenterAndExtents(BoxCenter, BoxExtents);

	// Find distance of box center from plane
	float BoxCenterDist = InPlane.PlaneDot(BoxCenter);

	// See size of box in plane normal direction
	float BoxSize = FVector::BoxPushOut(InPlane, BoxExtents);

	if (BoxCenterDist > BoxSize)
	{
		return 1;
	}
	else if (BoxCenterDist < -BoxSize)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

FProcMeshVertex UStaticLibrary::InterpolateVert(const FProcMeshVertex& V0, const FProcMeshVertex& V1, float Alpha)
{
	FProcMeshVertex Result;

	// Handle dodgy alpha
	if (FMath::IsNaN(Alpha) || !FMath::IsFinite(Alpha))
	{
		Result = V1;
		return Result;
	}

	Result.Position = FMath::Lerp(V0.Position, V1.Position, Alpha);

	Result.Normal = FMath::Lerp(V0.Normal, V1.Normal, Alpha);

	Result.Tangent.TangentX = FMath::Lerp(V0.Tangent.TangentX, V1.Tangent.TangentX, Alpha);
	Result.Tangent.bFlipTangentY = V0.Tangent.bFlipTangentY; // Assume flipping doesn't change along edge...

	Result.UV0 = FMath::Lerp(V0.UV0, V1.UV0, Alpha);

	Result.Color.R = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.R), float(V1.Color.R), Alpha)), 0, 255);
	Result.Color.G = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.G), float(V1.Color.G), Alpha)), 0, 255);
	Result.Color.B = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.B), float(V1.Color.B), Alpha)), 0, 255);
	Result.Color.A = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.A), float(V1.Color.A), Alpha)), 0, 255);

	return Result;
}

/** Transform triangle from 2D to 3D static-mesh triangle. */
void UStaticLibrary::Transform2DPolygonTo3D(const FUtilPoly2D& InPoly, const FMatrix& InMatrix, TArray<FProcMeshVertex>& OutVerts, FBox& OutBox)
{
	FVector PolyNormal = -InMatrix.GetUnitAxis(EAxis::Z);
	FProcMeshTangent PolyTangent(InMatrix.GetUnitAxis(EAxis::X), false);

	for (int32 VertexIndex = 0; VertexIndex < InPoly.Verts.Num(); VertexIndex++)
	{
		const FUtilVertex2D& InVertex = InPoly.Verts[VertexIndex];

		FProcMeshVertex NewVert;

		NewVert.Position = InMatrix.TransformPosition(FVector(InVertex.Pos.X, InVertex.Pos.Y, 0.f));
		NewVert.Normal = PolyNormal;
		NewVert.Tangent = PolyTangent;
		NewVert.Color = InVertex.Color;
		NewVert.UV0 = InVertex.UV;

		OutVerts.Add(NewVert);

		// Update bounding box
		OutBox += NewVert.Position;
	}
}

/** Given a polygon, decompose into triangles. */
bool UStaticLibrary::TriangulatePoly(TArray<uint32>& OutTris, const TArray<FProcMeshVertex>& PolyVerts, int32 VertBase, const FVector& PolyNormal)
{
	// Can't work if not enough verts for 1 triangle
	int32 NumVerts = PolyVerts.Num() - VertBase;
	if (NumVerts < 3)
	{
		OutTris.Add(0);
		OutTris.Add(2);
		OutTris.Add(1);

		// Return true because poly is already a tri
		return true;
	}

	// Remember initial size of OutTris, in case we need to give up and return to this size
	const int32 TriBase = OutTris.Num();

	// Init array of vert indices, in order. We'll modify this
	TArray<int32> VertIndices;
	VertIndices.AddUninitialized(NumVerts);
	for (int VertIndex = 0; VertIndex < NumVerts; VertIndex++)
	{
		VertIndices[VertIndex] = VertBase + VertIndex;
	}

	// Keep iterating while there are still vertices
	while (VertIndices.Num() >= 3)
	{
		// Look for an 'ear' triangle
		bool bFoundEar = false;
		for (int32 EarVertexIndex = 0; EarVertexIndex < VertIndices.Num(); EarVertexIndex++)
		{
			// Triangle is 'this' vert plus the one before and after it
			const int32 AIndex = (EarVertexIndex == 0) ? VertIndices.Num() - 1 : EarVertexIndex - 1;
			const int32 BIndex = EarVertexIndex;
			const int32 CIndex = (EarVertexIndex + 1) % VertIndices.Num();

			const FProcMeshVertex& AVert = PolyVerts[VertIndices[AIndex]];
			const FProcMeshVertex& BVert = PolyVerts[VertIndices[BIndex]];
			const FProcMeshVertex& CVert = PolyVerts[VertIndices[CIndex]];

			// Check that this vertex is convex (cross product must be positive)
			const FVector ABEdge = BVert.Position - AVert.Position;
			const FVector ACEdge = CVert.Position - AVert.Position;
			const float TriangleDeterminant = (ABEdge ^ ACEdge) | PolyNormal;
			if (TriangleDeterminant > 0.f)
			{
				continue;
			}

			bool bFoundVertInside = false;
			// Look through all verts before this in array to see if any are inside triangle
			for (int32 VertexIndex = 0; VertexIndex < VertIndices.Num(); VertexIndex++)
			{
				const FProcMeshVertex& TestVert = PolyVerts[VertIndices[VertexIndex]];

				if (VertexIndex != AIndex &&
					VertexIndex != BIndex &&
					VertexIndex != CIndex &&
					FGeomTools::PointInTriangle(FVector3f(AVert.Position), FVector3f(BVert.Position), FVector3f(CVert.Position), FVector3f(TestVert.Position)))
				{
					bFoundVertInside = true;
					break;
				}
			}

			// Triangle with no verts inside - its an 'ear'! 
			if (!bFoundVertInside)
			{
				OutTris.Add(VertIndices[AIndex]);
				OutTris.Add(VertIndices[CIndex]);
				OutTris.Add(VertIndices[BIndex]);

				// And remove vertex from polygon
				VertIndices.RemoveAt(EarVertexIndex);

				bFoundEar = true;
				break;
			}
		}

		// If we couldn't find an 'ear' it indicates something is bad with this polygon - discard triangles and return.
		if (!bFoundEar)
		{
			OutTris.SetNum(TriBase, true);
			return false;
		}
	}

	return true;
}

/** Util to slice a convex hull with a plane */
void UStaticLibrary::SliceConvexElem(const FKConvexElem& InConvex, const FPlane& SlicePlane, TArray<FVector>& OutConvexVerts)
{
	// Get set of planes that make up hull
	TArray<FPlane> ConvexPlanes;
	InConvex.GetPlanes(ConvexPlanes);

	if (ConvexPlanes.Num() >= 4)
	{
		// Add on the slicing plane (need to flip as it culls geom in the opposite sense to our geom culling code)
		ConvexPlanes.Add(SlicePlane.Flip());

		// Create output convex based on new set of planes
		FKConvexElem SlicedElem;
		bool bSuccess = SlicedElem.HullFromPlanes(ConvexPlanes, InConvex.VertexData);
		if (bSuccess)
		{
			OutConvexVerts = SlicedElem.VertexData;
		}
	}
}

void UStaticLibrary::HolePunchProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector EnterPoint, FVector EnterNormal, FVector ExitPoint, float HoleRadius, EProcMeshSliceCapOption CapOption, UMaterialInterface* CapMaterial, int32 Steps)
{
	if (InProcMesh != nullptr)
	{
		TArray<FPlane> SlicePlanes;
		if (Steps < 4)
		{
			SlicePlanes.SetNumUninitialized(3);
		}
		else
		{
			SlicePlanes.SetNumUninitialized(Steps);
		}

		/*
		FVector North = EnterPoint + FVector(1, 0, 0) * HoleRadius;
		FVector NorthEast = EnterPoint + FVector(0.5, 0.5, 0) * HoleRadius;
		FVector East = EnterPoint + FVector(0, 1, 0) * HoleRadius;
		FVector SouthEast = EnterPoint + FVector(-0.5, 0.5, 0) * HoleRadius;
		FVector South = EnterPoint + FVector(-1, 0, 0) * HoleRadius;
		FVector SouthWest = EnterPoint + FVector(-0.5, -0.5, 0) * HoleRadius;
		FVector West = EnterPoint + FVector(0, -1, 0) * HoleRadius;
		FVector NorthWest = EnterPoint + FVector(0.5, -0.5, 0) * HoleRadius;
		*/

		for (int32 a = 0; a < SlicePlanes.Num(); a++)
		{
			FRotator NormalRotation =
				FRotationMatrix::MakeFromX(EnterNormal).Rotator();
			NormalRotation.Roll += (a + 1 / SlicePlanes.Num()) * 360;

			FTransform ProcCompToWorld = InProcMesh->GetComponentToWorld();
			FVector LocalEnterSpherePos = ProcCompToWorld.InverseTransformPosition(EnterPoint);
			FVector LocalExitSpherePos = ProcCompToWorld.InverseTransformPosition(ExitPoint);

			FVector RandUnitVector =
				FVector(0, FMath::FRand(), FMath::FRand()).GetSafeNormal();

			FRotator LocRotation =
				FRotationMatrix::MakeFromZ(RandUnitVector).Rotator();

			// Location on edge of circle pointing inward
			FVector LocPlanePos =
				LocRotation.RotateVector(RandUnitVector * HoleRadius + LocalEnterSpherePos);

			// Rotation according to impact normal
			FVector LocalPlaneNormal =
				NormalRotation.RotateVector(LocPlanePos).UpVector;

			FPlane SlicePlane(LocPlanePos, LocalPlaneNormal);

			// Set of new edges created by clipping polys by plane
			TArray<FUtilEdge3D> ClipEdges;

			for (int32 SectionIndex = 0; SectionIndex < InProcMesh->GetNumSections(); SectionIndex++)
			{
				FProcMeshSection* BaseSection = InProcMesh->GetProcMeshSection(SectionIndex);
				// If we have a section, and it has some valid geom
				if (BaseSection != nullptr && BaseSection->ProcIndexBuffer.Num() > 0 && BaseSection->ProcVertexBuffer.Num() > 0)
				{
					// Compare bounding box of section with slicing plane
					int32 BoxCompare = BoxPlaneCompareLimited(BaseSection->SectionLocalBox, SlicePlane, HoleRadius, EnterPoint);

					// Box totally clipped, clear section
					if (BoxCompare == -1)
					{
						InProcMesh->ClearMeshSection(SectionIndex);
					}
					// Box totally on one side of plane, leave it alone, do nothing
					else if (BoxCompare == 1)
					{
						// ...
					}
					// Box intersects plane, need to clip some polys!
					else
					{
						// New section for geometry
						FProcMeshSection NewSection;

						// Map of base vert index to sliced vert index
						TMap<int32, int32> BaseToSlicedVertIndex;
						TMap<int32, int32> BaseToOtherSlicedVertIndex;

						const int32 NumBaseVerts = BaseSection->ProcVertexBuffer.Num();

						// Distance of each base vert from slice point
						TArray<float> VertDistance;
						VertDistance.AddUninitialized(NumBaseVerts);

						// Build vertex buffer 
						for (int32 BaseVertIndex = 0; BaseVertIndex < NumBaseVerts; BaseVertIndex++)
						{
							FProcMeshVertex& BaseVert = BaseSection->ProcVertexBuffer[BaseVertIndex];

							// Calc distance from plane
							VertDistance[BaseVertIndex] = SlicePlane.PlaneDot(BaseVert.Position);

							// See if vert is being kept in this section
							if (VertDistance[BaseVertIndex] > 0.f)
							{
								// Copy to sliced v buffer
								int32 SlicedVertIndex = NewSection.ProcVertexBuffer.Add(BaseVert);
								// Update section bounds
								NewSection.SectionLocalBox += BaseVert.Position;
								// Add to map
								BaseToSlicedVertIndex.Add(BaseVertIndex, SlicedVertIndex);
							}
						}


						// Iterate over base triangles (ie 3 indices at a time)
						for (int32 BaseIndex = 0; BaseIndex < BaseSection->ProcIndexBuffer.Num(); BaseIndex += 3)
						{
							int32 BaseV[3]; // Triangle vert indices in original mesh
							int32* SlicedV[3]; // Pointers to vert indices in new v buffer

							// For each vertex..
							for (int32 i = 0; i < 3; i++)
							{
								// Get triangle vert index
								BaseV[i] = BaseSection->ProcIndexBuffer[BaseIndex + i];
								// Look up in sliced v buffer
								SlicedV[i] = BaseToSlicedVertIndex.Find(BaseV[i]);
							}

							// If all verts survived plane cull, keep the triangle
							if (SlicedV[0] != nullptr && SlicedV[1] != nullptr && SlicedV[2] != nullptr)
							{
								NewSection.ProcIndexBuffer.Add(*SlicedV[0]);
								NewSection.ProcIndexBuffer.Add(*SlicedV[1]);
								NewSection.ProcIndexBuffer.Add(*SlicedV[2]);
							}
							// If partially culled, clip to create 1 or 2 new triangles
							else
							{
								int32 FinalVerts[4];
								int32 NumFinalVerts = 0;

								FUtilEdge3D NewClipEdge;
								int32 ClippedEdges = 0;

								float PlaneDist[3];
								PlaneDist[0] = VertDistance[BaseV[0]];
								PlaneDist[1] = VertDistance[BaseV[1]];
								PlaneDist[2] = VertDistance[BaseV[2]];

								for (int32 EdgeIdx = 0; EdgeIdx < 3; EdgeIdx++)
								{
									int32 ThisVert = EdgeIdx;

									// If start vert is inside, add it.
									if (SlicedV[ThisVert] != nullptr)
									{
										check(NumFinalVerts < 4);
										FinalVerts[NumFinalVerts++] = *SlicedV[ThisVert];
									}

									// If start and next vert are on opposite sides, add intersection
									int32 NextVert = (EdgeIdx + 1) % 3;

									if ((SlicedV[EdgeIdx] == nullptr) != (SlicedV[NextVert] == nullptr))
									{
										// Find distance along edge that plane is
										float Alpha = -PlaneDist[ThisVert] / (PlaneDist[NextVert] - PlaneDist[ThisVert]);
										// Interpolate vertex params to that point
										FProcMeshVertex InterpVert = InterpolateVert(BaseSection->ProcVertexBuffer[BaseV[ThisVert]], BaseSection->ProcVertexBuffer[BaseV[NextVert]], FMath::Clamp(Alpha, 0.0f, 1.0f));

										// Add to vertex buffer
										int32 InterpVertIndex = NewSection.ProcVertexBuffer.Add(InterpVert);
										// Update bounds
										NewSection.SectionLocalBox += InterpVert.Position;

										// Save vert index for this poly
										check(NumFinalVerts < 4);
										FinalVerts[NumFinalVerts++] = InterpVertIndex;

										// When we make a new edge on the surface of the clip plane, save it off.
										check(ClippedEdges < 2);
										if (ClippedEdges == 0)
										{
											NewClipEdge.V0 = FVector3f(InterpVert.Position);
										}
										else
										{
											NewClipEdge.V1 = FVector3f(InterpVert.Position);
										}

										ClippedEdges++;
									}
								}

								// Triangulate the clipped polygon.
								for (int32 VertexIndex = 2; VertexIndex < NumFinalVerts; VertexIndex++)
								{
									NewSection.ProcIndexBuffer.Add(FinalVerts[0]);
									NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex - 1]);
									NewSection.ProcIndexBuffer.Add(FinalVerts[VertexIndex]);
								}

								check(ClippedEdges != 1); // Should never clip just one edge of the triangle

								// If we created a new edge, save that off here as well
								if (ClippedEdges == 2)
								{
									ClipEdges.Add(NewClipEdge);
								}
							}
						}

						// If we have some valid geometry, update section
						if (NewSection.ProcIndexBuffer.Num() > 0 && NewSection.ProcVertexBuffer.Num() > 0)
						{
							// Assign new geom to this section
							InProcMesh->SetProcMeshSection(SectionIndex, NewSection);
						}
						// If we don't, remove this section
						else
						{
							InProcMesh->ClearMeshSection(SectionIndex);
						}
					}
				}
			}

			// Create cap geometry (if some edges to create it from)
			if (CapOption != EProcMeshSliceCapOption::NoCap && ClipEdges.Num() > 0)
			{
				FProcMeshSection CapSection;
				int32 CapSectionIndex = INDEX_NONE;

				// If using an existing section, copy that info first
				if (CapOption == EProcMeshSliceCapOption::UseLastSectionForCap)
				{
					CapSectionIndex = InProcMesh->GetNumSections() - 1;
					CapSection = *InProcMesh->GetProcMeshSection(CapSectionIndex);
				}
				// Adding new section for cap
				else
				{
					CapSectionIndex = InProcMesh->GetNumSections();
				}

				// Project 3D edges onto slice plane to form 2D edges
				TArray<FUtilEdge2D> Edges2D;
				FUtilPoly2DSet PolySet;
				FGeomTools::ProjectEdges(Edges2D, PolySet.PolyToWorld, ClipEdges, SlicePlane);

				// Find 2D closed polygons from this edge soup
				FGeomTools::Buid2DPolysFromEdges(PolySet.Polys, Edges2D, FColor(255, 255, 255, 255));

				// Remember start point for vert and index buffer before adding and cap geom
				int32 CapVertBase = CapSection.ProcVertexBuffer.Num();
				int32 CapIndexBase = CapSection.ProcIndexBuffer.Num();

				// Triangulate each poly
				for (int32 PolyIdx = 0; PolyIdx < PolySet.Polys.Num(); PolyIdx++)
				{
					// Generate UVs for the 2D polygon.
					FGeomTools::GeneratePlanarTilingPolyUVs(PolySet.Polys[PolyIdx], 64.f);

					// Remember start of vert buffer before adding triangles for this poly
					int32 PolyVertBase = CapSection.ProcVertexBuffer.Num();

					// Transform from 2D poly verts to 3D
					Transform2DPolygonTo3D(PolySet.Polys[PolyIdx], PolySet.PolyToWorld, CapSection.ProcVertexBuffer, CapSection.SectionLocalBox);

					// Triangulate this polygon
					TriangulatePoly(CapSection.ProcIndexBuffer, CapSection.ProcVertexBuffer, PolyVertBase, LocalPlaneNormal);
				}

				// Set geom for cap section
				InProcMesh->SetProcMeshSection(CapSectionIndex, CapSection);

				// If creating new section for cap, assign cap material to it
				if (CapOption == EProcMeshSliceCapOption::CreateNewSectionForCap)
				{
					InProcMesh->SetMaterial(CapSectionIndex, CapMaterial);
				}
			}

			// Array of sliced collision shapes
			TArray< TArray<FVector> > SlicedCollision;
			TArray< TArray<FVector> > OtherSlicedCollision;

			UBodySetup* ProcMeshBodySetup = InProcMesh->GetBodySetup();

			for (int32 ConvexIndex = 0; ConvexIndex < ProcMeshBodySetup->AggGeom.ConvexElems.Num(); ConvexIndex++)
			{
				FKConvexElem& BaseConvex = ProcMeshBodySetup->AggGeom.ConvexElems[ConvexIndex];

				int32 BoxCompare = BoxPlaneCompareLimited(BaseConvex.ElemBox, SlicePlane, HoleRadius, EnterPoint);
				// If box totally valid, just keep mesh as is
				if (BoxCompare == 1)
				{
					SlicedCollision.Add(BaseConvex.VertexData);
				}
				// Need to actually slice the convex shape
				else
				{
					TArray<FVector> SlicedConvexVerts;
					SliceConvexElem(BaseConvex, SlicePlane, SlicedConvexVerts);
					// If we got something valid, add it
					if (SlicedConvexVerts.Num() >= 4)
					{
						SlicedCollision.Add(SlicedConvexVerts);
					}
				}
			}

			// Update collision of proc mesh
			InProcMesh->SetCollisionConvexMeshes(SlicedCollision);
		}
	}
}

void UStaticLibrary::DentProceduralMesh(UProceduralMeshComponent* InProcMesh, FVector Center, float Radius, float ExpFalloff)
{

}

TObjectPtr<UObject> UStaticLibrary::GetCurrentPlanet(TObjectPtr<UObject> CallingObject)
{
	if (CallingObject == nullptr) return nullptr;

	return nullptr;;
}

