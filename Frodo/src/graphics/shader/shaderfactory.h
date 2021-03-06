#pragma once

#include <fd.h>
#include "shader.h"

namespace FD {

enum FD_SHADERFACTORY_SHADER_TYPE {
	FD_DEFERRED_SHADER_UNKNOWN,
	FD_DEFERRED_SHADER_GEOMETRY,
	FD_DEFERRED_SHADER_DIRECTIONAL_LIGHT,
	FD_DEFERRED_SHADER_POINT_LIGHT,
	FD_DEFERRED_SHADER_SPOT_LIGHT,
	FD_FORWARD_SHADER_UNKNWON,
	FD_FORWARD_SHADER_DIRECTIONAL_LIGHT,
	FD_FORWARD_SHADER_POINT_LIGHT,
	FD_FORWARD_SHADER_SPOT_LIGHT,
	FD_FONT_UNKNOWN,
	FD_FONT_DEFAULT,
	FD_UI_UNKNOW,
	FD_UI_DEFAULT,
	FD_SPRITE_DEFAULT
};

class FDAPI ShaderFactory {
public:

	static Shader* GetShader(FD_SHADERFACTORY_SHADER_TYPE shader);
};

}