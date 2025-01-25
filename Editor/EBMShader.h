#pragma once

#include "Base.h"
#include "EBMArchive.h"

EXTERN_C_BEGIN

EBMShader EBMShaderLoad(
	EditorContextRef Context
);

Void EBMShaderSetMaterial(
	EditorContextRef Context,
	EBMShader Shader,
	EBMMaterialRef Material,
	EBMMaterialRef BlendMaterial,
	Matrix ModelMatrix
);

EXTERN_C_END