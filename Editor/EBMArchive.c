#include "Context.h"
#include "EBMArchive.h"
#include "EBMShader.h"
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

EBMAnimationRef EBMArchiveGetAnimation(
	EBMArchiveRef Archive,
	CString Name
) {
	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Animations); Index += 1) {
		EBMAnimationRef Animation = (EBMAnimationRef)ArrayGetElementAtIndex(Archive->Animations, Index);
		if (CStringIsEqual(Animation->Name, Name)) return Animation;
	}

	return NULL;
}

EBMMeshRef EBMArchiveGetMesh(
	EBMArchiveRef Archive,
	CString Name
) {
	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Meshes); Index += 1) {
		EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Index);
		if (CStringIsEqual(Mesh->Name, Name)) return Mesh;
	}

	return NULL;
}

EBMBoneRef EBMArchiveGetBone(
	EBMArchiveRef Archive,
	CString Name
) {
	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Bones); Index += 1) {
		EBMBoneRef Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Index);
		if (CStringIsEqual(Bone->Name, Name)) return Bone;
	}

	return NULL;
}

Bool EBMArchiveLoadFromFile(
	Shader Shader,
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

	Archive->Version = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt32);

	Archive->Unknown1 = *(UInt8*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt8);

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
				EBMMaterialRef MeshMaterial = (EBMMaterialRef)ArrayAppendUninitializedElement(Archive->Materials);
				
				MeshMaterial->Properties = *(EBMMaterialProperties*)&Source[SourceOffset];
				SourceOffset += sizeof(EBMMaterialProperties);

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				MeshMaterial->TextureMain.Texture.Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!MeshMaterial->TextureMain.Texture.Name) Fatal("Memory allocation failed!");
				memcpy(MeshMaterial->TextureMain.Texture.Name, &Source[SourceOffset], NameLength);
				MeshMaterial->TextureMain.Texture.Name[NameLength] = '\0';
				SourceOffset += NameLength;

				Int32 DataLength = *(Int32*)&Source[SourceOffset];
				SourceOffset += sizeof(Int32);

				UInt8* Data = (UInt8*)&Source[SourceOffset];
				SourceOffset += DataLength;

				MeshMaterial->TextureMain.Texture.Data = NULL;
				if (DataLength > 0) {
					Int32 Width = 0;
					Int32 Height = 0;
					if (DecompressDDSTextureFromMemory(
						Archive->Allocator,
						Data,
						DataLength,
						&Width,
						&Height,
						&MeshMaterial->TextureMain.Texture.Data,
						&MeshMaterial->TextureMain.Texture.Length
					)) {
						MeshMaterial->TextureMain.Texture.Image = (Image){
							.data = MeshMaterial->TextureMain.Texture.Data,
							.width = Width,
							.height = Height,
							.mipmaps = 1,
							.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
						};
					}
					else {
						MeshMaterial->TextureMain.Texture.Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
					}
				}
				else {
					MeshMaterial->TextureMain.Texture.Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
				}

				ImageFormat(&MeshMaterial->TextureMain.Texture.Image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
				MeshMaterial->TextureMain.Texture.Texture = LoadTextureFromImage(MeshMaterial->TextureMain.Texture.Image);
				SetTextureFilter(MeshMaterial->TextureMain.Texture.Texture, RL_TEXTURE_FILTER_NEAREST);
				SetTextureWrap(MeshMaterial->TextureMain.Texture.Texture, RL_TEXTURE_WRAP_CLAMP);

				MeshMaterial->TextureMain.IsFaceted = *(Bool*)&Source[SourceOffset];
				SourceOffset += sizeof(Bool);

				MeshMaterial->TextureMain.ScrollSpeed = *(Vector2*)&Source[SourceOffset];
				SourceOffset += sizeof(Vector2);

				MeshMaterial->TextureBlend.MaterialIndex = *(Int32*)&Source[SourceOffset];
				SourceOffset += sizeof(Int32);

				MeshMaterial->TextureBlend.IsFaceted = *(Bool*)&Source[SourceOffset];
				SourceOffset += sizeof(Bool);

				MeshMaterial->TextureBlend.ScrollSpeed = *(Vector2*)&Source[SourceOffset];
				SourceOffset += sizeof(Vector2);

				MeshMaterial->TextureBlend.BlendFlags = *(UInt32*)&Source[SourceOffset];
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

				Bone->WorldMatrix = ConvertDirectXMatrixToOpenGL(*(Matrix*)&Source[SourceOffset]);
				SourceOffset += sizeof(Matrix);

				Bone->InverseParentWorldMatrix = MatrixInvert(ConvertDirectXMatrixToOpenGL(*(Matrix*)&Source[SourceOffset]));
				SourceOffset += sizeof(Matrix);

//				Bone->BindPoseMatrix = MatrixMultiply(Bone->InverseParentWorldMatrix, Bone->WorldMatrix);
//				Bone->OffsetMatrix = MatrixInvert(Bone->BindPoseMatrix);
//				Bone->BoneIndex = ArrayGetElementCount(Archive->Bones) - 1;
			}
			else if (ChunkType == EBM_CHUNK_TYPE_MESH) {
				EBMMeshRef Mesh = (EBMMeshRef)ArrayAppendUninitializedElement(Archive->Meshes);
				memset(Mesh, 0, sizeof(struct _EBMMesh));
				memset(&Mesh->Mesh, 0, sizeof(struct Mesh));

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Mesh->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!Mesh->Name) Fatal("Memory allocation failed!");
				memcpy(Mesh->Name, &Source[SourceOffset], NameLength);
				Mesh->Name[NameLength] = '\0';
				SourceOffset += NameLength;

				Mesh->WorldMatrix = ConvertDirectXMatrixToOpenGL(*(Matrix*)&Source[SourceOffset]);
				SourceOffset += sizeof(Matrix);

				Mesh->LocalMatrix = ConvertDirectXMatrixToOpenGL(*(Matrix*)&Source[SourceOffset]);
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
				Mesh->Mesh.animVertices = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
				if (!Mesh->Mesh.vertices || !Mesh->Mesh.animVertices) Fatal("Memory allocation failed!");

				Mesh->Mesh.normals = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
				Mesh->Mesh.animNormals = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
				if (!Mesh->Mesh.normals ||!Mesh->Mesh.animNormals) Fatal("Memory allocation failed!");

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

				memcpy(Mesh->Mesh.animVertices, Mesh->Mesh.vertices, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
				memcpy(Mesh->Mesh.animNormals, Mesh->Mesh.normals, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);

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
						EBMBoneRef Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, BoneIndex);

						for (Int Index = 0; Index < Count; Index += 1) {
							EBMSkinBoneRef Skin = (EBMSkinBoneRef)ArrayAppendUninitializedElement(Archive->SkinBones);
							Skin->MeshIndex = ArrayGetElementCount(Archive->Meshes) - 1;
							Skin->BoneIndex = BoneIndex;
							
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

				Animation->Duration = 0.0f;
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

					for (Int TranslationIndex = 0; TranslationIndex < Animation->Nodes[Index].TranslationCount; TranslationIndex += 1) {
						Animation->Nodes[Index].Translations[TranslationIndex].Position.z *= -1;
						Animation->Duration = MAX(Animation->Duration, Animation->Nodes[Index].Translations[TranslationIndex].Timestamp);
					}

					Animation->Nodes[Index].RotationCount = *(Int32*)&Source[SourceOffset];
					SourceOffset += sizeof(Int32);

					Animation->Nodes[Index].Rotations = (EBMAnimationRotation*)AllocatorAllocate(Archive->Allocator, sizeof(EBMAnimationRotation) * Animation->Nodes[Index].RotationCount);
					if (!Animation->Nodes[Index].Rotations) Fatal("Memory allocation failed!");
					memcpy(Animation->Nodes[Index].Rotations, &Source[SourceOffset], sizeof(EBMAnimationRotation) * Animation->Nodes[Index].RotationCount);
					SourceOffset += sizeof(EBMAnimationRotation) * Animation->Nodes[Index].RotationCount;

					for (Int RotationIndex = 0; RotationIndex < Animation->Nodes[Index].RotationCount; RotationIndex += 1) {
						//Animation->Nodes[Index].Rotations[RotationIndex].Rotation.x *= -1;
						//Animation->Nodes[Index].Rotations[RotationIndex].Rotation.y *= -1;
						//Animation->Nodes[Index].Rotations[RotationIndex].Rotation.z *= -1;
						//Animation->Nodes[Index].Rotations[RotationIndex].Rotation = QuaternionInvert(
						//	Animation->Nodes[Index].Rotations[RotationIndex].Rotation
						//);
						Animation->Duration = MAX(Animation->Duration, Animation->Nodes[Index].Rotations[RotationIndex].Timestamp);
					}
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

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Meshes); Index += 1) {
		EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Index);
		Mesh->RootBoneMemory.Name = Mesh->Name;
		Mesh->RootBoneMemory.ParentBoneIndex = INT_MIN;
		Mesh->RootBoneMemory.WorldMatrix = Mesh->WorldMatrix;
		Mesh->RootBoneMemory.InverseParentWorldMatrix = MatrixInvert(MatrixIdentity());
		Mesh->RootBoneMemory.ParentBone = NULL;
		Mesh->RootBoneMemory.BoneMatrix = MatrixIdentity();
		Mesh->RootBone = &Mesh->RootBoneMemory;
		if (Mesh->RootBoneIndex >= 0) Mesh->RootBone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Mesh->RootBoneIndex);
		assert(Mesh->RootBone);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Bones); Index += 1) {
		EBMBoneRef Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Index);
		Bone->ParentBone = NULL;
		Bone->BoneMatrix = MatrixIdentity();
		if (Bone->ParentBoneIndex < 0) continue;

		Bone->ParentBone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Bone->ParentBoneIndex);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->SkinBones); Index += 1) {
		EBMSkinBoneRef Skin = (EBMSkinBoneRef)ArrayGetElementAtIndex(Archive->SkinBones, Index);
		Skin->Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Skin->MeshIndex);
		Skin->Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Skin->BoneIndex);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Animations); Index += 1) {
		EBMAnimationRef Animation = (EBMAnimationRef)ArrayGetElementAtIndex(Archive->Animations, Index);
		
		for (Int NodeIndex = 0; NodeIndex < Animation->NodeCount; NodeIndex += 1) {
			EBMAnimationNodeRef AnimationNode = &Animation->Nodes[NodeIndex];
			AnimationNode->Bone = EBMArchiveGetBone(Archive, AnimationNode->Name);
			if (!AnimationNode->Bone) {
				EBMMeshRef Mesh = EBMArchiveGetMesh(Archive, AnimationNode->Name);
				assert(Mesh);
				AnimationNode->Bone = Mesh->RootBone;
			}
		}
	}

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

