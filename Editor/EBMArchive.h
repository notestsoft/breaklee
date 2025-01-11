#pragma once

#include "Base.h"

EXTERN_C_BEGIN

#pragma pack(push, 1)

enum {
	EBM_CHUNK_TYPE_MATERIAL = 0x41470201,
	EBM_CHUNK_TYPE_MESH = 0x41470202,
	EBM_CHUNK_TYPE_SKELETON = 0x41470203,
	EBM_CHUNK_TYPE_ANIMATION = 0x41470204,
	EBM_CHUNK_TYPE_SKIN = 0x41470205,
};

struct _EBMMaterialProperties {
	Vector4 Ambient;
	Vector4 Diffuse;
	Vector4 Specular;
	Vector4 Emission;
	Float32 Strength;
};
typedef struct _EBMMaterialProperties EBMMaterialProperties;

struct _EBMTexture {
	CString Name;
	UInt32 Length;
	UInt8* Data;
	Image Image;
	Texture2D Texture;
};
typedef struct _EBMTexture EBMTexture;

struct _EBMMaterialTextureMain {
	EBMTexture Texture;
	Bool IsFaceted;
	Vector2 ScrollSpeed;
};
typedef struct _EBMMaterialTextureMain EBMMaterialTextureMain;

struct _EBMMaterialTextureBlend {
	Int32 MaterialIndex;
	Bool IsFaceted;
	Vector2 ScrollSpeed;
	UInt32 BlendFlags;
};
typedef struct _EBMMaterialTextureBlend EBMMaterialTextureBlend;

struct _EBMMaterial {
	EBMMaterialProperties Properties;
	EBMMaterialTextureMain TextureMain;
	EBMMaterialTextureBlend TextureBlend;
};
typedef struct _EBMMaterial* EBMMaterialRef;

struct _EBMVertex {
	Vector3 Position;
	Vector3 Normal;
	Vector2 UV;
};
typedef struct _EBMVertex EBMVertex;

struct _EBMMesh {
	CString Name;
	Matrix WorldMatrix;
	Matrix LocalMatrix;
	Int32 RootBoneIndex;
	UInt8 MaterialIndex;
	Bool IsEffectEnabled;
	CString EffectName;
	Mesh Mesh;
};
typedef struct _EBMMesh* EBMMeshRef;

struct _EBMBone {
	CString Name;
	Int32 ParentBoneIndex;
	Matrix SpaceMatrix;
	Matrix ParentSpaceMatrix;
};
typedef struct _EBMBone* EBMBoneRef;

struct _EBMSkinBone {
	Int32 Count;
	Int32* Indices;
	Float32* Weights;
};
typedef struct _EBMSkinBone* EBMSkinBoneRef;

struct _EBMAnimationTranslation {
	Float32 Timestamp;
	Vector3 Position;
};
typedef struct _EBMAnimationTranslation EBMAnimationTranslation;

struct _EBMAnimationRotation {
	Float32 Timestamp;
	Vector4 Rotation;
};
typedef struct _EBMAnimationRotation EBMAnimationRotation;

struct _EBMAnimationNode {
	CString Name;
	Int32 TranslationCount;
	EBMAnimationTranslation* Translations;
	Int32 RotationCount;
	EBMAnimationRotation* Rotations;
};
typedef struct _EBMAnimationNode EBMAnimationNode;

struct _EBMAnimation {
	CString Name;
	Int32 NodeCount;
	EBMAnimationNode* Nodes;
};
typedef struct _EBMAnimation* EBMAnimationRef;

struct _EBMOptions {
	UInt32 Unknown1 : 8;
	UInt32 AlphaThreshold : 8;
	UInt32 Unknown2 : 16;
};
typedef struct _EBMOptions EBMOptions;

struct _EBMArchive {
	AllocatorRef Allocator;
	UInt8 Magic;
	UInt16 Version;
	UInt8 Padding;
	UInt16 Unknown1;
	EBMOptions Options;
	Vector3 BoundsMin;
	Vector3 BoundsMax;
	Int32 ScalePercentage;
	BoundingBox Bounds;
	ArrayRef Materials;
	ArrayRef Bones;
	ArrayRef Meshes;
	ArrayRef SkinBones;
	ArrayRef Animations;
};
typedef struct _EBMArchive* EBMArchiveRef;

#pragma pack(pop)

EBMArchiveRef EBMArchiveCreate(
	AllocatorRef Allocator
);

Void EBMArchiveDestroy(
	EBMArchiveRef Archive
);

Bool EBMArchiveLoadFromFile(
	EBMArchiveRef Archive,
	CString FilePath
);

Void EBMArchiveSetupCamera(
	EditorContextRef Context,
	EBMArchiveRef Archive
);

Void EBMArchiveDraw(
	EditorContextRef Context,
	EBMArchiveRef Archive
);

EXTERN_C_END