#include "Context.h"
#include "MCLArchive.h"
#include "MeshUtil.h"

MCLArchiveRef MCLArchiveCreate(
	AllocatorRef Allocator
) {
	MCLArchiveRef Archive = (MCLArchiveRef)AllocatorAllocate(Allocator, sizeof(struct _MCLArchive));
	memset(Archive, 0, sizeof(struct _MCLArchive));
	Archive->Allocator = Allocator;
	Archive->Effects = ArrayCreateEmpty(Allocator, sizeof(struct _MCLEffect), 8);
	Archive->Textures = ArrayCreateEmpty(Allocator, sizeof(struct _MCLTexture), 8);
	Archive->TileTextures = ArrayCreateEmpty(Allocator, sizeof(struct _MCLTexture), 8);
	Archive->Tiles = ArrayCreateEmpty(Allocator, sizeof(struct _MCLTile), 8);
	Archive->Models = ArrayCreateEmpty(Allocator, sizeof(struct _MCLModel), 8);
	Archive->InteractiveObjects = ArrayCreateEmpty(Allocator, sizeof(struct _MCLInteractiveObject), 8);
	Archive->StaticObjects = ArrayCreateEmpty(Allocator, sizeof(struct _MCLObject), 8);
	Archive->EnvironmentObjects = ArrayCreateEmpty(Allocator, sizeof(struct _MCLObject), 8);
	Archive->SoundGroups = ArrayCreateEmpty(Allocator, sizeof(struct _MCLSoundGroup), 8);
	return Archive;
}

// TODO: Fix memory leaks!
Void MCLArchiveDestroy(
	MCLArchiveRef Archive
) {
	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->Effects); Index += 1) {
		MCLEffectRef Effect = (MCLEffectRef)ArrayGetElementAtIndex(Archive->Effects, Index);
		AllocatorDeallocate(Archive->Allocator, Effect->Name);
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->Textures); Index += 1) {
		MCLTextureRef Texture = (MCLTextureRef)ArrayGetElementAtIndex(Archive->Textures, Index);
		if (Texture->Length > 0) {
			UnloadTexture(Texture->Texture);
			UnloadImage(Texture->Image);
			//if (Texture->Data) AllocatorDeallocate(Archive->Allocator, Texture->Data);
		}
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->TileTextures); Index += 1) {
		MCLTextureRef Texture = (MCLTextureRef)ArrayGetElementAtIndex(Archive->TileTextures, Index);
		AllocatorDeallocate(Archive->Allocator, Texture->Data);
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->Tiles); Index += 1) {
		MCLTileRef Tile = (MCLTileRef)ArrayGetElementAtIndex(Archive->Tiles, Index);

		if (Tile->HasLayerColor) {
			for (Int Index = 0; Index < Tile->MeshColor.MeshCount; Index += 1) {
				UnloadMesh(Tile->MeshColor.Meshes[Index]);
			}

			AllocatorDeallocate(Archive->Allocator, Tile->MeshColor.Vertices);
			AllocatorDeallocate(Archive->Allocator, Tile->MeshColor.Colors);
			AllocatorDeallocate(Archive->Allocator, Tile->MeshColor.UVs);
			AllocatorDeallocate(Archive->Allocator, Tile->MeshColor.Indices);
			AllocatorDeallocate(Archive->Allocator, Tile->MeshColor.Meshes);
			AllocatorDeallocate(Archive->Allocator, Tile->MeshColor.Textures);
		}

		if (Tile->HasLayer) {
			for (Int Index = 0; Index < Tile->Mesh.MeshCount; Index += 1) {
				UnloadMesh(Tile->Mesh.Meshes[Index]);
			}

			AllocatorDeallocate(Archive->Allocator, Tile->Mesh.Vertices);
			AllocatorDeallocate(Archive->Allocator, Tile->Mesh.Colors);
			AllocatorDeallocate(Archive->Allocator, Tile->Mesh.UVs);
			AllocatorDeallocate(Archive->Allocator, Tile->Mesh.Indices);
			AllocatorDeallocate(Archive->Allocator, Tile->Mesh.Meshes);
			AllocatorDeallocate(Archive->Allocator, Tile->Mesh.Textures);
		}
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->Models); Index += 1) {
		MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Index);
		EBMArchiveDestroy(Model->Archive);
		AllocatorDeallocate(Archive->Allocator, Model->Name);
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->InteractiveObjects); Index += 1) {
		MCLInteractiveObjectRef Object = (MCLInteractiveObjectRef)ArrayGetElementAtIndex(Archive->InteractiveObjects, Index);
		AllocatorDeallocate(Archive->Allocator, Object->Name);
	}

	for (Int32 Index = 0; Index < ArrayGetElementCount(Archive->SoundGroups); Index += 1) {
		MCLSoundGroupRef SoundGroup = (MCLSoundGroupRef)ArrayGetElementAtIndex(Archive->SoundGroups, Index);
		for (Int32 SoundIndex = 0; SoundIndex < SoundGroup->SoundCount; SoundIndex += 1) {
			AllocatorDeallocate(Archive->Allocator, SoundGroup->Sounds[SoundIndex].Name);
		}
	}

	ArrayDestroy(Archive->Effects);
	ArrayDestroy(Archive->Textures);
	ArrayDestroy(Archive->TileTextures);
	ArrayDestroy(Archive->Tiles);
	ArrayDestroy(Archive->Models);
	ArrayDestroy(Archive->InteractiveObjects);
	ArrayDestroy(Archive->StaticObjects);
	ArrayDestroy(Archive->EnvironmentObjects);
	ArrayDestroy(Archive->SoundGroups);
	AllocatorDeallocate(Archive->Allocator, Archive);
}

