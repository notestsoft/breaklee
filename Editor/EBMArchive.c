#include "Context.h"
#include "EBMArchive.h"
#include "MeshUtil.h"

EBMArchiveRef EBMArchiveCreate(
	AllocatorRef Allocator
) {
	EBMArchiveRef Archive = (EBMArchiveRef)AllocatorAllocate(Allocator, sizeof(struct _EBMArchive));
	memset(Archive, 0, sizeof(struct _EBMArchive));
	Archive->Allocator = Allocator;
	Archive->Magic = 3;
	Archive->Version = 1005;
	Archive->ScalePercentage = 100;
	Archive->Materials = ArrayCreateEmpty(Allocator, sizeof(struct _EBMMaterial), 8);
	Archive->Bones = ArrayCreateEmpty(Allocator, sizeof(struct _EBMBone), 8);
	Archive->Meshes = ArrayCreateEmpty(Allocator, sizeof(struct _EBMMesh), 8);
	Archive->SkinBones = ArrayCreateEmpty(Allocator, sizeof(struct _EBMSkinBone), 8);
	Archive->Animations = ArrayCreateEmpty(Allocator, sizeof(struct _EBMAnimation), 8);
	return Archive;
}

Void EBMArchiveDestroy(
	EBMArchiveRef Archive
) {
	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Materials); Index += 1) {
		EBMMaterialRef Material = (EBMMaterialRef)ArrayGetElementAtIndex(Archive->Materials, Index);
		AllocatorDeallocate(Archive->Allocator, Material->TextureMain.Texture.Name);
		//if (Material->TextureMain.Texture.Data) AllocatorDeallocate(Archive->Allocator, Material->TextureMain.Texture.Data);
		UnloadTexture(Material->TextureMain.Texture.Texture);
		UnloadImage(Material->TextureMain.Texture.Image);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Meshes); Index += 1) {
		EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Index);
		UnloadMesh(Mesh->Mesh);
		AllocatorDeallocate(Archive->Allocator, Mesh->Name);
		if (Mesh->IsEffectEnabled) AllocatorDeallocate(Archive->Allocator, Mesh->EffectName);
		//AllocatorDeallocate(Archive->Allocator, Mesh->Mesh.vertices);
		//AllocatorDeallocate(Archive->Allocator, Mesh->Mesh.normals);
		//AllocatorDeallocate(Archive->Allocator, Mesh->Mesh.texcoords);
		//AllocatorDeallocate(Archive->Allocator, Mesh->Mesh.indices);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->SkinBones); Index += 1) {
		EBMSkinBoneRef Bone = (EBMSkinBoneRef)ArrayGetElementAtIndex(Archive->SkinBones, Index);
		AllocatorDeallocate(Archive->Allocator, Bone->Indices);
		AllocatorDeallocate(Archive->Allocator, Bone->Weights);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Animations); Index += 1) {
		EBMAnimationRef Animation = (EBMAnimationRef)ArrayGetElementAtIndex(Archive->Animations, Index);

		for (Int Index = 0; Index < Animation->NodeCount; Index += 1) {
			AllocatorDeallocate(Archive->Allocator, Animation->Nodes[Index].Name);
			AllocatorDeallocate(Archive->Allocator, Animation->Nodes[Index].Translations);
			AllocatorDeallocate(Archive->Allocator, Animation->Nodes[Index].Rotations);
		}

		AllocatorDeallocate(Archive->Allocator, Animation->Name);
		AllocatorDeallocate(Archive->Allocator, Animation->Nodes);
	}

	ArrayDestroy(Archive->Materials);
	ArrayDestroy(Archive->Bones);
	ArrayDestroy(Archive->Meshes);
	ArrayDestroy(Archive->SkinBones);
	ArrayDestroy(Archive->Animations);
	AllocatorDeallocate(Archive->Allocator, Archive);
}