Matrix EBMBoneGetGlobalBoneMatrix(
	EBMArchiveRef Archive,
	EBMBoneRef Bone,
	EBMMeshRef Mesh
) {
	if (Bone->ParentBoneIndex == INT_MIN) return Bone->BoneMatrix;

	EBMBoneRef ParentBone = (Bone->ParentBone) ? Bone->ParentBone : Mesh->RootBone;
	assert(ParentBone);

	return MatrixMultiply(
		EBMBoneGetGlobalBoneMatrix(Archive, ParentBone, Mesh),
		Bone->BoneMatrix
	); 
}

Void EBMAnimationUpdate(
	EBMArchiveRef Archive,
	EBMAnimationRef Animation,
	Float32 ElapsedTime
) {
	EBMAnimationTranslation DefaultTranslation = {
		.Timestamp = 0.0f,
		.Position = Vector3Zero()
	};
	EBMAnimationRotation DefaultRotation = {
		.Timestamp = 0.0f,
		.Rotation = QuaternionIdentity()
	};

	for (Int Index = 0; Index < Animation->NodeCount; Index += 1) {
		EBMAnimationNodeRef AnimationNode = &Animation->Nodes[Index];
		Vector3 FrameTranslation = Vector3Zero();
		Quaternion FrameRotation = QuaternionIdentity();

		for (Int TranslationIndex = 0; TranslationIndex < AnimationNode->TranslationCount; TranslationIndex += 1) {
			if (AnimationNode->Translations[TranslationIndex].Timestamp <= ElapsedTime &&
				AnimationNode->TranslationCount > TranslationIndex + 1) continue;

			EBMAnimationTranslation* Source = (TranslationIndex > 0) ? &AnimationNode->Translations[TranslationIndex - 1] : &DefaultTranslation;
			EBMAnimationTranslation* Destination = &AnimationNode->Translations[TranslationIndex];

			assert((Destination->Timestamp - Source->Timestamp) > 0);
			Float32 Delta = (ElapsedTime - Source->Timestamp) / (Destination->Timestamp - Source->Timestamp);
			FrameTranslation = Vector3Lerp(Source->Position, Destination->Position, Delta);
			FrameTranslation.z *= -1;
			break;
		}

		for (Int RotationIndex = 0; RotationIndex < AnimationNode->RotationCount; RotationIndex += 1) {
			if (AnimationNode->Rotations[RotationIndex].Timestamp <= ElapsedTime &&
				AnimationNode->TranslationCount > RotationIndex + 1) continue;

			EBMAnimationRotation* Source = (RotationIndex > 0) ? &AnimationNode->Rotations[RotationIndex - 1] : &DefaultRotation;
			EBMAnimationRotation* Destination = &AnimationNode->Rotations[RotationIndex];

			assert((Destination->Timestamp - Source->Timestamp) > 0);
			Float32 Delta = (ElapsedTime - Source->Timestamp) / (Destination->Timestamp - Source->Timestamp);
			FrameRotation = QuaternionLerp(Source->Rotation, Destination->Rotation, Delta);
			FrameRotation = QuaternionInvert(FrameRotation);
			break;
		}

		AnimationNode->Bone->BoneMatrix = MatrixMultiply(
			//			MatrixMultiply(AnimationNode->Bone->WorldMatrix, AnimationNode->Bone->InverseParentWorldMatrix),
			AnimationNode->Bone->WorldMatrix,
			MatrixMultiply(
				QuaternionToMatrix(FrameRotation),
				MatrixTranslate(FrameTranslation.x, FrameTranslation.y, FrameTranslation.z)
			)
		);
	}
}