Bool MCLArchiveLoadFromFile(
	EditorContextRef Context,
	MCLArchiveRef Archive,
	CString FilePath
) {
	UInt8* Source = NULL;
	Int32 SourceLength = 0;
	FileRef File = FileOpen(FilePath);
	if (!File) goto error;
	if (!FileRead(File, &Source, &SourceLength)) goto error;

	Int32 SourceOffset = 0;

	Archive->Version = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(Archive->Version);

	Archive->Unknown1 = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(Archive->Unknown1);

	memcpy(Archive->Environment, &Source[SourceOffset], sizeof(Archive->Environment));
	SourceOffset += sizeof(Archive->Environment);

	if (Archive->Version >= 1006) {
		Archive->Unknown3 = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(Archive->Unknown3);
	}

	UInt32 EffectCount = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt32);

	for (Int Index = 0; Index < EffectCount; Index += 1) {
		MCLEffectRef Effect = (MCLEffectRef)ArrayAppendUninitializedElement(Archive->Effects);

		UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt16);

		Effect->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
		if (!Effect->Name) Fatal("Memory allocation failed!");
		memcpy(Effect->Name, &Source[SourceOffset], NameLength);
		Effect->Name[NameLength] = '\0';
		SourceOffset += NameLength;

		Effect->Unknown1 = *(Float32*)&Source[SourceOffset];
		SourceOffset += sizeof(Effect->Unknown1);

		Effect->Unknown2 = *(Float32*)&Source[SourceOffset];
		SourceOffset += sizeof(Effect->Unknown2);

		Effect->Unknown3 = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(Effect->Unknown3);

		Effect->Unknown4 = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(Effect->Unknown4);
	}

	UInt32 TextureCount = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt32);

	for (Int Index = 0; Index < TextureCount; Index += 1) {
		MCLTextureRef Texture = (MCLTextureRef)ArrayAppendUninitializedElement(Archive->Textures);
		memset(Texture, 0, sizeof(struct _MCLTexture));

		UInt32 Length = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);

		UInt8* Data = (UInt8*)&Source[SourceOffset];
		SourceOffset += Length;

		Texture->Data = NULL;
		if (Length > 0) {
			Int32 Width = 0;
			Int32 Height = 0;
			if (DecompressDDSTextureFromMemory(
				Archive->Allocator,
				Data,
				Length,
				&Width,
				&Height,
				&Texture->Data,
				&Texture->Length
			)) {
				Texture->Image = (Image){
					.data = Texture->Data,
					.width = Width,
					.height = Height,
					.mipmaps = 1,
					.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
				};
			}
			else {
				Texture->Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
			}
		}
		else {
			Texture->Image = GenImageColor(32, 32, (Color){ 0, 0, 0, 0 });
		}

		ImageFormat(&Texture->Image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
		Texture->Texture = LoadTextureFromImage(Texture->Image);
		SetTextureWrap(Texture->Texture, TEXTURE_WRAP_REPEAT);
	}

	Archive->Unknown4 = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(Archive->Unknown4);

	Archive->Unknown5 = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(Archive->Unknown5);

	Archive->Unknown6 = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(Archive->Unknown6);

	memcpy(Archive->HeightMap, &Source[SourceOffset], sizeof(Archive->HeightMap));
	SourceOffset += sizeof(Archive->HeightMap);

	// Create a heightmap array (example with random values)
	Float32 MinHeight = 0.0f;
	Float32 MaxHeight = 0.0f;
	for (int i = 0; i < 257 * 257; i++)
	{
		MinHeight = MIN(MinHeight, Archive->HeightMap[i]);
		MaxHeight = MAX(MaxHeight, Archive->HeightMap[i]);
	}

	// Convert HeightMap to an Image (grayscale)
	Image heightmapImage = {
		.data = malloc(257 * 257),
		.width = 257,
		.height = 257,
		.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
		.mipmaps = 1
	};

	for (int y = 0; y < 257; y++)
	{
		for (int x = 0; x < 257; x++)
		{
			int index1 = (256 - y) + x * 257;
			int index2 = x + y * 257;
			((unsigned char*)heightmapImage.data)[index2] = (unsigned char)(
				(Archive->HeightMap[index1] - MinHeight) / (MaxHeight - MinHeight) * 255.0f
			);
		}
	}

	// Generate mesh and model from heightmap
	Mesh heightmapMesh = GenMeshHeightmap(heightmapImage, (Vector3) { 25600.0f, 25600.0f / 16, 25600.0f});
	Archive->HeightMapModel = LoadModelFromMesh(heightmapMesh);

	// Set a texture for the heightmap model (optional)
	Texture2D heightmapTexture = LoadTextureFromImage(heightmapImage);
	Archive->HeightMapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = heightmapTexture;

	// Clean up heightmapImage data (no longer needed)
	UnloadImage(heightmapImage);

	memcpy(Archive->ThreadMap, &Source[SourceOffset], sizeof(Archive->ThreadMap));
	SourceOffset += sizeof(Archive->ThreadMap);

	for (Int Index = 0; Index < 64; Index += 1) {
		MCLTextureRef Texture = (MCLTextureRef)ArrayAppendUninitializedElement(Archive->TileTextures);

		UInt32 Length = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);

		UInt8* Data = (UInt8*)&Source[SourceOffset];
		SourceOffset += Length;

		if (Length > 0) {
			Int32 Width = 0;
			Int32 Height = 0;
			if (DecompressDDSTextureFromMemory(
				Archive->Allocator,
				Data,
				Length,
				&Width,
				&Height,
				&Texture->Data,
				&Texture->Length
			)) {
				Texture->Image = (Image){
					.data = Texture->Data,
					.width = Width,
					.height = Height,
					.mipmaps = 1,
					.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
				};
			}
			else {
				Texture->Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
			}
		}
		else {
			Texture->Image = GenImageColor(32, 32, (Color) { 0, 0, 0, 0 });
		}

		ImageFormat(&Texture->Image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
		Texture->Texture = LoadTextureFromImage(Texture->Image);
		SetTextureWrap(Texture->Texture, TEXTURE_WRAP_REPEAT);
	}

	for (Int Index = 0; Index < 64; Index += 1) {
		MCLTileRef Tile = (MCLTileRef)ArrayAppendUninitializedElement(Archive->Tiles);
		memset(Tile, 0, sizeof(struct _MCLTile));

		Tile->BoundsMin = *(Vector3*)&Source[SourceOffset];
		SourceOffset += sizeof(Tile->BoundsMin);

		Tile->BoundsMax = *(Vector3*)&Source[SourceOffset];
		SourceOffset += sizeof(Tile->BoundsMax);

		Float32 Temp = Tile->BoundsMax.z;
		Tile->BoundsMax.z = -Tile->BoundsMin.z;
		Tile->BoundsMin.z = -Temp;

		Int32 HasLayerColor = *(Int32*)&Source[SourceOffset];
		SourceOffset += sizeof(Int32);

		Tile->HasLayerColor = HasLayerColor != 0;
		if (Tile->HasLayerColor) {
			UInt32 FaceCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);

			UInt32 VertexCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);

			MCLVertexColor* Vertices = (MCLVertexColor*)&Source[SourceOffset];
			SourceOffset += sizeof(MCLVertexColor) * VertexCount;

			MCLFace* Faces = (MCLFace*)&Source[SourceOffset];
			SourceOffset += sizeof(MCLFace) * FaceCount;

			Int32* FaceTextureIDs = (Int32*)&Source[SourceOffset];
			SourceOffset += sizeof(Int32) * FaceCount;
			
			UInt32 UsedTextureIDCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);
			Int32* UsedFaceTextureIDs = (Int32*)&Source[SourceOffset];
			SourceOffset += sizeof(Int32) * UsedTextureIDCount;

			Tile->MeshColor.VertexCount = (Int32)VertexCount;
			Tile->MeshColor.Vertices = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Vector3) * VertexCount);
			if (!Tile->MeshColor.Vertices) Fatal("Memory allocation failed!");
			
			Tile->MeshColor.Colors = (UInt8*)AllocatorAllocate(Archive->Allocator, sizeof(UInt32) * VertexCount);
			if (!Tile->MeshColor.Colors) Fatal("Memory allocation failed!");

			Tile->MeshColor.UVs = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Vector2) * VertexCount);
			if (!Tile->MeshColor.UVs) Fatal("Memory allocation failed!");

			Tile->MeshColor.TriangleCount = FaceCount;
			Tile->MeshColor.Indices = (UInt16*)AllocatorAllocate(Archive->Allocator, sizeof(MCLFace) * FaceCount);
			if (!Tile->MeshColor.Indices) Fatal("Memory allocation failed!");

			for (Int Index = 0; Index < VertexCount; Index += 1) {
				Tile->MeshColor.Vertices[Index * 3 + 0] = Vertices[Index].Position.x;
				Tile->MeshColor.Vertices[Index * 3 + 1] = Vertices[Index].Position.y;
				Tile->MeshColor.Vertices[Index * 3 + 2] = 25600.0f - Vertices[Index].Position.z;
				Tile->MeshColor.Colors[Index * 4 + 0] = 0xFF - Vertices[Index].Red;
				Tile->MeshColor.Colors[Index * 4 + 1] = 0xFF - Vertices[Index].Green;
				Tile->MeshColor.Colors[Index * 4 + 2] = 0xFF - Vertices[Index].Blue;
				Tile->MeshColor.Colors[Index * 4 + 3] = 0xFF - Vertices[Index].Alpha;
				Tile->MeshColor.UVs[Index * 2 + 0] = Vertices[Index].UV.x;
				Tile->MeshColor.UVs[Index * 2 + 1] = Vertices[Index].UV.y;
			}

			Tile->MeshColor.MeshCount = UsedTextureIDCount;
			Tile->MeshColor.Meshes = (Mesh*)AllocatorAllocate(Archive->Allocator, sizeof(Mesh) * UsedTextureIDCount);
			if (!Tile->MeshColor.Meshes) Fatal("Memory allocation failed!");
			Tile->MeshColor.Textures = (Texture*)AllocatorAllocate(Archive->Allocator, sizeof(Texture) * UsedTextureIDCount);
			if (!Tile->MeshColor.Textures) Fatal("Memory allocation failed!");

			Int32 IndexOffset = 0;
			for (Int Index = 0; Index < UsedTextureIDCount; Index += 1) {
				Int32 TextureIndex = UsedFaceTextureIDs[Index];
				MCLTextureRef Texture = ArrayGetElementAtIndex(Archive->Textures, TextureIndex);
				Tile->MeshColor.Textures[Index] = Texture->Texture;

				Mesh* Mesh = &Tile->MeshColor.Meshes[Index];
				memset(Mesh, 0, sizeof(struct Mesh));

				Mesh->vertexCount = Tile->MeshColor.VertexCount;
				Mesh->vertices = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Vector3) * VertexCount);
				if (!Mesh->vertices) Fatal("Memory allocation failed!");
				memcpy(Mesh->vertices, Tile->MeshColor.Vertices, sizeof(Vector3) * VertexCount);

				Mesh->colors = (UInt8*)AllocatorAllocate(Archive->Allocator, sizeof(UInt32) * VertexCount);
				if (!Mesh->colors) Fatal("Memory allocation failed!");
				memcpy(Mesh->colors, Tile->MeshColor.Colors, sizeof(UInt32) * VertexCount);

				Mesh->texcoords = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Vector2) * VertexCount);
				if (!Mesh->texcoords) Fatal("Memory allocation failed!");
				memcpy(Mesh->texcoords, Tile->MeshColor.UVs, sizeof(Vector2) * VertexCount);

				Int32 TriangleCount = 0;
				UInt16* Indices = &Tile->MeshColor.Indices[IndexOffset];
				for (Int FaceIndex = 0; FaceIndex < FaceCount; FaceIndex += 1) {
					Int32 FaceTextureIndex = FaceTextureIDs[FaceIndex];
					if (FaceTextureIndex != TextureIndex) continue;

					Tile->MeshColor.Indices[IndexOffset++] = Faces[FaceIndex].Indices[0];
					Tile->MeshColor.Indices[IndexOffset++] = Faces[FaceIndex].Indices[2];
					Tile->MeshColor.Indices[IndexOffset++] = Faces[FaceIndex].Indices[1];
					TriangleCount += 1;
				}

				Mesh->triangleCount = TriangleCount;
				Mesh->indices = (UInt16*)AllocatorAllocate(Archive->Allocator, sizeof(MCLFace) * TriangleCount);
				if (!Mesh->indices) Fatal("Memory allocation failed!");
				memcpy(Mesh->indices, Indices, sizeof(MCLFace) * TriangleCount);

				UploadMesh(Mesh, false);
			}
		}

		Int32 HasLayer = *(Int32*)&Source[SourceOffset];
		SourceOffset += sizeof(Int32);

		Tile->HasLayer = HasLayer != 0;
		if (Tile->HasLayer) {
			UInt32 FaceCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);

			UInt32 VertexCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);

			MCLVertex* Vertices = (MCLVertex*)&Source[SourceOffset];
			SourceOffset += sizeof(MCLVertex) * VertexCount;

			MCLFace* Faces = (MCLFace*)&Source[SourceOffset];
			SourceOffset += sizeof(MCLFace) * FaceCount;

			Int32* FaceTextureIDs = (Int32*)&Source[SourceOffset];
			SourceOffset += sizeof(Int32) * FaceCount;

			UInt32 UsedTextureIDCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);
			Int32* UsedFaceTextureIDs = (Int32*)&Source[SourceOffset];
			SourceOffset += sizeof(Int32) * UsedTextureIDCount;

			Tile->Mesh.VertexCount = (Int32)VertexCount;
			Tile->Mesh.Vertices = (Float32*)RL_MALLOC(sizeof(Vector3) * VertexCount);
			if (!Tile->Mesh.Vertices) Fatal("Memory allocation failed!");

			Tile->Mesh.Colors = (UInt8*)RL_MALLOC(sizeof(UInt32) * VertexCount);
			if (!Tile->Mesh.Colors) Fatal("Memory allocation failed!");

			Tile->Mesh.UVs = (Float32*)RL_MALLOC(sizeof(Vector2) * VertexCount);
			if (!Tile->Mesh.UVs) Fatal("Memory allocation failed!");

			Tile->Mesh.TriangleCount = FaceCount;
			Tile->Mesh.Indices = (UInt16*)RL_MALLOC(sizeof(MCLFace) * FaceCount);
			if (!Tile->Mesh.Indices) Fatal("Memory allocation failed!");

			for (Int Index = 0; Index < VertexCount; Index += 1) {
				Tile->Mesh.Vertices[Index * 3 + 0] = Vertices[Index].Position.x;
				Tile->Mesh.Vertices[Index * 3 + 1] = Vertices[Index].Position.y;
				Tile->Mesh.Vertices[Index * 3 + 2] = 25600.0f - Vertices[Index].Position.z;
				Tile->Mesh.Colors[Index * 4 + 0] = 0xFF;
				Tile->Mesh.Colors[Index * 4 + 1] = 0xFF;
				Tile->Mesh.Colors[Index * 4 + 2] = 0xFF;
				Tile->Mesh.Colors[Index * 4 + 3] = 0xFF;
				Tile->Mesh.UVs[Index * 2 + 0] = Vertices[Index].UV.x;
				Tile->Mesh.UVs[Index * 2 + 1] = Vertices[Index].UV.y;
			}

			Tile->Mesh.MeshCount = UsedTextureIDCount;
			Tile->Mesh.Meshes = (Mesh*)AllocatorAllocate(Archive->Allocator, sizeof(Mesh) * UsedTextureIDCount);
			if (!Tile->Mesh.Meshes) Fatal("Memory allocation failed!");
			Tile->Mesh.Textures = (Texture*)AllocatorAllocate(Archive->Allocator, sizeof(Texture) * UsedTextureIDCount);
			if (!Tile->Mesh.Textures) Fatal("Memory allocation failed!");

			Int32 IndexOffset = 0;
			for (Int Index = 0; Index < UsedTextureIDCount; Index += 1) {
				Int32 TextureIndex = UsedFaceTextureIDs[Index];
				MCLTextureRef Texture = ArrayGetElementAtIndex(Archive->Textures, TextureIndex);
				Tile->Mesh.Textures[Index] = Texture->Texture;

				Mesh* Mesh = &Tile->Mesh.Meshes[Index];
				memset(Mesh, 0, sizeof(struct Mesh));

				Mesh->vertexCount = Tile->Mesh.VertexCount;
				Mesh->vertices = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Vector3) * VertexCount);
				if (!Mesh->vertices) Fatal("Memory allocation failed!");
				memcpy(Mesh->vertices, Tile->Mesh.Vertices, sizeof(Vector3) * VertexCount);

				Mesh->colors = (UInt8*)AllocatorAllocate(Archive->Allocator, sizeof(UInt32) * VertexCount);
				if (!Mesh->colors) Fatal("Memory allocation failed!");
				memcpy(Mesh->colors, Tile->Mesh.Colors, sizeof(UInt32) * VertexCount);

				Mesh->texcoords = (Float32*)AllocatorAllocate(Archive->Allocator, sizeof(Vector2) * VertexCount);
				if (!Mesh->texcoords) Fatal("Memory allocation failed!");
				memcpy(Mesh->texcoords, Tile->Mesh.UVs, sizeof(Vector2) * VertexCount);

				Int32 TriangleCount = 0;
				UInt16* Indices = &Tile->Mesh.Indices[IndexOffset];
				for (Int FaceIndex = 0; FaceIndex < FaceCount; FaceIndex += 1) {
					Int32 FaceTextureIndex = FaceTextureIDs[FaceIndex];
					if (FaceTextureIndex != TextureIndex) continue;

					Tile->Mesh.Indices[IndexOffset++] = Faces[FaceIndex].Indices[0];
					Tile->Mesh.Indices[IndexOffset++] = Faces[FaceIndex].Indices[2];
					Tile->Mesh.Indices[IndexOffset++] = Faces[FaceIndex].Indices[1];
					TriangleCount += 1;
				}

				Mesh->triangleCount = TriangleCount;
				Mesh->indices = (UInt16*)AllocatorAllocate(Archive->Allocator, sizeof(MCLFace) * TriangleCount);
				if (!Mesh->indices) Fatal("Memory allocation failed!");
				memcpy(Mesh->indices, Indices, sizeof(MCLFace) * TriangleCount);

				UploadMesh(Mesh, false);
			}
		}
	}

	UInt32 ModelCount = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt32);

	for (Int Index = 0; Index < ModelCount; Index += 1) {
		MCLModelRef Model = (MCLModelRef)ArrayAppendUninitializedElement(Archive->Models);
		Model->IsWater = false;

		UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt16);

		Model->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
		if (!Model->Name) Fatal("Memory allocation failed!");
		memcpy(Model->Name, &Source[SourceOffset], NameLength);
		Model->Name[NameLength] = '\0';
		SourceOffset += NameLength;

		Model->BoundsMin = *(Vector3*)&Source[SourceOffset];
		SourceOffset += sizeof(Model->BoundsMin);

		Model->BoundsMax = *(Vector3*)&Source[SourceOffset];
		SourceOffset += sizeof(Model->BoundsMax);

		Float32 Temp = Model->BoundsMax.z;
		Model->BoundsMax.z = -Model->BoundsMin.z;
		Model->BoundsMin.z = -Temp;

		Model->Archive = EBMArchiveCreate(Archive->Allocator);

		if (CStringHasFileExtension(Model->Name, ".ebm") || CStringHasFileExtension(Model->Name, ".EBM")) {
			CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Object", Model->Name, NULL);
			if (!EBMArchiveLoadFromFile(Context->ShaderEBM, Model->Archive, FilePath)) {
				Warn("Loading model %s failed!", FilePath);
			}
		}
		else if (CStringHasFileExtension(Model->Name, ".ewt") || CStringHasFileExtension(Model->Name, ".EWT")) {
			Model->IsWater = true;

			EBMMeshRef Mesh = (EBMMeshRef)ArrayAppendUninitializedElement(Model->Archive->Meshes);
			memset(Mesh, 0, sizeof(struct _EBMMesh));

			EBMMaterialRef Material = (EBMMaterialRef)ArrayAppendUninitializedElement(Model->Archive->Materials);
			memset(Material, 0, sizeof(struct _EBMMaterial));

			Material->TextureMain.Texture.Image = GenImageColor(32, 32, (Color) { 0x5A, 0xBC, 0xD8, 0x90 });
			ImageFormat(&Material->TextureMain.Texture.Image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
			Material->TextureMain.Texture.Texture = LoadTextureFromImage(Material->TextureMain.Texture.Image);

			CString FilePath = PathCombineAll(Context->Config.Editor.ClientDataPath, "Object", Model->Name, NULL);
			UInt8 Source[32];
			Int32 SourceLength = 0;
			FileRef File = FileOpen(FilePath);
			if (!File) goto error;
			if (!FileReadNoAlloc(File, Source, (Int32)sizeof(Source), &SourceLength)) goto error;

			if (SourceLength > 0) {
				Archive->WaterLevel = *(UInt8*)&Source[0];

				Float32 Vertices[] = {
				   Model->BoundsMin.x, Model->BoundsMin.y, Model->BoundsMax.z,
				   Model->BoundsMax.x, Model->BoundsMin.y, Model->BoundsMax.z,
				   Model->BoundsMax.x, Model->BoundsMax.y, Model->BoundsMax.z,
				   Model->BoundsMin.x, Model->BoundsMax.y, Model->BoundsMax.z,
				   Model->BoundsMin.x, Model->BoundsMin.y, Model->BoundsMin.z,
				   Model->BoundsMin.x, Model->BoundsMax.y, Model->BoundsMin.z,
				   Model->BoundsMax.x, Model->BoundsMax.y, Model->BoundsMin.z,
				   Model->BoundsMax.x, Model->BoundsMin.y, Model->BoundsMin.z,
				   Model->BoundsMin.x, Model->BoundsMax.y, Model->BoundsMin.z,
				   Model->BoundsMin.x, Model->BoundsMax.y, Model->BoundsMax.z,
				   Model->BoundsMax.x, Model->BoundsMax.y, Model->BoundsMax.z,
				   Model->BoundsMax.x, Model->BoundsMax.y, Model->BoundsMin.z,
				   Model->BoundsMin.x, Model->BoundsMin.y, Model->BoundsMin.z,
				   Model->BoundsMax.x, Model->BoundsMin.y, Model->BoundsMin.z,
				   Model->BoundsMax.x, Model->BoundsMin.y, Model->BoundsMax.z,
				   Model->BoundsMin.x, Model->BoundsMin.y, Model->BoundsMax.z,
				   Model->BoundsMax.x, Model->BoundsMin.y, Model->BoundsMin.z,
				   Model->BoundsMax.x, Model->BoundsMax.y, Model->BoundsMin.z,
				   Model->BoundsMax.x, Model->BoundsMax.y, Model->BoundsMax.z,
				   Model->BoundsMax.x, Model->BoundsMin.y, Model->BoundsMax.z,
				   Model->BoundsMin.x, Model->BoundsMin.y, Model->BoundsMin.z,
				   Model->BoundsMin.x, Model->BoundsMin.y, Model->BoundsMax.z,
				   Model->BoundsMin.x, Model->BoundsMax.y, Model->BoundsMax.z,
				   Model->BoundsMin.x, Model->BoundsMax.y, Model->BoundsMin.z,
				};

				Mesh->Mesh.vertices = (float*)RL_MALLOC(sizeof(Vertices));
				memcpy(Mesh->Mesh.vertices, Vertices, sizeof(Vertices));

				Mesh->Mesh.indices = (unsigned short*)RL_MALLOC(36 * sizeof(unsigned short));

				Int QuadCount = 0;
				for (Int Index = 0; Index < 36; Index += 6) {
					Mesh->Mesh.indices[Index] = 4 * QuadCount;
					Mesh->Mesh.indices[Index + 1] = 4 * QuadCount + 1;
					Mesh->Mesh.indices[Index + 2] = 4 * QuadCount + 2;
					Mesh->Mesh.indices[Index + 3] = 4 * QuadCount;
					Mesh->Mesh.indices[Index + 4] = 4 * QuadCount + 2;
					Mesh->Mesh.indices[Index + 5] = 4 * QuadCount + 3;
					QuadCount++;
				}

				Mesh->Mesh.vertexCount = 24;
				Mesh->Mesh.triangleCount = 12;

				UploadMesh(&Mesh->Mesh, false);
			}

			FileClose(File);
		}
	}

	UInt32 InteractiveObjectCount = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt32);

	for (Int Index = 0; Index < InteractiveObjectCount; Index += 1) {
		MCLInteractiveObjectRef Object = (MCLInteractiveObjectRef)ArrayAppendUninitializedElement(Archive->InteractiveObjects);

		UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt16);

		Object->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
		if (!Object->Name) Fatal("Memory allocation failed!");
		memcpy(Object->Name, &Source[SourceOffset], NameLength);
		Object->Name[NameLength] = '\0';
		SourceOffset += NameLength;

		Object->Object.Position = *(Vector3*)&Source[SourceOffset];
		Object->Object.Position.z = 25600.0f - Object->Object.Position.z;
		SourceOffset += sizeof(Object->Object.Position);

		Object->Object.Rotation = *(Quaternion*)&Source[SourceOffset];
		Object->Object.Rotation.z = -Object->Object.Rotation.z;
		Object->Object.Rotation.w = -Object->Object.Rotation.w;
		SourceOffset += sizeof(Object->Object.Rotation);

		Object->Object.Scale = *(Vector3*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->Object.Scale);

		Object->Object.ModelIndex = *(Int32*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->Object.ModelIndex);

		Object->Type = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->Type);

		Object->Flags = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->Flags);

		Object->NpcIndex = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->NpcIndex);

		MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Object->Object.ModelIndex);
		Float32 CenterZ = (Model->BoundsMin.z + Model->BoundsMax.z) / 2;
		//Object->Object.Position.z = CenterZ - (Object->Object.Position.z - CenterZ);
	}

	UInt32 StaticObjectCount = *(UInt32*)&Source[SourceOffset];
	SourceOffset += sizeof(UInt32);

	for (Int Index = 0; Index < StaticObjectCount; Index += 1) {
		MCLObjectRef Object = (MCLObjectRef)ArrayAppendUninitializedElement(Archive->StaticObjects);

		Object->Position = *(Vector3*)&Source[SourceOffset];
		Object->Position.z = 25600.0f - Object->Position.z;
		SourceOffset += sizeof(Object->Position);

		Object->Rotation = *(Quaternion*)&Source[SourceOffset];
		Object->Rotation.z = -Object->Rotation.z;
		Object->Rotation.w = -Object->Rotation.w;
		SourceOffset += sizeof(Object->Rotation);

		Object->Scale = *(Vector3*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->Scale);

		Object->ModelIndex = *(Int32*)&Source[SourceOffset];
		SourceOffset += sizeof(Object->ModelIndex);

		MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Object->ModelIndex);
		Float32 CenterZ = (Model->BoundsMin.z + Model->BoundsMax.z) / 2;
		//Object->Position.z = CenterZ - (Object->Position.z - CenterZ);
		//Object->Position.z = 25600.0f + 3800.0f + (CenterZ - (Object->Position.z - CenterZ));
	}

	if (Archive->Version < 1007) {
		UInt32 UnknownMemoryCount = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);
		SourceOffset += (sizeof(UInt32) * 3 + sizeof(UInt16) * 3) * UnknownMemoryCount;

		UInt32 EnvironmentObjectCount = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);

		for (Int Index = 0; Index < EnvironmentObjectCount; Index += 1) {
			MCLObjectRef Object = (MCLObjectRef)ArrayAppendUninitializedElement(Archive->EnvironmentObjects);

			Object->Position = *(Vector3*)&Source[SourceOffset];
			Object->Position.z = 25600.0f - Object->Position.z;
			SourceOffset += sizeof(Object->Position);

			Object->Rotation = *(Quaternion*)&Source[SourceOffset];
			Object->Rotation.z = -Object->Rotation.z;
			Object->Rotation.w = -Object->Rotation.w;
			SourceOffset += sizeof(Object->Rotation);

			Object->Scale = *(Vector3*)&Source[SourceOffset];
			SourceOffset += sizeof(Object->Scale);

			Object->ModelIndex = *(Int32*)&Source[SourceOffset];
			SourceOffset += sizeof(Object->ModelIndex);

			MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Object->ModelIndex);
			Float32 CenterZ = (Model->BoundsMin.z + Model->BoundsMax.z) / 2;
			//Object->Position.z = 25600.0f + 3800.0f + (CenterZ - (Object->Position.z - CenterZ));
		}

		UInt32 UnknownIndexCount = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);
		SourceOffset += sizeof(UInt32) * UnknownIndexCount;

		SourceOffset += sizeof(UInt8) * 256 * 256;
		SourceOffset += sizeof(UInt8) * 256 * 256;

		UInt32 SoundGroupCount = *(UInt32*)&Source[SourceOffset];
		SourceOffset += sizeof(UInt32);
		
		for (Int Index = 0; Index < SoundGroupCount; Index += 1) {
			MCLSoundGroupRef SoundGroup = (MCLSoundGroupRef)ArrayAppendUninitializedElement(Archive->SoundGroups);

			SoundGroup->SoundCount = *(UInt32*)&Source[SourceOffset];
			SourceOffset += sizeof(UInt32);

			SoundGroup->Sounds = (MCLSound*)AllocatorAllocate(Archive->Allocator, sizeof(MCLSound) * SoundGroup->SoundCount);
			if (!SoundGroup->Sounds) Fatal("Memory allocation failed!");

			for (Int Index = 0; Index < SoundGroup->SoundCount; Index += 1) {
				MCLSoundRef Sound = &SoundGroup->Sounds[Index];

				UInt16 NameLength = *(UInt16*)&Source[SourceOffset];
				SourceOffset += sizeof(UInt16);

				Sound->Name = (CString)AllocatorAllocate(Archive->Allocator, NameLength + 1);
				if (!Sound->Name) Fatal("Memory allocation failed!");
				memcpy(Sound->Name, &Source[SourceOffset], NameLength);
				Sound->Name[NameLength] = '\0';
				SourceOffset += NameLength;

				Sound->Volume = *(Float32*)&Source[SourceOffset];
				SourceOffset += sizeof(Float32);
			}
		}

		SourceOffset += sizeof(UInt32) * 4;
	}

	FileClose(File);
	free(Source);
	return true;