Bool EBMArchiveLoadFromFile(
	EBMArchiveRef Archive,
	CString FilePath
) {
	UInt8* Source = NULL;
	Int32 SourceLength = 0;
	FileRef File = FileOpen(FilePath);
	if (!File) goto error;
	if (!FileRead(File, &Source, &SourceLength)) goto error;

	Int32 SourceOffset = 0;

	Archive->Magic = *(UInt8*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt8);

	Archive->Version = *(UInt16*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt16);

	Archive->Padding = *(UInt8*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt8);

	Archive->Unknown1 = *(UInt16*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt16);

	Archive->Options = *(EBMOptions*)&Source[SourceOffset];
	SourceOffset += sizeof(EBMOptions);

	Archive->BoundsMin = *(Vector3*)&Source[SourceOffset];
	SourceOffset += sizeof(Vector3);

	Archive->BoundsMax = *(Vector3*)&Source[SourceOffset];
	SourceOffset += sizeof(Vector3);

	Archive->ScalePercentage = *(Int32*)&Source[SourceOffset];
	SourceOffset += sizeof(Int32);

	Archive->Bounds.min = (Vector3){ 0, 0, 0 };
	Archive->Bounds.max = (Vector3){ 0, 0, 0 };

	Bool IsMeshEffectEnabled = Archive->Version > 1008;

	while (SourceOffset < SourceLength) {
		UInt32 ChunkType = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);

		UInt16 Count = *(UInt16*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt16);

		for (Int Index = 0; Index < Count; Index += 1) {
			if (ChunkType == EBM_CHUNK_TYPE_MATERIAL) {
				EBMMaterialRef Material = (EBMMaterialRef)ArrayAppendUninitializedElement(Archive->Materials);
				
				Material->Properties = *(EBMMaterialProperties*)&Source[SourceOffset];
				SourceOffset += sizeof(EBMMaterialProperties);

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Material->TextureMain.Texture.Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!Material->TextureMain.Texture.Name) Fatal("Memory allocation failed!");
				memcpy(Material->TextureMain.Texture.Name, &Source[SourceOffset], NameLength);
				Material->TextureMain.Texture.Name[NameLength] = '\0';
				SourceOffset += NameLength;

				Int32 DataLength = *(Int32*)&Source[SourceOffset];
				SourceOffset += sizeof(Int32);

				UInt8* Data = (UInt8*)&Source[SourceOffset];
				SourceOffset += DataLength;

				Material->TextureMain.Texture.Data = NULL;
				if (DataLength > 0) {
					Int32 Width = 0;
					Int32 Height = 0;
					if (DecompressDDSTextureFromMemory(
						Archive->Allocator,
						Data,
						DataLength,
						&Width,
						&Height,
						&Material->TextureMain.Texture.Data,
						&Material->TextureMain.Texture.Length
					)) {
						Material->TextureMain.Texture.Image = (Image){
							.data = Material->TextureMain.Texture.Data,
							.width = Width,
							.height = Height,
							.mipmaps = 1,
							.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
						};
					}
					else {
						Material->TextureMain.Texture.Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
					}
				}
				else {
					Material->TextureMain.Texture.Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
				}

				ImageFormat(&Material->TextureMain.Texture.Image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
				Material->TextureMain.Texture.Texture = LoadTextureFromImage(Material->TextureMain.Texture.Image);

				Material->TextureMain.IsFaceted = *(Bool*)&Source[SourceOffset];
				SourceOffset += sizeof(Bool);

				Material->TextureMain.ScrollSpeed = *(Vector2*)&Source[SourceOffset];
				SourceOffset += sizeof(Vector2);

				Material->TextureBlend.MaterialIndex = *(Int32*)&Source[SourceOffset];
				SourceOffset += sizeof(Int32);

				Material->TextureBlend.IsFaceted = *(Bool*)&Source[SourceOffset];
				SourceOffset += sizeof(Bool);

				Material->TextureBlend.ScrollSpeed = *(Vector2*)&Source[SourceOffset];
				SourceOffset += sizeof(Vector2);

				Material->TextureBlend.BlendFlags = *(UInt32*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt32);
			}
			else if (ChunkType == EBM_CHUNK_TYPE_SKELETON) {
				EBMBoneRef Bone = (EBMBoneRef)ArrayAppendUninitializedElement(Archive->Bones);

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Bone->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!Bone->Name) Fatal("Memory allocation failed!");
				memcpy(Bone->Name, &Source[SourceOffset], NameLength);
				Bone->Name[NameLength] = '\0'; 
				SourceOffset += NameLength;

				Bone->ParentBoneIndex = *(Int32*)&Source[SourceOffset];
				SourceOffset += sizeof(Int32);

				Bone->SpaceMatrix = *(Matrix*)&Source[SourceOffset];
				SourceOffset += sizeof(Matrix);

				Bone->ParentSpaceMatrix = *(Matrix*)&Source[SourceOffset];
				SourceOffset += sizeof(Matrix);
			}
			else if (ChunkType == EBM_CHUNK_TYPE_MESH) {
				EBMMeshRef Mesh = (EBMMeshRef)ArrayAppendUninitializedElement(Archive->Meshes);
				memset(&Mesh->Mesh, 0, sizeof(struct Mesh));

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Mesh->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!Mesh->Name) Fatal("Memory allocation failed!");
				memcpy(Mesh->Name, &Source[SourceOffset], NameLength);
				Mesh->Name[NameLength] = '\0';
				SourceOffset += NameLength;

				Mesh->WorldMatrix = *(Matrix*)&Source[SourceOffset];
				SourceOffset += sizeof(Matrix);

				Mesh->LocalMatrix = *(Matrix*)&Source[SourceOffset];
				SourceOffset += sizeof(Matrix);

				Mesh->RootBoneIndex = *(Int32*)&Source[SourceOffset];
				SourceOffset += sizeof(Int32);

				Mesh->MaterialIndex = *(UInt8*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt8);

				Mesh->Mesh.vertexCount = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Mesh->Mesh.triangleCount = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Mesh->IsEffectEnabled = IsMeshEffectEnabled;

				Mesh->EffectName = NULL;
				if (Mesh->IsEffectEnabled) {
					UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
					SourceOffset += sizeof(UInt16);

					Mesh->EffectName = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
					if (!Mesh->EffectName) Fatal("Memory allocation failed!");
					memcpy(Mesh->EffectName, &Source[SourceOffset], NameLength);
					Mesh->EffectName[NameLength] = '\0';
					SourceOffset += NameLength;
				}

				Mesh->Mesh.vertices = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
				if (!Mesh->Mesh.vertices) Fatal("Memory allocation failed!");

				Mesh->Mesh.normals = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
				if (!Mesh->Mesh.normals) Fatal("Memory allocation failed!");

				Mesh->Mesh.texcoords = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Mesh->Mesh.vertexCount * 2);
				if (!Mesh->Mesh.texcoords) Fatal("Memory allocation failed!");
				
				Mesh->Mesh.indices = (UInt16*)AllocatorAllocate(Archive->Allocator, sizeof(UInt16) * Mesh->Mesh.triangleCount * 3);
				if (!Mesh->Mesh.indices) Fatal("Memory allocation failed!");

				Float32 CenterZ = (Archive->BoundsMax.z + Archive->BoundsMin.z) / 2;

				for (Int Index = 0; Index < Mesh->Mesh.vertexCount; Index += 1) {
					EBMVertex Vertex = *(EBMVertex*)&Source[SourceOffset];
					SourceOffset += sizeof(EBMVertex);

					Vector3 Position = Vertex.Position;

					Mesh->Mesh.vertices[Index * 3 + 0] = Position.x;
					Mesh->Mesh.vertices[Index * 3 + 1] = Position.y;
					Mesh->Mesh.vertices[Index * 3 + 2] = -Position.z;// CenterZ - (Position.z - CenterZ);//-Position.z; // Archive->BoundsMin.z + (Archive->BoundsMax.z - Position.z);
					Mesh->Mesh.normals[Index * 3 + 0] = Vertex.Normal.x;
					Mesh->Mesh.normals[Index * 3 + 1] = Vertex.Normal.y;
					Mesh->Mesh.normals[Index * 3 + 2] = -Vertex.Normal.z;
					Mesh->Mesh.texcoords[Index * 2 + 0] = Vertex.UV.x;
					Mesh->Mesh.texcoords[Index * 2 + 1] = Vertex.UV.y;
				}

				for (Int Index = 0; Index < Mesh->Mesh.triangleCount; Index += 1) {
					Mesh->Mesh.indices[Index * 3 + 0] = *(UInt16*)&Source[SourceOffset];
					SourceOffset += sizeof(UInt16);
					Mesh->Mesh.indices[Index * 3 + 2] = *(UInt16*)&Source[SourceOffset];
					SourceOffset += sizeof(UInt16);
					Mesh->Mesh.indices[Index * 3 + 1] = *(UInt16*)&Source[SourceOffset];
					SourceOffset += sizeof(UInt16);
				}

				UploadMesh(&Mesh->Mesh, false);

				BoundingBox MeshBounds = GetMeshBoundingBox(Mesh->Mesh);
				Archive->Bounds.min.x = MIN(Archive->Bounds.min.x, MeshBounds.min.x);
				Archive->Bounds.min.y = MIN(Archive->Bounds.min.y, MeshBounds.min.y);
				Archive->Bounds.min.z = MIN(Archive->Bounds.min.z, MeshBounds.min.z);
				Archive->Bounds.max.x = MAX(Archive->Bounds.max.x, MeshBounds.max.x);
				Archive->Bounds.max.y = MAX(Archive->Bounds.max.y, MeshBounds.max.y);
				Archive->Bounds.max.z = MAX(Archive->Bounds.max.z, MeshBounds.max.z);

				UInt32 SubChunkType = *(UInt32*)&Source[SourceOffset];
				if (SubChunkType == EBM_CHUNK_TYPE_SKIN) {
					SourceOffset += sizeof(UInt32);

					UInt16 Count = *(UInt16*)&Source[SourceOffset];
					SourceOffset += sizeof(UInt16);

					for (Int BoneIndex = 0; BoneIndex < ArrayGetElementCount(Archive->Bones); BoneIndex += 1) {
						for (Int Index = 0; Index < Count; Index += 1) {
							EBMSkinBoneRef Skin = (EBMSkinBoneRef)ArrayAppendUninitializedElement(Archive->SkinBones);

							Skin->Count = *(Int32*)&Source[SourceOffset];
							SourceOffset += sizeof(Int32);

							Skin->Indices = (Int32*)AllocatorAllocate(Archive->Allocator, sizeof(Int32) * Skin->Count);
							if (!Skin->Indices) Fatal("Memory allocation failed!");

							Skin->Weights = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Skin->Count);
							if (!Skin->Weights) Fatal("Memory allocation failed!");

							memcpy(Skin->Indices, &Source[SourceOffset], sizeof(Int32) * Skin->Count);
							SourceOffset += sizeof(Int32) * Skin->Count;

							memcpy(Skin->Weights, &Source[SourceOffset], sizeof(Int32) * Skin->Count);
							SourceOffset += sizeof(Float32) * Skin->Count;
						}
					}					
				}
			}
			else if (ChunkType == EBM_CHUNK_TYPE_ANIMATION) {
				EBMAnimationRef Animation = (EBMAnimationRef)ArrayAppendUninitializedElement(Archive->Animations);

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Animation->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!Animation->Name) Fatal("Memory allocation failed!");
				memcpy(Animation->Name, &Source[SourceOffset], NameLength);
				Animation->Name[NameLength] = '\0';
				SourceOffset += NameLength;

				Animation->NodeCount = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Animation->Nodes = (EBMAnimationNode*)AllocatorAllocate(Archive->Allocator, sizeof(EBMAnimationNode) * Animation->NodeCount);
				if (!Animation->Nodes) Fatal("Memory allocation failed!");

				for (Int Index = 0; Index < Animation->NodeCount; Index += 1) {
					UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
					SourceOffset += sizeof(UInt16);

					Animation->Nodes[Index].Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
					if (!Animation->Nodes[Index].Name) Fatal("Memory allocation failed!");
					memcpy(Animation->Nodes[Index].Name, &Source[SourceOffset], NameLength);
					Animation->Nodes[Index].Name[NameLength] = '\0';
					SourceOffset += NameLength;

					Animation->Nodes[Index].TranslationCount = *(Int32*)&Source[SourceOffset];
					SourceOffset += sizeof(Int32);

					Animation->Nodes[Index].Translations = (EBMAnimationTranslation*)AllocatorAllocate(Archive->Allocator, sizeof(EBMAnimationTranslation) * Animation->Nodes[Index].TranslationCount);
					if (!Animation->Nodes[Index].Translations) Fatal("Memory allocation failed!");
					memcpy(Animation->Nodes[Index].Translations, &Source[SourceOffset], sizeof(EBMAnimationTranslation) * Animation->Nodes[Index].TranslationCount);
					SourceOffset += sizeof(EBMAnimationTranslation) * Animation->Nodes[Index].TranslationCount;

					Animation->Nodes[Index].RotationCount = *(Int32*)&Source[SourceOffset];
					SourceOffset += sizeof(Int32);

					Animation->Nodes[Index].Rotations = (EBMAnimationRotation*)AllocatorAllocate(Archive->Allocator, sizeof(EBMAnimationRotation) * Animation->Nodes[Index].RotationCount);
					if (!Animation->Nodes[Index].Rotations) Fatal("Memory allocation failed!");
					memcpy(Animation->Nodes[Index].Rotations, &Source[SourceOffset], sizeof(EBMAnimationRotation) * Animation->Nodes[Index].RotationCount);
					SourceOffset += sizeof(EBMAnimationRotation) * Animation->Nodes[Index].RotationCount;
				}
			}
			else {
				Warn("Unknown chunk type %d", ChunkType);
				break;
			}
		}
	}

	Float32 Temp = Archive->BoundsMin.z;
	Archive->BoundsMin.z = -Archive->BoundsMax.z;
	Archive->BoundsMax.z = -Temp;

	FileClose(File);
	free(Source);
	return true;

