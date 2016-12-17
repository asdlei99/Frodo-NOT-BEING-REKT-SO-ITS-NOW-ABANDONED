#include "shader.h"
#include <d3dcompiler.h>
#include <core/log.h>
#include <util/vfs/vfs.h>
#include <math/math.h>


void Shader::CreateBuffers() {

	for (size_t i = 0; i < vCBuffers.GetSize(); i++) {
		ShaderStructInfo* cbuffer = vCBuffers[i];

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = cbuffer->structSize;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.StructureByteStride = cbuffer->structSize;
		desc.Usage = D3D11_USAGE_DYNAMIC;

		D3DContext::GetDevice()->CreateBuffer(&desc, 0, &cbuffer->buffer);

	}
	
	for (size_t i = 0; i < pCBuffers.GetSize(); i++) {
		ShaderStructInfo* cbuffer = pCBuffers[i];

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = cbuffer->structSize;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.StructureByteStride = cbuffer->structSize;
		desc.Usage = D3D11_USAGE_DYNAMIC;

		D3DContext::GetDevice()->CreateBuffer(&desc, 0, &cbuffer->buffer);

	}
}

Shader::Shader(const String& vertexFilename, const String& pixelFilename, bool src) {
	inputLayout = nullptr;
	vByteCode = nullptr;
	pByteCode = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;

	String vSource;
	String pSource;

	if (src) {
		vSource = vertexFilename;
		pSource = pixelFilename;
	} else {
		vSource = VFS::Get()->ReadTextFile(vertexFilename);
		pSource = VFS::Get()->ReadTextFile(pixelFilename);
	}

	ID3DBlob* error = nullptr;

	D3DCompile(*vSource, vSource.length, 0, 0, 0, "vsMain", "vs_5_0", 0, 0, &vByteCode, &error);

	if (error) {
		FD_FATAL("VertexShader ERROR: %s", error->GetBufferPointer());
		DX_FREE(error);
		FD_ASSERT(vByteCode && "VertexShader failed to compile");
	}

	DX_FREE(error);

	D3DCompile(*pSource, pSource.length, 0, 0, 0, "psMain", "ps_5_0", 0, 0, &pByteCode, &error);

	if (error) {
		FD_FATAL("PixelShader ERROR: %s", error->GetBufferPointer());
		DX_FREE(error);
		FD_ASSERT(pByteCode && "PixelShader failed to compile");
	}

	DX_FREE(error);
	

	D3DContext::GetDevice()->CreateVertexShader(vByteCode->GetBufferPointer(), vByteCode->GetBufferSize(), 0, &vertexShader);

	FD_ASSERT(vertexShader && "Failed to create vertexshader");

	D3DContext::GetDevice()->CreatePixelShader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), 0, &pixelShader);

	FD_ASSERT(pixelShader && "Failed to create pixelshader");

	RemoveComments(vSource);
	RemoveComments(pSource);

	ParseStructs(vSource, FD_SHADER_TYPE_VERTEXSHADER);
	ParseStructs(pSource, FD_SHADER_TYPE_PIXELSHADER);

	ParseTextures(pSource);

	CreateBuffers();
}

Shader::~Shader() {
	DX_FREE(inputLayout);
	DX_FREE(vertexShader);
	DX_FREE(pixelShader);
	DX_FREE(vByteCode);
	DX_FREE(pByteCode);

	for (size_t i = 0; i < vCBuffers.GetSize(); i++)
		delete vCBuffers[i];

	for (size_t i = 0; i < pCBuffers.GetSize(); i++)
		delete pCBuffers[i];

	for (size_t i = 0; i < pTextures.GetSize(); i++)
		delete pTextures[i];
}

void Shader::Bind() {
	D3DContext::GetDeviceContext()->IASetInputLayout(inputLayout);

	D3DContext::GetDeviceContext()->VSSetShader(vertexShader, 0, 0);
	D3DContext::GetDeviceContext()->PSSetShader(pixelShader, 0, 0);


	for (size_t i = 0; i < vCBuffers.GetSize(); i++) {
		ShaderStructInfo& cb = *vCBuffers[i];
		D3DContext::GetDeviceContext()->VSSetConstantBuffers(cb.semRegister, 1, &cb.buffer);
	}

	for (size_t i = 0; i < pCBuffers.GetSize(); i++) {
		ShaderStructInfo& cb = *pCBuffers[i];
		D3DContext::GetDeviceContext()->PSSetConstantBuffers(cb.semRegister, 1, &cb.buffer);
	}
}

