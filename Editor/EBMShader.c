#include "Context.h"
#include "EBMShader.h"

EBMShader EBMShaderLoad(
	EditorContextRef Context
) {
	EBMShader Shader = { 0 };
	Shader.Shader = LoadShader(
		PathCombineNoAlloc(Context->Config.Editor.ShaderDataPath, "New.vs"),
		PathCombineNoAlloc(Context->Config.Editor.ShaderDataPath, "New.fs")
	);
	Shader.UniformLocationTime = GetShaderLocation(Shader.Shader, "time");
	Shader.UniformLocationTexScroll = GetShaderLocation(Shader.Shader, "texScroll");
	Shader.UniformLocationTexScroll2 = GetShaderLocation(Shader.Shader, "texScroll2");
	Shader.UniformLocationTexFaceted = GetShaderLocation(Shader.Shader, "texFaceted");
	Shader.UniformLocationTexFaceted2 = GetShaderLocation(Shader.Shader, "texFaceted2");
	Shader.UniformLocationBlendOp = GetShaderLocation(Shader.Shader, "blendOp");
	Shader.UniformLocationSrcBlend = GetShaderLocation(Shader.Shader, "srcBlend");
	Shader.UniformLocationDestBlend = GetShaderLocation(Shader.Shader, "destBlend");
	Shader.UniformLocationTexBlendFactor = GetShaderLocation(Shader.Shader, "texBlendFactor");
	Shader.UniformLocationLightDir = GetShaderLocation(Shader.Shader, "lightDir");
	Shader.UniformLocationLightIntensity = GetShaderLocation(Shader.Shader, "lightIntensity");
	Shader.UniformLocationCameraPosition = GetShaderLocation(Shader.Shader, "cameraPosition");
	Shader.UniformLocationMaterialAmbient = GetShaderLocation(Shader.Shader, "materialAmbient");
	Shader.UniformLocationMaterialDiffuse = GetShaderLocation(Shader.Shader, "materialDiffuse");
	Shader.UniformLocationMaterialSpecular = GetShaderLocation(Shader.Shader, "materialSpecular");
	Shader.UniformLocationMaterialEmission = GetShaderLocation(Shader.Shader, "materialEmission");
	Shader.UniformLocationMaterialStrength = GetShaderLocation(Shader.Shader, "materialStrength");
	return Shader;
}

Void EBMShaderSetMaterial(
	EditorContextRef Context,
	EBMShader Shader,
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
	SetShaderValue(Shader.Shader, Shader.UniformLocationTime, &Time, SHADER_UNIFORM_FLOAT);
	
	Vector2 Scroll = Material->TextureMain.ScrollSpeed;
	//Scroll.x *= GetTime();
	//Scroll.y *= GetTime();
	SetShaderValue(Shader.Shader, Shader.UniformLocationTexScroll, &Scroll, SHADER_UNIFORM_VEC2);

	Scroll = Material->TextureBlend.ScrollSpeed;
	//Scroll.x *= GetTime();
	//Scroll.y *= GetTime();
	SetShaderValue(Shader.Shader, Shader.UniformLocationTexScroll2, &Scroll, SHADER_UNIFORM_VEC2);

	Float32 IsFaceted = Material->TextureMain.IsFaceted ? 1.0f : 0.0f;
	SetShaderValue(Shader.Shader, Shader.UniformLocationTexFaceted, &IsFaceted, SHADER_UNIFORM_FLOAT);

	IsFaceted = Material->TextureBlend.IsFaceted ? 1.0f : 0.0f;
	SetShaderValue(Shader.Shader, Shader.UniformLocationTexFaceted2, &IsFaceted, SHADER_UNIFORM_FLOAT);

	Int32 BlendOperation = 1;// Material->TextureBlend.BlendFlags & 0x2;
	SetShaderValue(Shader.Shader, Shader.UniformLocationBlendOp, &BlendOperation, SHADER_UNIFORM_INT);

	Int32 SrcBlend = 3;// (Material->TextureBlend.BlendFlags >> 2) & 0x2;
	SetShaderValue(Shader.Shader, Shader.UniformLocationSrcBlend, &SrcBlend, SHADER_UNIFORM_INT);

	Int32 DestBlend = 12;// (Material->TextureBlend.BlendFlags >> 4) & 0x2;
	SetShaderValue(Shader.Shader, Shader.UniformLocationDestBlend, &DestBlend, SHADER_UNIFORM_INT);

	Float32 texBlendFactor = (BlendMaterial) ? 1.0f : 0.0f;
	SetShaderValue(Shader.Shader, Shader.UniformLocationTexBlendFactor, &texBlendFactor, SHADER_UNIFORM_FLOAT);

	//Vector3 lightPos = { 10.0f, 10.0f, 10.0f };
	Vector3 lightDir = { 0.75, 0.5, -0.5 };
//	lightPos = Vector3Add(lightPos, Context->Camera.position);
	Float32 lightIntensity = 1.0f;
//	SetShaderValue(Shader, GetShaderLocation(Shader, "lightPosition"), &lightPos, SHADER_UNIFORM_VEC3);
	Float32 cameraPosition[3] = { Context->Camera.position.x, Context->Camera.position.y, Context->Camera.position.z };
	SetShaderValue(Shader.Shader, Shader.UniformLocationLightDir, &lightDir, SHADER_UNIFORM_VEC3);
	SetShaderValue(Shader.Shader, Shader.UniformLocationLightIntensity, &lightIntensity, SHADER_UNIFORM_FLOAT);
	SetShaderValue(Shader.Shader, Shader.UniformLocationCameraPosition, &cameraPosition, SHADER_UNIFORM_VEC3);

	SetShaderValue(Shader.Shader, Shader.UniformLocationMaterialAmbient, &Material->Properties.Ambient, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader.Shader, Shader.UniformLocationMaterialDiffuse, &Material->Properties.Diffuse, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader.Shader, Shader.UniformLocationMaterialSpecular, &Material->Properties.Specular, SHADER_UNIFORM_VEC4);
	SetShaderValue(Shader.Shader, Shader.UniformLocationMaterialEmission, &Material->Properties.Emission , SHADER_UNIFORM_VEC3);
	SetShaderValue(Shader.Shader, Shader.UniformLocationMaterialStrength, &Material->Properties.Strength, SHADER_UNIFORM_FLOAT);

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