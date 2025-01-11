#pragma once

#include "Context.h"

EXTERN_C_BEGIN

Matrix ConvertDirectXMatrixToOpenGL(
    Matrix Source
);

Matrix ConvertTransformToRightHanded(
    Matrix Transform,
    Vector3 Position,
    Quaternion Rotation,
    Vector3 Scale,
    Float32 ScalePercentage,
    Float32 OffsetY
);

Void ConvertVerticesToRightHanded(
    Int32 VertexCount,
    Float32* Vertices
);

Void ConvertUVsToRightHanded(
    Int32 VertexCount,
    Float32* UVs
);

Void ConvertIndicesToRightHanded(
    Int32 TriangleCount,
    UInt16* Indices
);

Void ConvertMeshVerticesToRightHanded(
    Mesh* Mesh
);

Void ConvertMeshNormalsToRightHanded(
    Mesh* Mesh
);

Void ConvertMeshTexcoordsToRightHanded(
    Mesh* Mesh
);

Void ConvertMeshIndicesToRightHanded(
    Mesh* Mesh
);

Void ConvertMeshToRightHanded(
    Mesh* Mesh
);

EXTERN_C_END
