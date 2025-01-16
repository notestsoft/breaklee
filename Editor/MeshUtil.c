#include "MeshUtil.h"


Matrix ConvertDirectXMatrixToOpenGL(
    Matrix Source
) {

    Matrix Result = { 0 };
    // Transpose the matrix (row-major to column-major)
    Result = MatrixTranspose(Source);
/*
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            ((Float32*)&Result)[col * 4 + row] = ((Float32*)&Source)[row * 4 + col];
        }
    }
    */
    
    // If necessary, adjust for coordinate system
    // Negate the z-axis for right-handed systems (projection matrix example)
   // ((Float32*)&Result)[2 * 4 + 2] *= -1; // Negate m22
  //  ((Float32*)&Result)[3 * 4 + 2] *= -1; // Negate m32
    return Result;
}

/*
Matrix ConvertDirectXMatrixToOpenGL(Matrix dxMatrix) {
    Matrix oglMatrix;

    // Transpose the matrix to switch from row-major to column-major
    oglMatrix.m0 = dxMatrix.m0;  oglMatrix.m1 = dxMatrix.m4;  oglMatrix.m2 = dxMatrix.m8;  oglMatrix.m3 = dxMatrix.m12;
    oglMatrix.m4 = dxMatrix.m1;  oglMatrix.m5 = dxMatrix.m5;  oglMatrix.m6 = dxMatrix.m9;  oglMatrix.m7 = dxMatrix.m13;
    oglMatrix.m8 = dxMatrix.m2;  oglMatrix.m9 = dxMatrix.m6;  oglMatrix.m10 = dxMatrix.m10; oglMatrix.m11 = dxMatrix.m14;
    oglMatrix.m12 = dxMatrix.m3; oglMatrix.m13 = dxMatrix.m7; oglMatrix.m14 = dxMatrix.m11; oglMatrix.m15 = dxMatrix.m15;

    // Flip the Y-axis by scaling Y components
    oglMatrix.m1 = -oglMatrix.m1;
    oglMatrix.m5 = -oglMatrix.m5;
    oglMatrix.m9 = -oglMatrix.m9;
    oglMatrix.m13 = -oglMatrix.m13;

    // If it's a projection or view matrix, invert the Z-axis for right-handed coordinate system
    oglMatrix.m2 = -oglMatrix.m2;
    oglMatrix.m6 = -oglMatrix.m6;
    oglMatrix.m10 = -oglMatrix.m10;
    oglMatrix.m14 = -oglMatrix.m14;
    
    return oglMatrix;
}
*/
Matrix ConvertTransformToRightHanded(
    Matrix Transform, 
    Vector3 Position, 
    Quaternion Rotation, 
    Vector3 Scale, 
    Float32 ScalePercentage, 
    Float32 OffsetY
) {
    // Flip Z-axis for position (Convert from DirectX to OpenGL)
    Position.z *= -1.0f;

    // Flip Z-axis for rotation (assuming quaternion rotation needs to be adjusted for the new coordinate system)
    Rotation.x *= -1.0f;  // Flip X
    Rotation.y *= -1.0f;  // Flip Y
    Rotation.z *= -1.0f;  // Flip Z
    // Note: Quaternion rotation for DirectX to OpenGL should be adjusted like this for full conversion

    // Apply transformations in the following order (the same as your original transformation pipeline)
    Matrix ModelTransform = MatrixIdentity();

    // Scale based on object scale and model scale percentage
    ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(Scale.x, Scale.y, Scale.z));
    ModelTransform = MatrixMultiply(ModelTransform, MatrixScale(100.0f / ScalePercentage, 100.0f / ScalePercentage, 100.0f / ScalePercentage));

    // Convert quaternion to matrix (this handles rotation)
    ModelTransform = MatrixMultiply(ModelTransform, QuaternionToMatrix(Rotation));

    // Translate (flip Z-axis for position)
    ModelTransform = MatrixMultiply(ModelTransform, MatrixTranslate(Position.x, OffsetY + Position.y, Position.z));

    // Apply any additional custom transformations
    ModelTransform = MatrixMultiply(ModelTransform, Transform);

    return ModelTransform;
}

Void ConvertVerticesToRightHanded(
    Int32 VertexCount,
    Float32* Vertices
) {
    for (Int Index = 0; Index < VertexCount; Index += 1) {
        Vertices[Index * 3 + 2] *= -1.0f;
    }
}

Void ConvertUVsToRightHanded(
    Int32 VertexCount,
    Float32* UVs
) {
    for (Int Index = 0; Index < VertexCount; Index += 1) {
        UVs[Index * 2 + 1] = 1.0f - UVs[Index * 2 + 1];
    }
}

Void ConvertIndicesToRightHanded(
    Int32 TriangleCount,
    UInt16* Indices
) {
    for (Int Index = 0; Index < TriangleCount; Index += 1) {
        UInt16 Temp = Indices[Index * 3 + 1];
        Indices[Index * 3 + 1] = Indices[Index * 3 + 2];
        Indices[Index * 3 + 2] = Temp;
    }
}

Void ConvertMeshVerticesToRightHanded(
    Mesh* Mesh
) {
    for (Int Index = 0; Index < Mesh->vertexCount; Index += 1) {
        Mesh->vertices[Index * 3 + 2] *= -1.0f;
    }
}

Void ConvertMeshNormalsToRightHanded(
    Mesh* Mesh
) {
    if (Mesh->normals != NULL) {
        for (Int Index = 0; Index < Mesh->vertexCount; Index += 1) {
            Mesh->normals[Index * 3 + 2] *= -1.0f;
        }
    }
}

Void ConvertMeshTexcoordsToRightHanded(
    Mesh* Mesh
) {
    if (Mesh->texcoords != NULL) {
        for (Int Index = 0; Index < Mesh->vertexCount; Index += 1) {
            Mesh->texcoords[Index * 2 + 1] = 1.0f - Mesh->texcoords[Index * 2 + 1];
        }
    }
}

Void ConvertMeshIndicesToRightHanded(
    Mesh* Mesh
) {
    if (Mesh->indices != NULL) {
        for (Int Index = 0; Index < Mesh->triangleCount; Index += 1) {
            int temp = Mesh->indices[Index * 3 + 1];
            Mesh->indices[Index * 3 + 1] = Mesh->indices[Index * 3 + 2];
            Mesh->indices[Index * 3 + 2] = temp;
        }
    }
}

Void ConvertMeshToRightHanded(
    Mesh* Mesh
) {
    ConvertMeshVerticesToRightHanded(Mesh);
    ConvertMeshNormalsToRightHanded(Mesh);
    ConvertMeshIndicesToRightHanded(Mesh);
}