#pragma once
#include <fd.h>
#include <core/log.h>
#include <graphics/d3dcontext.h>
#include <util/list.h>
#include <util/string.h>
#include <math/math.h>

class FDAPI FDBufferLayout {
private:
	friend class FDShader;
private:
	struct BufferLayoutAttrib {
		String name;
		DXGI_FORMAT format;
		unsigned int size;
		unsigned int offset;
	};

private:
	ID3D11InputLayout* layout;

	List<BufferLayoutAttrib> elements;

	unsigned int offset;

	void Push(const char* name, DXGI_FORMAT format);

public:
	FDBufferLayout() { offset = 0; }
	~FDBufferLayout();

	void CreateInputLayout(FDShader* shader);
	void Bind();

	template<typename T>
	inline void Push(const char* name) { FD_FATAL("Unsupported format"); }

	template<> inline void Push<vec2>(const char* name) { Push(name, DXGI_FORMAT_R32G32_FLOAT); }
	template<> inline void Push<vec3>(const char* name) { Push(name, DXGI_FORMAT_R32G32B32_FLOAT); }
	template<> inline void Push<vec4>(const char* name) { Push(name, DXGI_FORMAT_R32G32B32A32_FLOAT); }

	
};