error:
	if (File) FileClose(File);
	if (Source) free(Source);

	return false;
}

Void EBMArchiveSetupCamera(
	EditorContextRef Context,
	EBMArchiveRef Archive
) {
	Float32 Width = Archive->BoundsMax.x - Archive->BoundsMin.x;
	Float32 Height = Archive->BoundsMax.y - Archive->BoundsMin.y;
	Float32 Depth = Archive->BoundsMax.z - Archive->BoundsMin.z;
	Float32 Distance = MAX(Width, MAX(Height, Depth)) / 100 * 3;

	Context->Camera.position = (Vector3){ 0.0f, Distance, -Distance };
	Context->Camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	Context->Camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	Context->Camera.fovy = 45.0f;
	Context->Camera.projection = CAMERA_PERSPECTIVE;
}

Void EBMArchiveDraw(
	EditorContextRef Context,
	EBMArchiveRef Archive
) {
	Material Material = LoadMaterialDefault();
	Matrix Transform = MatrixIdentity();
	Transform = MatrixMultiply(Transform, MatrixScale(1.0f / 100, 1.0f / 100, 1.0f / 100));

	Float32 Width = Archive->BoundsMax.x - Archive->BoundsMin.x;
	Float32 Height = Archive->BoundsMax.y - Archive->BoundsMin.y;
	Float32 Depth = Archive->BoundsMax.z - Archive->BoundsMin.z;

	Int CellCount = 8;
	Float32 CellSize = MAX(Width, MAX(Height, Depth)) / (100 * CellCount);
	Float32 SideLength = CellSize * CellCount;
	for (Int Offset = -CellCount; Offset <= CellCount; Offset += 1) {
		Vector3 Start = (Vector3){ -SideLength, 0, CellSize * Offset };
		Vector3 End = (Vector3){ SideLength, 0, CellSize * Offset };
		DrawLine3D(Start, End, LIME);

		Start = (Vector3){ CellSize * Offset, 0, -SideLength };
		End = (Vector3){ CellSize * Offset, 0, SideLength };
		DrawLine3D(Start, End, LIME);
	}

	Matrix ModelTransform = MatrixIdentity();
	ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(100.0f / Archive->ScalePercentage, 100.0f / Archive->ScalePercentage, 100.0f / Archive->ScalePercentage));
	ModelTransform = MatrixMultiply(ModelTransform, Transform);

	for (Int MeshIndex = 0; MeshIndex < ArrayGetElementCount(Archive->Meshes); MeshIndex += 1) {
		EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, MeshIndex);
		EBMMaterialRef MeshMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Archive->Materials, Mesh->MaterialIndex);
		SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, MeshMaterial->TextureMain.Texture.Texture);
		DrawMesh(Mesh->Mesh, Material, ModelTransform);
	}

	BoundingBox Bounds;
	Bounds.min = Vector3Transform(Archive->BoundsMin, ModelTransform);
	Bounds.max = Vector3Transform(Archive->BoundsMax, ModelTransform);
	DrawBoundingBox(Bounds, RED);
}