Void EBMArchiveUpdateAnimation(
	EBMArchiveRef Archive,
	CString Name,
	Float32 ElapsedTime
) {
	EBMAnimationRef Animation = EBMArchiveGetAnimation(Archive, Name);
	if (!Animation) return;
	EBMAnimationUpdate(Archive, Animation, ElapsedTime);

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Meshes); Index += 1) {
		EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Index);
		memset(Mesh->Mesh.animVertices, 0, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
		memset(Mesh->Mesh.animNormals, 0, sizeof(Float32) * Mesh->Mesh.vertexCount * 3);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->SkinBones); Index += 1) {
		EBMSkinBoneRef SkinBone = (EBMSkinBoneRef)ArrayGetElementAtIndex(Archive->SkinBones, Index);

		for (Int SkinBoneIndex = 0; SkinBoneIndex < SkinBone->Count; SkinBoneIndex += 1) {
			if (SkinBone->Weights[SkinBoneIndex] <= FLT_EPSILON) continue;

			Matrix AnimationMatrix = EBMBoneGetGlobalBoneMatrix(Archive, SkinBone->Bone, SkinBone->Mesh);
			//AnimationMatrix = MatrixMultiply(AnimationMatrix, MatrixInvert(SkinBone->Bone->WorldMatrix));
			//AnimationMatrix = MatrixMultiply(MatrixScale(0.5f, 0.5f, 0.5f), AnimationMatrix);
			//AnimatedTransform = MatrixMultiply(AnimatedTransform, MatrixRotateXYZ((Vector3) { M_PI * 0.0, M_PI * 0.0, M_PI * 1.5 }));

			//Matrix SkinningTransform = MatrixMultiply(AnimatedTransform, MatrixInvert(SkinBone->Bone->WorldMatrix));
			//Matrix SkinningTransform = EBMBoneGetAnimatedTransform2(Archive, Bone);

			Int32 VertexIndex = SkinBone->Indices[SkinBoneIndex];
			assert(VertexIndex < SkinBone->Mesh->Mesh.vertexCount);
			Vector3 Vertex = *(Vector3*)&SkinBone->Mesh->Mesh.vertices[VertexIndex * 3];
			Vector3 AnimatedVertex = Vector3Transform(Vertex, AnimationMatrix);
			SkinBone->Mesh->Mesh.animVertices[VertexIndex * 3 + 0] += AnimatedVertex.x * SkinBone->Weights[SkinBoneIndex];
			SkinBone->Mesh->Mesh.animVertices[VertexIndex * 3 + 1] += AnimatedVertex.y * SkinBone->Weights[SkinBoneIndex];
			SkinBone->Mesh->Mesh.animVertices[VertexIndex * 3 + 2] += AnimatedVertex.z * SkinBone->Weights[SkinBoneIndex];

			Vector3 Normal = *(Vector3*)&SkinBone->Mesh->Mesh.normals[VertexIndex * 3];
			Vector3 AnimatedNormal = Vector3Normalize(Vector3Transform(Normal, MatrixTranspose(MatrixInvert(AnimationMatrix))));
			SkinBone->Mesh->Mesh.animNormals[VertexIndex * 3 + 0] += AnimatedNormal.x * SkinBone->Weights[SkinBoneIndex];
			SkinBone->Mesh->Mesh.animNormals[VertexIndex * 3 + 1] += AnimatedNormal.y * SkinBone->Weights[SkinBoneIndex];
			SkinBone->Mesh->Mesh.animNormals[VertexIndex * 3 + 2] += AnimatedNormal.z * SkinBone->Weights[SkinBoneIndex];
		}
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Meshes); Index += 1) {
		EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, Index);
		UpdateMeshBuffer(Mesh->Mesh, 0, Mesh->Mesh.animVertices, sizeof(Float32) * Mesh->Mesh.vertexCount * 3, 0);
		UpdateMeshBuffer(Mesh->Mesh, 2, Mesh->Mesh.animNormals, sizeof(Float32) * Mesh->Mesh.vertexCount * 3, 0);
	}

	/*
	void UpdateModelAnimationBones(Model model, ModelAnimation anim, int frame)
	{
		if ((anim.frameCount > 0) && (anim.bones != NULL) && (anim.framePoses != NULL))
		{
			if (frame >= anim.frameCount) frame = frame % anim.frameCount;

			// Get first mesh which have bones
			int firstMeshWithBones = -1;

			for (int i = 0; i < model.meshCount; i++)
			{
				if (model.meshes[i].boneMatrices)
				{
					if (firstMeshWithBones == -1)
					{
						firstMeshWithBones = i;
						break;
					}
				}
			}

			// Update all bones and boneMatrices of first mesh with bones.
			for (int boneId = 0; boneId < anim.boneCount; boneId++)
			{
				Vector3 inTranslation = model.bindPose[boneId].translation; // 
				Quaternion inRotation = model.bindPose[boneId].rotation;
				Vector3 inScale = model.bindPose[boneId].scale;

				Vector3 outTranslation = anim.framePoses[frame][boneId].translation; // TransformMatrix
				Quaternion outRotation = anim.framePoses[frame][boneId].rotation;
				Vector3 outScale = anim.framePoses[frame][boneId].scale;

				Quaternion invRotation = QuaternionInvert(inRotation);
				Vector3 invTranslation = Vector3RotateByQuaternion(Vector3Negate(inTranslation), invRotation);
				Vector3 invScale = Vector3Divide((Vector3) { 1.0f, 1.0f, 1.0f }, inScale);

				Vector3 boneTranslation = Vector3Add(Vector3RotateByQuaternion(
					Vector3Multiply(outScale, invTranslation), outRotation), outTranslation);
				Quaternion boneRotation = QuaternionMultiply(outRotation, invRotation);
				Vector3 boneScale = Vector3Multiply(outScale, invScale);

				Matrix boneMatrix = MatrixMultiply(MatrixMultiply(
					QuaternionToMatrix(boneRotation),
					MatrixTranslate(boneTranslation.x, boneTranslation.y, boneTranslation.z)),
					MatrixScale(boneScale.x, boneScale.y, boneScale.z));

				model.meshes[firstMeshWithBones].boneMatrices[boneId] = boneMatrix;
			}

			// Update remaining meshes with bones
			// NOTE: Using deep copy because shallow copy results in double free with 'UnloadModel()'
			if (firstMeshWithBones != -1)
			{
				for (int i = firstMeshWithBones + 1; i < model.meshCount; i++)
				{
					if (model.meshes[i].boneMatrices)
					{
						memcpy(model.meshes[i].boneMatrices,
							model.meshes[firstMeshWithBones].boneMatrices,
							model.meshes[i].boneCount * sizeof(model.meshes[i].boneMatrices[0]));
					}
				}
			}
		}
	}

	void UpdateModelAnimation(Model model, ModelAnimation anim, int frame)
	{
		UpdateModelAnimationBones(model, anim, frame);

		for (int m = 0; m < model.meshCount; m++)
		{
			Mesh mesh = model.meshes[m];
			Vector3 animVertex = { 0 };
			Vector3 animNormal = { 0 };
			int boneId = 0;
			int boneCounter = 0;
			float boneWeight = 0.0;
			bool updated = false; // Flag to check when anim vertex information is updated
			const int vValues = mesh.vertexCount * 3;

			// Skip if missing bone data, causes segfault without on some models
			if ((mesh.boneWeights == NULL) || (mesh.boneIds == NULL)) continue;

			for (int vCounter = 0; vCounter < vValues; vCounter += 3)
			{
				mesh.animVertices[vCounter] = 0;
				mesh.animVertices[vCounter + 1] = 0;
				mesh.animVertices[vCounter + 2] = 0;
				if (mesh.animNormals != NULL)
				{
					mesh.animNormals[vCounter] = 0;
					mesh.animNormals[vCounter + 1] = 0;
					mesh.animNormals[vCounter + 2] = 0;
				}

				// Iterates over 4 bones per vertex
				for (int j = 0; j < 4; j++, boneCounter++)
				{
					boneWeight = mesh.boneWeights[boneCounter];
					boneId = mesh.boneIds[boneCounter];

					// Early stop when no transformation will be applied
					if (boneWeight == 0.0f) continue;
					animVertex = (Vector3){ mesh.vertices[vCounter], mesh.vertices[vCounter + 1], mesh.vertices[vCounter + 2] };
					animVertex = Vector3Transform(animVertex, model.meshes[m].boneMatrices[boneId]);
					mesh.animVertices[vCounter] += animVertex.x * boneWeight;
					mesh.animVertices[vCounter + 1] += animVertex.y * boneWeight;
					mesh.animVertices[vCounter + 2] += animVertex.z * boneWeight;
					updated = true;

					// Normals processing
					// NOTE: We use meshes.baseNormals (default normal) to calculate meshes.normals (animated normals)
					if ((mesh.normals != NULL) && (mesh.animNormals != NULL))
					{
						animNormal = (Vector3){ mesh.normals[vCounter], mesh.normals[vCounter + 1], mesh.normals[vCounter + 2] };
						animNormal = Vector3Transform(animNormal, MatrixTranspose(MatrixInvert(model.meshes[m].boneMatrices[boneId])));
						mesh.animNormals[vCounter] += animNormal.x * boneWeight;
						mesh.animNormals[vCounter + 1] += animNormal.y * boneWeight;
						mesh.animNormals[vCounter + 2] += animNormal.z * boneWeight;
					}
				}
			}

			if (updated)
			{
				rlUpdateVertexBuffer(mesh.vboId[0], mesh.animVertices, mesh.vertexCount * 3 * sizeof(float), 0); // Update vertex position
				if (mesh.normals != NULL) rlUpdateVertexBuffer(mesh.vboId[2], mesh.animNormals, mesh.vertexCount * 3 * sizeof(float), 0); // Update vertex normals
			}
		}
	}
	*/
}