void Shader::SetVSConstantBufferSlotInternal(unsigned int slot, void* data) {
	ShaderStructInfo& cb = *vCBuffers[slot];
	D3D11_MAPPED_SUBRESOURCE sub;
	ZeroMemory(&sub, sizeof(D3D11_MAPPED_SUBRESOURCE));

	D3DContext::GetDeviceContext()->Map((ID3D11Resource*)cb.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
	memcpy(sub.pData, data, cb.structSize);
	D3DContext::GetDeviceContext()->Unmap((ID3D11Resource*)cb.buffer, 0);
}

void Shader::SetPSConstantBufferSlotInternal(unsigned int slot, void* data) {
	ShaderStructInfo& cb = *pCBuffers[slot];
	D3D11_MAPPED_SUBRESOURCE sub;
	ZeroMemory(&sub, sizeof(D3D11_MAPPED_SUBRESOURCE));

	D3DContext::GetDeviceContext()->Map((ID3D11Resource*)cb.buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
	memcpy(sub.pData, data, cb.structSize);
	D3DContext::GetDeviceContext()->Unmap((ID3D11Resource*)cb.buffer, 0);
}

void Shader::SetVSConstantBuffer(const String& bufferName, void* data) {
	size_t size = vCBuffers.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (vCBuffers[i]->name == bufferName) SetVSConstantBufferSlotInternal((unsigned int)i, data);
		return;
	}

	FD_WARNING("Buffer not found \"%s\"", *bufferName);
}

void Shader::SetPSConstantBuffer(const String& bufferName, void* data) {
	size_t size = pCBuffers.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (pCBuffers[i]->name == bufferName) SetPSConstantBufferSlotInternal((unsigned int)i, data);
		return;
	}

	FD_WARNING("Buffer not found \"%s\"", *bufferName);
}

void Shader::SetVSConstantBuffer(unsigned int slot, void* data) {
	size_t size = vCBuffers.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (vCBuffers[i]->semRegister == slot) {
			SetVSConstantBufferSlotInternal(slot, data);
			return;
		}
	}

	FD_WARNING("No buffer at slot %u", slot);
}

void Shader::SetPSConstantBuffer(unsigned int slot, void* data) {
	size_t size = pCBuffers.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (pCBuffers[i]->semRegister == slot) {
			SetPSConstantBufferSlotInternal((unsigned int)i, data);
			return;
		}
	}

	FD_WARNING("No buffer at slot %u", slot);
}

void Shader::SetTexture(unsigned int slot, const Texture* tex) {
	ID3D11ShaderResourceView* view = tex == nullptr ? nullptr : tex->GetResourceView();
	D3DContext::GetDeviceContext()->PSSetShaderResources(slot, 1, &view);
}

unsigned int Shader::GetVSConstantBufferSlotByName(const String& bufferName) {
	size_t size = vCBuffers.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (vCBuffers[i]->name == bufferName) return vCBuffers[i]->semRegister;
	}

	FD_FATAL("Buffer not found \"%s\"", *bufferName);
	return -1;
}

unsigned int Shader::GetPSConstantBufferSlotByName(const String& bufferName) {
	size_t size = pCBuffers.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (pCBuffers[i]->name == bufferName) return pCBuffers[i]->semRegister;
	}

	FD_FATAL("Buffer not found \"%s\"", *bufferName);
	return -1;
}

unsigned int Shader::GetPSTextureSlotByName(const String& textureName) {
	size_t size = pTextures.GetSize();
	for (size_t i = 0; i < size; i++) {
		if (pTextures[i]->name == textureName) return pTextures[i]->semRegister;
	}

	FD_FATAL("Texture not found \"%s\"", *textureName);
	return -1;
}