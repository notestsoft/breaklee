#pragma once

#include "Base.h"
#include "Primitives.h"

EXTERN_C_BEGIN

__pragma(pack(push, 1))

enum {
    EBM_CHUNK_TYPE_MATERIAL     = 0x41470201,
    EBM_CHUNK_TYPE_MESH         = 0x41470202,
    EBM_CHUNK_TYPE_ARMATURE     = 0x41470203,
    EBM_CHUNK_TYPE_ANIMATION    = 0x41470204,
    EBM_CHUNK_TYPE_INFLUENCE    = 0x41470205,
};

enum {
    EBM_RENDER_FLAG_MULTIPLY = 0x04,
    EBM_RENDER_FLAG_GRAIN_MERGE = 0x08,
    EBM_RENDER_FLAG_HARD_LIGHT = 0x09,
    EBM_RENDER_FLAG_MULTIPLY_INVERT_PARENT = 0x0A,
    EBM_RENDER_FLAG_ODD_FLICKER_EFFECT = 0x0E,
    EBM_RENDER_FLAG_BELOW = 0x10,
    EBM_RENDER_FLAG_SCREEN = 0x12,
    EBM_RENDER_FLAG_SOME_PURE_WHITE_THING = 0x13,
    EBM_RENDER_FLAG_CHROME = 0x18,
};

typedef struct _EBMText {
    UInt16 Length;
    Char* Buffer;
} EBMText;

typedef struct _EBMHeader {
    Int32 Version;
    UInt16 Unknown1;
    
    struct {
        UInt8 Unknown1 : 1;
        UInt8 Unknown2 : 1;
        UInt8 AdditiveBlend : 1;
        UInt8 Unknown3 : 1;
        UInt8 Unknown4 : 1;
        UInt8 Unknown5 : 1;
        UInt8 Unknown6 : 1;
        UInt8 EnableAlpha : 1;
    } Flags;

    UInt8 AlphaThreshold;
    UInt16 Unknown2;
    Vector3 BoundMin;
    Vector3 BoundMax;
    UInt32 Unknown3;
} EBMHeader;

typedef struct _EBMChunkHeader {
    UInt32 ChunkType;
    UInt16 ChunkSize;
} EBMChunkHeader;

enum {
    EBM_MATERIAL_PROPERTY_COLOR0,
    EBM_MATERIAL_PROPERTY_AMBIENT,
    EBM_MATERIAL_PROPERTY_COLOR1,
    EBM_MATERIAL_PROPERTY_DIFFUSE,
    EBM_MATERIAL_PROPERTY_COLOR2,
    EBM_MATERIAL_PROPERTY_SPECULAR,
    EBM_MATERIAL_PROPERTY_EMISSIVE,
};

typedef struct _EBMMaterial {
    Vector4 Ambient;
    Vector4 Diffuse;
    Vector4 Specular;
    Vector4 Emissive;
    Float32 Emission;

    struct {
        EBMText ID;
        UInt32 Size;
        Char Signature[4];
        DDSHeader Header;
        UInt8* Data;
        Bool IsFaceted;
        Vector2 UVSpeed;
    } Texture;

    struct {
        Int32 MaterialIndex;
        Bool IsFaceted;
        Vector2 UVSpeed;
        UInt32 RenderFlags;
    } Layer;

} EBMMaterial;

typedef struct _EBMMaterialChunk {
    EBMChunkHeader Header;
    EBMMaterial* Data;
} EBMMaterialChunk;

typedef struct _EBMBone {
    EBMText ID;
    Int32 ParentBoneIndex;
    Matrix4 BoneSpaceMatrix;
    Matrix4 ParentBoneSpaceMatrix;
} EBMBone;

typedef struct _EBMSkeletonChunk {
    EBMChunkHeader Header;
    EBMBone* Data;
} EBMSkeletonChunk;

typedef struct _EBMInfluenceBone {
    UInt32 Count;
    UInt32* Indices;
    Float32* Weights;
} EBMInfluenceBone;

typedef struct _EBMInfluenceMeshChunk {
    EBMChunkHeader Header;
    EBMInfluenceBone* Data;
} EBMInfluenceMeshChunk;

typedef struct _EBMVertex {
    Vector3 Position;
    Vector3 Normal;
    Vector3 UV;
} EBMVertex;

typedef struct _EBMFace {
    UInt16 Indices[3];
} EBMFace;

typedef struct _EBMMesh {
    EBMText ID;
    Matrix4 WorldMatrix;
    Matrix4 LocalMatrix;
    Int32 RootBoneID;
    UInt8 MaterialIndex;
    UInt16 VertexCount;
    UInt16 FaceCount;
    EBMText EFXFile;
    EBMVertex* Vertices;
    EBMFace* Faces;
    EBMInfluenceMeshChunk* InfluenceChunk;
} EBMMesh;

typedef struct _EBMMeshChunk {
    EBMChunkHeader Header;
    EBMMesh* Data;
} EBMMeshChunk;

typedef struct _EBMTranslationKeyframe {
    Float32 Timestamp;
    Vector3 Translation;
} EBMTranslationKeyframe;

typedef struct _EBMRotationKeyframe {
    Float32 Timestamp;
    Quaternion Rotation;
} EBMRotationKeyframe;

typedef struct _EBMTransformation {
    EBMText ID;
    UInt32 TranslationCount;
    EBMTranslationKeyframe* TranslationKeyframes;
    UInt32 RotationCount;
    EBMRotationKeyframe* RotationKeyframes;
} EBMTransformation;

typedef struct _EBMAnimation {
    EBMText ID;
    UInt16 TransformationCount;
    EBMTransformation* Transformations;
} EBMAnimation;

typedef struct _EBMAnimationChunk {
    EBMChunkHeader Header;
    EBMAnimation* Data;
} EBMAnimationChunk;

typedef struct _EBMGlowPalette {
    struct {
        UInt8 Blue;
        UInt8 Green;
        UInt8 Red;
        UInt8 Alpha;
    } Colors[210];
} EBMGlowPalette;

__pragma(pack(pop))

EXTERN_C_END
