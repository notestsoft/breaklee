#include "Context.h"
#include "EBMShader.h"

const CString kEBMVertexShaderCode =
"#version 330 core\n"
"uniform mat4 uModel;\n"
"uniform mat4 uView;\n"
"uniform mat4 uProjection;\n"
"uniform vec2 uMainScroll;\n"
"uniform vec2 uBlendScroll;\n"
"uniform int uUseFaceted;\n"
"layout(location = 0) in vec3 aPosition;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"out vec3 vWorldPos;\n"
"out vec3 vNormal;\n"
"out vec2 vTexCoordMain;\n"
"out vec2 vTexCoordBlend;\n"
"void main() {\n"
"    vec4 worldPos = uModel * vec4(aPosition, 1.0);\n"
"    vNormal = (uUseFaceted == 1) ? normalize(aNormal) : normalize(mat3(transpose(inverse(uModel))) * aNormal);\n"
"    vTexCoordMain = aTexCoord + uMainScroll * worldPos.w;\n"
"    vTexCoordBlend = aTexCoord + uBlendScroll * worldPos.w;\n"
"    vWorldPos = worldPos.xyz;\n"
"    gl_Position = uProjection * uView * worldPos;\n"
"}\n";

const CString kEBMFragmentShaderCode =
"#version 330 core\n"
"uniform sampler2D uMainTexture;\n"
"uniform sampler2D uBlendTexture;\n"
"uniform vec4 uAmbient;\n"
"uniform vec4 uDiffuse;\n"
"uniform vec4 uSpecular;\n"
"uniform vec4 uEmission;\n"
"uniform float uStrength;\n"
"uniform vec3 uLightDir;\n"
"uniform float uBlendFlags;\n"
"uniform int uUseShadows;\n"
"uniform sampler2D uShadowMap;\n"
"uniform vec3 uShadowCoord;\n"
"in vec3 vWorldPos;\n"
"in vec3 vNormal;\n"
"in vec2 vTexCoordMain;\n"
"in vec2 vTexCoordBlend;\n"
"out vec4 FragColor;\n"
"float CalculateShadow(vec3 shadowCoord) {\n"
"    if (uUseShadows == 0) return 1.0;\n"
"    float shadow = texture(uShadowMap, shadowCoord.xy).r;\n"
"    return (shadowCoord.z - 0.005 > shadow) ? 0.5 : 1.0;\n"
"}\n"
"void main() {\n"
"    vec4 colorMain = texture(uMainTexture, vTexCoordMain);\n"
"    vec4 colorBlend = texture(uBlendTexture, vTexCoordBlend);\n"
"    vec4 blendedColor;\n"
"    if (uBlendFlags == 1) {\n"
"        blendedColor = mix(colorMain, colorBlend, 0.5);\n"
"    } else {\n"
"        blendedColor = colorMain;\n"
"    }\n"
"    vec3 normal = normalize(vNormal);\n"
"    vec3 light = normalize(-uLightDir);\n"
"    float diffuseFactor = max(dot(normal, light), 0.0);\n"
"    vec3 viewDir = normalize(-vWorldPos);\n"
"    vec3 reflectDir = reflect(-light, normal);\n"
"    float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 16.0) * uStrength;\n"
"    vec4 ambient = uAmbient * blendedColor;\n"
"    vec4 diffuse = uDiffuse * blendedColor * diffuseFactor;\n"
"    vec4 specular = uSpecular * specularFactor;\n"
"    vec4 emission = uEmission;\n"
"    float shadow = CalculateShadow(uShadowCoord);\n"
"    vec4 finalColor = (ambient + diffuse * shadow + specular) + emission;\n"
"    FragColor = finalColor;\n"
"}\n";

Shader EBMShaderLoad() {
	return LoadShaderFromMemory(kEBMVertexShaderCode, kEBMFragmentShaderCode);
}

Void EBMShaderSetMaterial(
	EditorContextRef Context,
	Shader Shader,
	EBMMaterialRef Material,
	EBMMaterialRef BlendMaterial,
	Matrix ModelTransform
) {
	Vector4 ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	Vector4 diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	Vector4 specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 emission = { 0.2f, 0.2f, 0.2f, 1.0f };
	float strength = 1.0f;

	// Set uniform values
	SetShaderValue(Shader, GetShaderLocation(Shader, "uAmbient"), &Material->Properties.Ambient, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uDiffuse"), &Material->Properties.Diffuse, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uSpecular"), &Material->Properties.Specular, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uEmission"), &Material->Properties.Emission, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uStrength"), &Material->Properties.Strength, SHADER_UNIFORM_FLOAT);
	SetShaderValueTexture(Shader, GetShaderLocation(Shader, "uMainTexture"), Material->TextureMain.Texture.Texture);
	SetShaderValueTexture(Shader, GetShaderLocation(Shader, "uBlendTexture"), BlendMaterial->TextureMain.Texture.Texture);

	Matrix View = GetCameraViewMatrix(&Context->Camera); // Get the view matrix from the camera
	Matrix Projection = GetCameraProjectionMatrix(&Context->Camera);

	SetShaderValueMatrix(Shader, GetShaderLocation(Shader, "uModel"), ModelTransform);
	SetShaderValueMatrix(Shader, GetShaderLocation(Shader, "uView"), viewMatrix);
	SetShaderValueMatrix(Shader, GetShaderLocation(Shader, "uProjection"), projectionMatrix);
}