error:
	if (File) FileClose(File);
	if (Source) free(Source);

	return false;
}

Void DrawTerrain(
	EditorContextRef Context,
	MCLArchiveRef Archive
) {
	Material Material = LoadMaterialDefault();
	Matrix Transform = MatrixIdentity();
	Transform = MatrixMultiply(Transform, MatrixScale(1.0f / 100, 1.0f / 100, 1.0f / 100));
	Transform = MatrixMultiply(Transform, MatrixTranslate(0.0f, 1.8f, 0.0f));

	Bool IsTracingObject = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
	Ray Ray = GetMouseRay(GetMousePosition(), Context->Camera);

	//DrawModel(Archive->HeightMapModel, (Vector3) { 0, 0.0f, 0 }, 1.0f / 100, (Color) { 0xFF, 0x00, 0x00, 0xFF });

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->Tiles); Index += 1) {
		MCLTileRef Tile = (MCLTileRef)ArrayGetElementAtIndex(Archive->Tiles, Index);
		if (Tile->HasLayer) {
			for (Int Index = 0; Index < Tile->Mesh.MeshCount; Index += 1) {
				SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, Tile->Mesh.Textures[Index]);
				DrawMesh(Tile->Mesh.Meshes[Index], Material, Transform);
			}
		}

		if (Tile->HasLayerColor) {
			for (Int Index = 0; Index < Tile->MeshColor.MeshCount; Index += 1) {
				SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, Tile->MeshColor.Textures[Index]);
				DrawMesh(Tile->MeshColor.Meshes[Index], Material, Transform);
			}
		}
	}

	Transform = MatrixMultiply(Transform, MatrixTranslate(0.0f, -1.8f, 0.0f));
	Transform = MatrixMultiply(Transform, MatrixTranslate(0.0f, Archive->WaterLevel / 64, 0.0f));
	//Transform = MatrixMultiply(Transform, MatrixTranslate(0.0f, -0.5f, 0.0f));

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->InteractiveObjects); Index += 1) {
		MCLInteractiveObjectRef Object = (MCLInteractiveObjectRef)ArrayGetElementAtIndex(Archive->InteractiveObjects, Index);
		MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Object->Object.ModelIndex);

		Float32 OffsetY = 0;
		UInt16 TilePositionX = (UInt16)MAX(0, MIN(256, Object->Object.Position.x / 256));
		UInt16 TilePositionY = (UInt16)MAX(0, MIN(256, Object->Object.Position.z / 256));
		Float32 TileHeight = Archive->HeightMap[(256 - TilePositionY) * 257 + TilePositionX];
		
		Matrix ModelTransform = MatrixIdentity();
		ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(Object->Object.Scale.x, Object->Object.Scale.y, Object->Object.Scale.z));
		ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(100.0f / Model->Archive->ScalePercentage, 100.0f / Model->Archive->ScalePercentage, 100.0f / Model->Archive->ScalePercentage));
		ModelTransform = MatrixMultiply(ModelTransform, QuaternionToMatrix(Object->Object.Rotation));
		ModelTransform = MatrixMultiply(ModelTransform, MatrixTranslate(Object->Object.Position.x, OffsetY + Object->Object.Position.y, Object->Object.Position.z));
		ModelTransform = MatrixMultiply(ModelTransform, Transform);

		for (Int MeshIndex = 0; MeshIndex < ArrayGetElementCount(Model->Archive->Meshes); MeshIndex += 1) {
			EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Model->Archive->Meshes, MeshIndex);
			EBMMaterialRef MeshMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Model->Archive->Materials, Mesh->MaterialIndex);

			SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, MeshMaterial->TextureMain.Texture.Texture);
			DrawMesh(Mesh->Mesh, Material, ModelTransform);

			BoundingBox Bounds;
			Bounds.min = Vector3Transform(Model->BoundsMin, ModelTransform);
			Bounds.max = Vector3Transform(Model->BoundsMax, ModelTransform);
			DrawBoundingBox(Bounds, RED);
		}
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->StaticObjects); Index += 1) {
		MCLObjectRef Object = (MCLObjectRef)ArrayGetElementAtIndex(Archive->StaticObjects, Index);
		MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Object->ModelIndex);

		Quaternion Rotation = Object->Rotation;
		Float32 OffsetY = 0;
		UInt16 TilePositionX = (UInt16)MAX(0, MIN(256, Object->Position.x / 256));
		UInt16 TilePositionY = (UInt16)MAX(0, MIN(256, Object->Position.z / 256));
		Float32 TileHeight = Archive->HeightMap[(256 - TilePositionY) * 257 + TilePositionX];
		Vector3 Position = Object->Position;

		Matrix ModelTransform = MatrixIdentity();
		ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(Object->Scale.x, Object->Scale.y, Object->Scale.z));
		ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(100.0f / Model->Archive->ScalePercentage, 100.0f / Model->Archive->ScalePercentage, 100.0f / Model->Archive->ScalePercentage));
		ModelTransform = MatrixMultiply(ModelTransform, QuaternionToMatrix(Rotation));
		ModelTransform = MatrixMultiply(ModelTransform, MatrixTranslate(Position.x, OffsetY + Position.y, Position.z));
		ModelTransform = MatrixMultiply(ModelTransform, Transform);

		for (Int MeshIndex = 0; MeshIndex < ArrayGetElementCount(Model->Archive->Meshes); MeshIndex += 1) {
			EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Model->Archive->Meshes, MeshIndex);
			EBMMaterialRef MeshMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Model->Archive->Materials, Mesh->MaterialIndex);

			SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, MeshMaterial->TextureMain.Texture.Texture);
			DrawMesh(Mesh->Mesh, Material, ModelTransform);

			BoundingBox Bounds;
			Bounds.min = Vector3Transform(Model->BoundsMin, ModelTransform);
			Bounds.max = Vector3Transform(Model->BoundsMax, ModelTransform);
			DrawBoundingBox(Bounds, RED);
		}
	}

	static Int Frames = 0;
	Frames++;
	static Int Flip = 0;
	static Bool Flipp0 = false;
	if (Frames > 60) {
		Flipp0 = !Flipp0;
		Flip += 1;
		Frames = 0;
		Trace("Flip: %d", Flip);
	}

	for (Int Index = 0; Index < ArrayGetElementCount(Archive->EnvironmentObjects); Index += 1) {
		MCLObjectRef Object = (MCLObjectRef)ArrayGetElementAtIndex(Archive->EnvironmentObjects, Index);
		MCLModelRef Model = (MCLModelRef)ArrayGetElementAtIndex(Archive->Models, Object->ModelIndex);

		Float32 OffsetY = 0;
		UInt16 TilePositionX = (UInt16)MAX(0, MIN(256, Object->Position.x / 256));
		UInt16 TilePositionY = (UInt16)MAX(0, MIN(256, Object->Position.z / 256));
		Float32 TileHeight = Archive->HeightMap[(256 - TilePositionY) * 257 + TilePositionX];
		if (TileHeight < Archive->WaterLevel && !Model->IsWater) {
			OffsetY = TileHeight + Archive->WaterLevel + (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5 * Object->Scale.y + Object->Position.y * -Object->Scale.y;
			Float32 MyHeight = (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5;
			OffsetY = TileHeight + Archive->WaterLevel + MyHeight * Object->Scale.y;
			OffsetY = (-Model->BoundsMin.y + (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5) * Object->Scale.y + Object->Position.y * -Object->Scale.y;
			OffsetY = Object->Position.y * -Object->Scale.y;
//			OffsetY = Object->Position.y * -Object->Scale.y - (Model->BoundsMax.y - Model->BoundsMin.y) * (Model->BoundsMax.y - Model->BoundsMin.y) / Model->BoundsMax.y * Object->Scale.y;
//			OffsetY = Object->Position.y * -Object->Scale.y;
			
			//OffsetY = Object->Position.y * -Object->Scale.y + Archive->WaterLevel + TileHeight;

			/*
			if (Model->Archive->Options.Unknown2 & 0b100) { // Optional Culling or Alpha (z-test player)
				// OffsetY += (Model->BoundsMax.y + Model->BoundsMin.y) * 0.5 * Object->Scale.y;
			}
			else 
			*/

			OffsetY += TileHeight;
			OffsetY += Archive->WaterLevel;
			//OffsetY -= (Model->BoundsMax.y - Model->BoundsMin.y) * 0.06666 * Object->Scale.y;

			if (Model->Archive->Options.Unknown1 & 0b10000) {
				OffsetY -= (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5 * Object->Scale.y;
				//OffsetY += Model->BoundsMin.y * Object->Scale.y * 4;
			} 
			else {
				OffsetY -= (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5;
				//OffsetY += (Model->BoundsMin.y + (0.0f - Model->BoundsMax.y)) * 0.5 * Object->Scale.y;
				//OffsetY -= (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5 * Object->Scale.y;
				//OffsetY = Object->Position.y * -Object->Scale.y + Archive->WaterLevel + TileHeight;
				//OffsetY = (Model->BoundsMax.y - Model->BoundsMin.y) * Object->Scale.y + TileHeight + Archive->WaterLevel;
			}
		}

		Quaternion Rotation = Object->Rotation;
		if (Flipp0) {
		}
		//  ((Model->BoundsMax.y - Model->BoundsMin.y) * 0.5 + Model->BoundsMin.y)
		//OffsetY += (Model->BoundsMin.y + (0.0f - Model->BoundsMax.y)) * 0.5 * Object->Scale.y;
		OffsetY += (Model->BoundsMax.y - Model->BoundsMin.y) * 0.5 * Object->Scale.y;

		Matrix ModelTransform = MatrixIdentity();
		ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(Object->Scale.x, Object->Scale.y, Object->Scale.z));
		ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(100.0f / Model->Archive->ScalePercentage, 100.0f / Model->Archive->ScalePercentage, 100.0f / Model->Archive->ScalePercentage));
		ModelTransform = MatrixMultiply(ModelTransform, QuaternionToMatrix(Rotation));
		ModelTransform = MatrixMultiply(ModelTransform, MatrixTranslate(Object->Position.x, OffsetY + Object->Position.y, Object->Position.z));
		ModelTransform = MatrixMultiply(ModelTransform, Transform);

		for (Int MeshIndex = 0; MeshIndex < ArrayGetElementCount(Model->Archive->Meshes); MeshIndex += 1) {
			EBMMeshRef Mesh = (EBMMeshRef)ArrayGetElementAtIndex(Model->Archive->Meshes, MeshIndex);
			EBMMaterialRef MeshMaterial = (EBMMaterialRef)ArrayGetElementAtIndex(Model->Archive->Materials, Mesh->MaterialIndex);
			SetMaterialTexture(&Material, MATERIAL_MAP_DIFFUSE, MeshMaterial->TextureMain.Texture.Texture);
			DrawMesh(Mesh->Mesh, Material, ModelTransform);

			BoundingBox Bounds;
			Bounds.min = Vector3Transform(Model->BoundsMin, ModelTransform);
			Bounds.max = Vector3Transform(Model->BoundsMax, ModelTransform);
			DrawBoundingBox(Bounds, RED);

			if (IsTracingObject) {
				RayCollision Collision = GetRayCollisionBox(Ray, Bounds);
				if (Collision.hit) {
					Trace(Model->Name);
				}
			}
		}
	}
}
