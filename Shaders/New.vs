#version 330

#define MAX_BONE_MATRIX_COUNT 40

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
// in vec4 vertexColor;
// in vec4 vertexTangent;
//in vec2 vertexTexCoord2;
// in uvec4 vertexBoneIds;
//in vec4 vertexBoneWeights;
//in mat4 instanceTransform;

uniform mat4 mvp;
uniform mat4 matView;
//uniform mat4 matProjection;
uniform mat4 matModel;
uniform mat4 matNormal;
//uniform vec4 colDiffuse;
//uniform mat4 boneMatrices[MAX_BONE_MATRIX_COUNT];

uniform float time;
uniform vec2 texScroll;
uniform vec2 texScroll2;
uniform float texFaceted;
uniform float texFaceted2;
uniform vec3 cameraPosition;

out vec3 fragPosition;
out vec3 fragCameraPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec2 fragTexCoord2;

void main() {
    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));
    fragCameraPosition = cameraPosition;
    fragNormal = normalize(mat3(matNormal) * vertexNormal);

    vec3 normal = normalize(vertexNormal);
    vec2 faceted = vec2((normal.x + 1.0) * 0.5, (normal.y + 1.0) * 0.5);
    fragTexCoord = texFaceted * faceted + (1.0 - texFaceted) * vertexTexCoord;
    fragTexCoord = fragTexCoord + fract(texScroll * time);
    fragTexCoord2 = texFaceted2 * faceted + (1.0 - texFaceted2) * vertexTexCoord;
    fragTexCoord2 = fragTexCoord2 + fract(texScroll2 * time);
    gl_Position = mvp*vec4(vertexPosition , 1.0);
}
