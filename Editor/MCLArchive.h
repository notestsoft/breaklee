#pragma once

#include "Base.h"
#include "EBMArchive.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

struct _MCLEffect {
	CString Name;
	Float32 Unknown1;
	Float32 Unknown2;
	UInt32 Unknown3;
	UInt32 Unknown4;
};
typedef struct _MCLEffect* MCLEffectRef;

struct _MCLTexture {
	UInt32 Length;
	UInt8* Data;
	Image Image;
	Texture2D Texture;
};
typedef struct _MCLTexture* MCLTextureRef;

struct _MCLVertexColor {
	Vector3 Position;
	UInt8 Red;
	UInt8 Green;
	UInt8 Blue;
	UInt8 Alpha;
	Vector2 UV;
};
typedef struct _MCLVertexColor MCLVertexColor;

struct _MCLVertex {
	Vector3 Position;
	Vector2 UV;
};
typedef struct _MCLVertex MCLVertex;

struct _MCLFace {
	UInt16 Indices[3];
};
typedef struct _MCLFace MCLFace;

struct _MCLTileMesh {
	Int32 VertexCount;
	Float32* Vertices;
	UInt8* Colors;
	Float32* UVs;
	Int32 TriangleCount;
	UInt16* Indices;
	Int32 MeshCount;
	Mesh* Meshes;
	Texture2D* Textures;
};

struct _MCLTile {
	Vector3 BoundsMin;
	Vector3 BoundsMax;
	Bool HasLayerColor;
	Bool HasLayer;
	struct _MCLTileMesh MeshColor;
	struct _MCLTileMesh Mesh;
};
typedef struct _MCLTile* MCLTileRef;

struct _MCLModel {
	CString Name;
	Vector3 BoundsMin;
	Vector3 BoundsMax;
	EBMArchiveRef Archive;
	Bool IsWater;
};
typedef struct _MCLModel* MCLModelRef;

struct _MCLObject {
	Vector3 Position;
	Quaternion Rotation;
	Vector3 Scale;
	Int32 ModelIndex;
	Int32 TileX;
	Int32 TileY;
};
typedef struct _MCLObject MCLObject;
typedef struct _MCLObject* MCLObjectRef;

struct _MCLInteractiveObject {
	CString Name;
	MCLObject Object;
	UInt32 Type;
	UInt32 Flags;
	UInt32 NpcIndex;
};
typedef struct _MCLInteractiveObject* MCLInteractiveObjectRef;

struct _MCLSound {
	CString Name;
	Float32 Volume;
};
typedef struct _MCLSound MCLSound;
typedef struct _MCLSound* MCLSoundRef;

struct _MCLSoundGroup {
	Int32 SoundCount;
	MCLSound* Sounds;
};
typedef struct _MCLSoundGroup* MCLSoundGroupRef;

struct _MCLArchive {
	AllocatorRef Allocator;
	UInt32 Version;
	UInt32 Unknown1;
	UInt8 Environment[124];
	UInt32 Unknown3;
	UInt32 Unknown4;
	UInt32 Unknown5;
	UInt32 Unknown6;
	Float32 HeightMap[257 * 257];
	UInt32 ThreadMap[256 * 256];
	UInt8 SoundGroupMap[256 * 256];
	UInt8 WaterLevel;
	Model HeightMapModel;
	ArrayRef Effects;
	ArrayRef Textures;
	ArrayRef TileTextures;
	ArrayRef Tiles;
	ArrayRef Models;
	ArrayRef InteractiveObjects;
	ArrayRef StaticObjects;
	ArrayRef EnvironmentObjects;
	ArrayRef SoundGroups;
};
typedef struct _MCLArchive* MCLArchiveRef;

#pragma pack(pop)

MCLArchiveRef MCLArchiveCreate(
	AllocatorRef Allocator
);

Void MCLArchiveDestroy(
	MCLArchiveRef Archive
);

Bool MCLArchiveLoadFromFile(
	EditorContextRef Context,
	MCLArchiveRef Archive,
	CString FilePath
);

RayCollision MCLArchiveTraceRay(
	MCLArchiveRef Archive,
	Ray Ray
);

Void DrawTerrain(
	EditorContextRef Context,
	Rectangle Frame,
	MCLArchiveRef Archive
);

EXTERN_C_END