#include "shader.h"
#include <math/math.h>

enum FD_SHADER_FIELD_TYPE {
	FD_SHADER_FIELD_TYPE_UNKOWN,
	FD_SHADER_FIELD_TYPE_MAT4,
	FD_SHADER_FIELD_TYPE_MAT3,
	FD_SHADER_FIELD_TYPE_VEC4,
	FD_SHADER_FIELD_TYPE_VEC3,
	FD_SHADER_FIELD_TYPE_VEC2,
	FD_SHADER_FIELD_TYPE_FLOAT
};


inline static String get_field_type_as_string(FD_SHADER_FIELD_TYPE type) {
	switch (type) {
	case FD_SHADER_FIELD_TYPE_UNKOWN: return ("UNKOWN");
	case FD_SHADER_FIELD_TYPE_MAT4: return ("float4x4");
	case FD_SHADER_FIELD_TYPE_MAT3: return ("float3x3");
	case FD_SHADER_FIELD_TYPE_VEC4: return ("float4");
	case FD_SHADER_FIELD_TYPE_VEC3: return ("float3");
	case FD_SHADER_FIELD_TYPE_VEC2: return ("float2");
	case FD_SHADER_FIELD_TYPE_FLOAT: return ("float");
	}
	return ("ERROR");
}

inline static unsigned int get_field_type_size(FD_SHADER_FIELD_TYPE type) {
	switch (type) {
	case FD_SHADER_FIELD_TYPE_UNKOWN: return 0;
	case FD_SHADER_FIELD_TYPE_MAT4: return sizeof(mat4);
	case FD_SHADER_FIELD_TYPE_MAT3: return sizeof(mat3);
	case FD_SHADER_FIELD_TYPE_VEC4: return sizeof(vec4);
	case FD_SHADER_FIELD_TYPE_VEC3: return sizeof(vec3);
	case FD_SHADER_FIELD_TYPE_VEC2: return sizeof(vec2);
	case FD_SHADER_FIELD_TYPE_FLOAT: return sizeof(float);
	}

	return 0;
}

void Shader::RemoveComments(String& source) {

	while (true) {
		size_t start = source.Find("/*");

		if (start == -1) break;

		size_t end = source.Find("*/", start);

		source.Remove(start, end + 2);
	}

	while (true) {
		size_t start = source.Find("//");

		if (start == -1) break;

		size_t end = source.Find("\n", start);
		if (end == -1) end = source.length;

		source.Remove(start, end);
	}

}

void Shader::ParseStructs(String source, FD_SHADER_TYPE type) {

	while (true) {
		size_t cbufferStart = source.Find("cbuffer") + 7;

		if (cbufferStart < 7) break;

		size_t colon = source.Find(":", cbufferStart);

		String name(source.str + cbufferStart, colon - cbufferStart);
		name.RemoveBlankspace();

		size_t regIndex = source.Find("register", colon) + 10;

		ShaderStructInfo* cbuffer = new ShaderStructInfo;

		cbuffer->name = name;
		cbuffer->semRegister = atoi(*source + regIndex);

		CalcStructSize(source, cbufferStart - 7, cbuffer);

		switch (type) {
		case FD_SHADER_TYPE_VERTEXSHADER:
			vCBuffers.Push_back(cbuffer);
			break;
		case FD_SHADER_TYPE_PIXELSHADER:
			pCBuffers.Push_back(cbuffer);
			break;
		}
	}


}

void Shader::CalcStructSize(String& structSource, size_t offset, ShaderStructInfo* cbuffer) {

	FD_SHADER_FIELD_TYPE types[6]{
		FD_SHADER_FIELD_TYPE_MAT4,
		FD_SHADER_FIELD_TYPE_MAT3,
		FD_SHADER_FIELD_TYPE_VEC4,
		FD_SHADER_FIELD_TYPE_VEC3,
		FD_SHADER_FIELD_TYPE_VEC2,
		FD_SHADER_FIELD_TYPE_FLOAT };

	size_t end = structSource.Find("};", offset);

	String fields(*structSource + offset, end - offset);

	size_t numFields = fields.Count(";");

	size_t currSemicolon = fields.Find(";");
	size_t fieldOffset = 0;

	cbuffer->structSize = 0;

	for (size_t num = 0; num < numFields; num++) {
		for (size_t i = 0; i < 6; i++) {
			size_t index = fields.Find(get_field_type_as_string(types[i]), fieldOffset);

			if (index > currSemicolon || index == -1) continue;

			cbuffer->structSize += get_field_type_size(types[i]);

			fieldOffset = currSemicolon;
			currSemicolon = fields.Find(";", currSemicolon + 1);

			break;
		}
	}

	structSource.Remove(offset, end + 2);
}

void Shader::ParseTextures(String source) {

	while (true) {
		size_t textureStart = source.Find("Texture");

		if (textureStart < (size_t)0) break;

		ShaderTextureInfo* tex = new ShaderTextureInfo;
		tex->numTextures = 1;

		String type = source.SubString(textureStart + 7, source.Find(" ", textureStart + 7)).RemoveBlankspace();

		if (type == "1D ") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURE1D;
		}
		else if (type == "2D ") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURE2D;
		}
		else if (type == "3D ") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURE3D;
		}
		else if (type == "1DArray") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURE1D_ARRAY;
		}
		else if (type == "2DArray") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURE2D_ARRAY;
		}
		else if (type == "CubeArray") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURECUBE_ARRAY;
		}
		else if (type == "Cube") {
			tex->type = FD_SHADER_TEXTURE_TYPE_TEXTURECUBE;
		}
		else {
			break;
		}


		size_t nameStart = source.Find(" ", textureStart);
		size_t nameEnd = source.Find(":", nameStart);

		tex->name = source.SubString(nameStart, nameEnd).RemoveBlankspace();

		size_t regStart = source.Find("register(t", nameEnd) + 10;

		tex->semRegister = atoi(*source + regStart);

		source.Remove(textureStart, source.Find(";", regStart)+1);

		pTextures.Push_back(tex);
	}

}