void DrawMeshWireframe(Mesh mesh, Matrix transform) {
	for (int i = 0; i < mesh.triangleCount; i += 1) {
		// Each triangle has 3 indices (vertices)
		int index1 = mesh.indices[i * 3];
		int index2 = mesh.indices[i * 3 + 1];
		int index3 = mesh.indices[i * 3 + 2];

		// Get the positions of the vertices based on the indices
		Vector3 v1 = Vector3Transform(*(Vector3*)&mesh.vertices[index1 * 3], transform);
		Vector3 v2 = Vector3Transform(*(Vector3*)&mesh.vertices[index2 * 3], transform);
		Vector3 v3 = Vector3Transform(*(Vector3*)&mesh.vertices[index3 * 3], transform);

		// Draw lines between the vertices of the triangle
		DrawLine3D(v1, v2, RED);  // Edge between v1 and v2
		DrawLine3D(v2, v3, RED);  // Edge between v2 and v3
		DrawLine3D(v3, v1, RED);  // Edge between v3 and v1
	}
}

static void DrawTextCodepoint3D(EditorContextRef Context, Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
	// Character index position in sprite font
	// NOTE: In case a codepoint is not available in the font, index returned points to '?'
	int index = GetGlyphIndex(font, codepoint);
	float scale = fontSize / (float)font.baseSize;

	// Character destination rectangle on screen
	// NOTE: We consider charsPadding on drawing
	position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding) / (float)font.baseSize * scale;
	position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding) / (float)font.baseSize * scale;

	// Character source rectangle from font texture atlas
	// NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
	Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
						 font.recs[index].width + 2.0f * font.glyphPadding, font.recs[index].height + 2.0f * font.glyphPadding };

	float width = (float)(font.recs[index].width + 2.0f * font.glyphPadding) / (float)font.baseSize * scale;
	float height = (float)(font.recs[index].height + 2.0f * font.glyphPadding) / (float)font.baseSize * scale;

	if (font.texture.id > 0)
	{
		const float x = 0.0f;
		const float y = 0.0f;
		const float z = 0.0f;

		// normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
		const float tx = srcRec.x / font.texture.width;
		const float ty = srcRec.y / font.texture.height;
		const float tw = (srcRec.x + srcRec.width) / font.texture.width;
		const float th = (srcRec.y + srcRec.height) / font.texture.height;

		rlCheckRenderBatchLimit(4 + 4 * backface);
		rlSetTexture(font.texture.id);

		rlPushMatrix();
		rlTranslatef(position.x, position.y, position.z);

		rlBegin(RL_QUADS);
		rlColor4ub(tint.r, tint.g, tint.b, tint.a);

		// Front Face
		rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
		rlTexCoord2f(tx, th); rlVertex3f(x, y, z);              // Top Left Of The Texture and Quad
		rlTexCoord2f(tx, ty); rlVertex3f(x, y + height, z);     // Bottom Left Of The Texture and Quad
		rlTexCoord2f(tw, ty); rlVertex3f(x + width, y + height, z);     // Bottom Right Of The Texture and Quad
		rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

		if (backface)
		{
			// Back Face
			rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
			rlTexCoord2f(tx, th); rlVertex3f(x, y + height, z);          // Top Right Of The Texture and Quad
			rlTexCoord2f(tw, th); rlVertex3f(x + width, y + height, z);          // Top Left Of The Texture and Quad
			rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z); // Bottom Left Of The Texture and Quad
			rlTexCoord2f(tx, ty); rlVertex3f(x, y, z); // Bottom Right Of The Texture and Quad
		}
		rlEnd();

		rlPopMatrix();

		rlSetTexture(0);
	}
}

