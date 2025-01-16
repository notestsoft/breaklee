#include "Context.h"
#include "EBMShader.h"

Shader EBMShaderLoad(
	EditorContextRef Context
) {
	return LoadShader(
		PathCombineNoAlloc(Context->Config.Editor.ShaderDataPath, "New.vs"),
		PathCombineNoAlloc(Context->Config.Editor.ShaderDataPath, "New.fs")
	);
}

Void EBMShaderSetMaterial(
	EditorContextRef Context,
	Shader Shader,
	EBMMaterialRef Material,
	EBMMaterialRef BlendMaterial,
	Matrix ModelMatrix
) {
	/*
	uniform int blendOp;
	uniform int srcBlend;
	uniform int destBlend;
	*/
	Float32 Time = GetTime();
	SetShaderValue(Shader, GetShaderLocation(Shader, "time"), &Time, SHADER_UNIFORM_FLOAT);
	
	Vector2 Scroll = Material->TextureMain.ScrollSpeed;
	//Scroll.x *= GetTime();
	//Scroll.y *= GetTime();
	SetShaderValue(Shader, GetShaderLocation(Shader, "texScroll"), &Scroll, SHADER_UNIFORM_VEC2);

	Scroll = Material->TextureBlend.ScrollSpeed;
	//Scroll.x *= GetTime();
	//Scroll.y *= GetTime();
	SetShaderValue(Shader, GetShaderLocation(Shader, "texScroll2"), &Scroll, SHADER_UNIFORM_VEC2);

	Float32 IsFaceted = Material->TextureMain.IsFaceted ? 1.0f : 0.0f;
	SetShaderValue(Shader, GetShaderLocation(Shader, "texFaceted"), &IsFaceted, SHADER_UNIFORM_FLOAT);

	IsFaceted = Material->TextureBlend.IsFaceted ? 1.0f : 0.0f;
	SetShaderValue(Shader, GetShaderLocation(Shader, "texFaceted2"), &IsFaceted, SHADER_UNIFORM_FLOAT);

	Int32 BlendOperation = 1;// Material->TextureBlend.BlendFlags & 0x2;
	SetShaderValue(Shader, GetShaderLocation(Shader, "blendOp"), &BlendOperation, SHADER_UNIFORM_INT);

	Int32 SrcBlend = 3;// (Material->TextureBlend.BlendFlags >> 2) & 0x2;
	SetShaderValue(Shader, GetShaderLocation(Shader, "srcBlend"), &SrcBlend, SHADER_UNIFORM_INT);

	Int32 DestBlend = 12;// (Material->TextureBlend.BlendFlags >> 4) & 0x2;
	SetShaderValue(Shader, GetShaderLocation(Shader, "destBlend"), &DestBlend, SHADER_UNIFORM_INT);

	Float32 texBlendFactor = (BlendMaterial) ? 1.0f : 0.0f;
	SetShaderValue(Shader, GetShaderLocation(Shader, "texBlendFactor"), &texBlendFactor, SHADER_UNIFORM_FLOAT);

	//Vector3 lightPos = { 10.0f, 10.0f, 10.0f };
	Vector3 lightDir = { 0.75, 0.5, -0.5 };
//	lightPos = Vector3Add(lightPos, Context->Camera.position);
	Float32 lightIntensity = 1.0f;
//	SetShaderValue(Shader, GetShaderLocation(Shader, "lightPosition"), &lightPos, SHADER_UNIFORM_VEC3);
	Float32 cameraPosition[3] = { Context->Camera.position.x, Context->Camera.position.y, Context->Camera.position.z };
	SetShaderValue(Shader, GetShaderLocation(Shader, "lightDir"), &lightDir, SHADER_UNIFORM_VEC3);
	SetShaderValue(Shader, GetShaderLocation(Shader, "lightIntensity"), &lightIntensity, SHADER_UNIFORM_FLOAT);
	SetShaderValue(Shader, GetShaderLocation(Shader, "cameraPosition"), &cameraPosition, SHADER_UNIFORM_VEC3);

	SetShaderValue(Shader, GetShaderLocation(Shader, "materialAmbient"), &Material->Properties.Ambient, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "materialDiffuse"), &Material->Properties.Diffuse, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "materialSpecular"), &Material->Properties.Specular, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "materialEmission"), &Material->Properties.Emission , SHADER_UNIFORM_VEC3);
	SetShaderValue(Shader, GetShaderLocation(Shader, "materialStrength"), &Material->Properties.Strength, SHADER_UNIFORM_FLOAT);

	//Float32 Time = GetTime() * 0.001;
	//SetShaderValue(Shader, GetShaderLocation(Shader, "time"), &Time, SHADER_UNIFORM_FLOAT);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/*

	SetShaderValue(Shader, GetShaderLocation(Shader, "uAmbient"), &Material->Properties.Ambient, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uDiffuse"), &Material->Properties.Diffuse, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uSpecular"), &Material->Properties.Specular, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uEmission"), &Material->Properties.Emission, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uStrength"), &Material->Properties.Strength, SHADER_UNIFORM_FLOAT);
	SetShaderValueTexture(Shader, GetShaderLocation(Shader, "uMainTexture"), Material->TextureMain.Texture.Texture);

	Int UseBlend = BlendMaterial != 0;
	SetShaderValue(Shader, GetShaderLocation(Shader, "uUseBlend"), &UseBlend, SHADER_UNIFORM_INT);
	if (UseBlend) {
		SetShaderValueTexture(Shader, GetShaderLocation(Shader, "uBlendTexture"), BlendMaterial->TextureMain.Texture.Texture);
	}

	Int UseShadow = 0;
	SetShaderValue(Shader, GetShaderLocation(Shader, "uUseShadows"), &UseShadow, SHADER_UNIFORM_INT);

	SetShaderValue(Shader, GetShaderLocation(Shader, "uMainScroll"), &Material->TextureMain.ScrollSpeed, SHADER_UNIFORM_VEC2);
	SetShaderValue(Shader, GetShaderLocation(Shader, "uBlendScroll"), &Material->TextureBlend.ScrollSpeed, SHADER_UNIFORM_VEC2);

	Int IsFaceted = Material->TextureBlend.IsFaceted;
	SetShaderValue(Shader, GetShaderLocation(Shader, "uUseFaceted"), &IsFaceted, SHADER_UNIFORM_INT);

	Vector3 lightPos = (Vector3){ 10.0f, 10.0f, 10.0f };
	SetShaderValue(Shader, GetShaderLocation(Shader, "uLightDir"), &lightPos, SHADER_UNIFORM_VEC3);
	
	Matrix ViewMatrix = GetCameraViewMatrix(&Context->Camera);
	Matrix ProjectionMatrix = GetCameraProjectionMatrix(&Context->Camera, (Float32)GetScreenWidth() / GetScreenHeight());

	SetShaderValueMatrix(Shader, GetShaderLocation(Shader, "uModel"), ModelMatrix);
	SetShaderValueMatrix(Shader, GetShaderLocation(Shader, "uView"), ViewMatrix);
	SetShaderValueMatrix(Shader, GetShaderLocation(Shader, "uProjection"), ProjectionMatrix);
	*/
}