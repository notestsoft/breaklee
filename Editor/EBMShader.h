#pragma once

#include "Base.h"
#include "EBMArchive.h"

EXTERN_C_BEGIN

Shader EBMShaderLoad(
	EditorContextRef Context
);

Void EBMShaderSetMaterial(
	EditorContextRef Context,
	Shader Shader,
	EBMMaterialRef Material,
	EBMMaterialRef BlendMaterial,
	Matrix ModelMatrix
);

EXTERN_C_END