static void DrawText3D(EditorContextRef Context, Font font, const char* text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint)
{
	int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

	float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
	float textOffsetX = 0.0f;               // Offset X to next character to draw

	float scale = fontSize / (float)font.baseSize;

	Matrix Transform = MatrixLookAt(Context->Camera.position, Context->Camera.target, GetCameraUp(&Context->Camera));

	rlPushMatrix();

	for (int i = 0; i < length;)
	{
		// Get next codepoint from byte string and glyph index in font
		int codepointByteCount = 0;
		int codepoint = GetCodepoint(&text[i], &codepointByteCount);
		int index = GetGlyphIndex(font, codepoint);

		// NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
		// but we need to draw all of the bad bytes using the '?' symbol moving one byte
		if (codepoint == 0x3f) codepointByteCount = 1;

		if (codepoint == '\n')
		{
			// NOTE: Fixed line spacing of 1.5 line-height
			// TODO: Support custom line spacing defined by user
			textOffsetY += scale + lineSpacing / (float)font.baseSize * scale;
			textOffsetX = 0.0f;
		}
		else
		{
			if ((codepoint != ' ') && (codepoint != '\t'))
			{
				DrawTextCodepoint3D(
					Context, 
					font, 
					codepoint, 
					(Vector3) { position.x + textOffsetX, position.y, position.z + textOffsetY }, 
					fontSize, 
					backface, 
					tint
				);
			}

			if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing) / (float)font.baseSize * scale;
			else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing) / (float)font.baseSize * scale;
		}

		i += codepointByteCount;   // Move text bytes counter to next codepoint
	}
	rlPopMatrix();
}
/*
Void EBMArchiveDrawBone(
	EditorContextRef Context, 
	EBMArchiveRef Archive, 
	EBMBoneRef Parent, 
	EBMBoneRef Bone, 
	Matrix Transform,
	Int32 Depth
) {
	Vector3 ParentPosition = { 0, 0, 0 };
	if (Parent) {
		Matrix ParentTransform = EBMBoneGetGlobalBoneMatrix(Archive, Parent);
		ParentPosition = Vector3Transform(ParentPosition, MatrixMultiply(ParentTransform, Transform));
	}

	Vector3 BonePosition = { 0, 0, 0 };
	Matrix BoneTransform = EBMBoneGetAnimatedTransform(Archive, Bone);
	BonePosition = Vector3Transform(BonePosition, MatrixMultiply(BoneTransform, Transform));

	// Draw a line between parent and child
	DrawLine3D(ParentPosition, BonePosition, DARKPURPLE);

	// Visualize bone orientation (local axes)
	Vector3 XAxis = { 0.1f, 0, 0 };
	Vector3 YAxis = { 0, 0.1f, 0 };
	Vector3 ZAxis = { 0, 0, 0.1f };

	// Transform axes to bone space
	Vector3 BoneX = Vector3Transform(XAxis, BoneTransform);
	Vector3 BoneY = Vector3Transform(YAxis, BoneTransform);
	Vector3 BoneZ = Vector3Transform(ZAxis, BoneTransform);

	// Draw local axes (red = X, green = Y, blue = Z)
	//DrawLine3D(BonePosition, Vector3Add(BonePosition, BoneX), RED);
	//DrawLine3D(BonePosition, Vector3Add(BonePosition, BoneY), GREEN);
	//DrawLine3D(BonePosition, Vector3Add(BonePosition, BoneZ), BLUE);

	Color Colors[] = {
		LIGHTGRAY,
		GRAY,
		DARKGRAY,
		YELLOW,
		GOLD,
		ORANGE,
		PINK,
		RED,
		MAROON,
		GREEN,
		LIME,
		DARKGREEN,
		SKYBLUE,
		BLUE,
		DARKBLUE,
		PURPLE,
		VIOLET,
		DARKPURPLE,
		BEIGE,
		BROWN,
		DARKBROWN,
	};

	// Draw a small sphere at the bone position
	DrawSphere(BonePosition, 0.04f, Colors[Depth]);

	//DrawText3D(Context, GetFontDefault(), Bone->Name, BonePosition, 2, 0, 0, false, DARKPURPLE);

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Bones); Index += 1) {
		EBMBoneRef Child = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Index);
		if (Child->ParentBoneIndex != Bone->BoneIndex) continue;

		EBMArchiveDrawBone(Context, Archive, Bone, Child, Transform, Depth + 1);
	}
}
*/
Void EBMArchiveDraw(
	EditorContextRef Context,
	EBMArchiveRef Archive
) {
	if (Context->AnimationIndex >= 0) {
		EBMAnimationRef Animation = (EBMAnimationRef)ArrayGetElementAtIndex(Archive->Animations, Context->AnimationIndex);
		Archive->ElapsedTime += GetFrameTime();
		Archive->ElapsedTime = fmodf(Archive->ElapsedTime, Animation->Duration);
		EBMArchiveUpdateAnimation(Archive, Animation->Name, Archive->ElapsedTime);
	}

	Material Material = LoadMaterialDefault();
	Matrix Transform = MatrixIdentity();
	Transform = MatrixMultiply(Transform, MatrixScale(1.0f / 100, 1.0f / 100, 1.0f / 100));
	Transform = MatrixMultiply(Transform, MatrixScale((Float32)Archive->ScalePercentage / 100, (Float32)Archive->ScalePercentage / 100, (Float32)Archive->ScalePercentage / 100));

	Float32 Width = Archive->BoundsMax.x - Archive->BoundsMin.x;
	Float32 Height = Archive->BoundsMax.y - Archive->BoundsMin.y;
	Float32 Depth = Archive->BoundsMax.z - Archive->BoundsMin.z;

	Int CellCount = 8;
	Float32 CellSize = MAX(Width, MAX(Height, Depth)) * Archive->ScalePercentage / (100 * 100 * CellCount);
	Float32 SideLength = CellSize * CellCount;
	for (Int Offset = -CellCount; Offset <= CellCount; Offset += 1) {
		Vector3 Start = (Vector3){ -SideLength, 0, CellSize * Offset };
		Vector3 End = (Vector3){ SideLength, 0, CellSize * Offset };
		DrawLine3D(Start, End, LIME);

		Start = (Vector3){ CellSize * Offset, 0, -SideLength };
		End = (Vector3){ CellSize * Offset, 0, SideLength };
		DrawLine3D(Start, End, LIME);
	}

	BeginShaderMode(Context->ShaderEBM);
	{
		for (Int MeshIndex = 0; MeshIndex < ArrayGetElementCount(Archive->Meshes); MeshIndex += 1) {
			EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Archive->Meshes, MeshIndex);
			Matrix MeshTransform = MatrixIdentity();
			//MeshTransform = MatrixMultiply(MeshTransform, QuaternionToMatrix(Mesh->AnimatedRotation));
			//MeshTransform = MatrixMultiply(MeshTransform, MatrixTranslate(Mesh->AnimatedPosition.x, Mesh->AnimatedPosition.y, Mesh->AnimatedPosition.z));
			MeshTransform = MatrixMultiply(MeshTransform, Transform);

			EBMMaterialRef MeshMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Archive->Materials, Mesh->MaterialIndex);
			Material.shader = Context->ShaderEBM;

			SetTextureWrap(MeshMaterial->TextureMain.Texture.Texture, RL_TEXTURE_WRAP_CLAMP);
			SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, MeshMaterial->TextureMain.Texture.Texture);

			EBMMaterialRef BlendMaterial = NULL;
			if (MeshMaterial->TextureBlend.MaterialIndex >= 0) {
				BlendMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Archive->Materials, MeshMaterial->TextureBlend.MaterialIndex);
				SetTextureWrap(MeshMaterial->TextureMain.Texture.Texture, RL_TEXTURE_WRAP_REPEAT);
				SetMaterialTexture(&Material, MATERIAL_MAP_SPECULAR, BlendMaterial->TextureMain.Texture.Texture);
			}
			else {
				Texture Default = { 0 };
				SetTextureWrap(MeshMaterial->TextureMain.Texture.Texture, RL_TEXTURE_WRAP_REPEAT);
				SetMaterialTexture(&Material, MATERIAL_MAP_SPECULAR, Default);
			}

			EBMShaderSetMaterial(Context, Context->ShaderEBM, MeshMaterial, BlendMaterial, MeshTransform);
			DrawMesh(Mesh->Mesh, Material, MeshTransform);

			if (Context->ModelIndex == MeshIndex) {
				EndShaderMode();
				DrawMeshWireframe(Mesh->Mesh, MeshTransform);
				BeginShaderMode(Context->ShaderEBM);
			}
		}
	}
	EndShaderMode();
	/*
	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Bones); Index += 1) {
		EBMBoneRef Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Index);

		Vector3 AnimatedPosition = { 0, 0, 0 };
		Matrix AnimatedTransform = EBMBoneGetAnimatedTransform(Archive, Bone);
		AnimatedPosition = Vector3Transform(AnimatedPosition, MatrixMultiply(AnimatedTransform, Transform));
		//DrawSphere(AnimatedPosition, 0.05f, RED);
	}
	*/
	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Bones); Index += 1) {
		EBMBoneRef Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Index);

		Vector3 AnimatedPosition = { 0, 0, 0 };
		AnimatedPosition = Vector3Transform(AnimatedPosition, MatrixMultiply(Bone->WorldMatrix, Transform));
		//DrawSphere(AnimatedPosition, 0.05f, BLUE);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Bones); Index += 1) {
		EBMBoneRef Bone = (EBMBoneRef)ArrayGetElementAtIndex(Archive->Bones, Index);
		if (Bone->ParentBoneIndex >= 0) continue;

		//EBMArchiveDrawBone(Context, Archive, NULL, Bone, Transform, 0);
	}

	Transform = MatrixIdentity();
	Transform = MatrixMultiply(Transform, MatrixScale(1.0f / 100, 1.0f / 100, 1.0f / 100));

	BoundingBox Bounds;
	Bounds.min = Vector3Transform(Archive->BoundsMin, Transform);
	Bounds.max = Vector3Transform(Archive->BoundsMax, Transform);
	DrawBoundingBox(Bounds, RED);
}
