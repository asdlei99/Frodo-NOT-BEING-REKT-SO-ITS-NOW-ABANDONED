#include "material.h"

#include <core/log.h>

namespace FD {

MaterialInstance::MaterialInstance(Material* material) : parent(material) {
	vCBuffer = material->GetVCBuffer();
	vCBuffer.data = vCBuffer.structSize == 0 ? nullptr : new byte[vCBuffer.structSize];
	memcpy(vCBuffer.data, material->GetVCBuffer().data, vCBuffer.structSize);

	pCBuffer = material->GetPCBuffer();
	pCBuffer.data = pCBuffer.structSize == 0 ? nullptr : new byte[pCBuffer.structSize];
	memcpy(pCBuffer.data, material->GetPCBuffer().data, pCBuffer.structSize);

	textures = material->GetTextures();
	shader = material->GetShader();
}

MaterialInstance::MaterialInstance(const Material& material) : parent(nullptr) {
	vCBuffer = material.GetVCBuffer();
	vCBuffer.data = vCBuffer.structSize == 0 ? nullptr : new byte[vCBuffer.structSize];
	memcpy(vCBuffer.data, material.GetVCBuffer().data, vCBuffer.structSize);

	pCBuffer = material.GetPCBuffer();
	pCBuffer.data = pCBuffer.structSize == 0 ? nullptr : new byte[pCBuffer.structSize];
	memcpy(pCBuffer.data, material.GetPCBuffer().data, pCBuffer.structSize);

	textures = material.GetTextures();
	shader = material.GetShader();
}

MaterialInstance::~MaterialInstance() {
	delete vCBuffer.data;
	delete pCBuffer.data;
}

void MaterialInstance::Bind() {
	if (vCBuffer.data) shader->SetVSConstantBuffer(vCBuffer);
	if (pCBuffer.data) shader->SetPSConstantBuffer(pCBuffer);

	uint_t size = textures.GetKeyList().GetSize();

	for (uint_t i = 0; i < size; i++) {
		FD_MAP_PAIR<uint32, Texture*> pair = textures.GetPair(i);
		shader->SetTexture(pair.key, pair.data);
	}

	shader->Bind();
}

void MaterialInstance::SetTexture(const String& name, Texture* texture) {
	textures[shader->GetTextureInfo(name).semRegister] = texture;
}

void MaterialInstance::SetVCBuffer(const String& name, void* data) {
	delete[] vCBuffer.data;
	vCBuffer = shader->GetVSConstantBufferInfo(name);

	vCBuffer.data = new byte[vCBuffer.structSize];

	uint32 size = vCBuffer.structSize;
	uint32 toCopy = size;

	if (size != vCBuffer.structSize) {
		if (vCBuffer.structSize > size) {
			FD_WARNING("[Material] Not all of the buffer will be set! Constant Buffer \"%s\" is size %u, supplied size is %u.", *name, vCBuffer.structSize, size);
		} else {
			toCopy = vCBuffer.structSize;
			FD_WARNING("[Material] Truncating data! Constant Buffer \"%s\" is size %u, supplied size is %u.", *name, vCBuffer.structSize, size);
		}
	}

	if (data) memcpy(vCBuffer.data, data, toCopy);
	else memset(vCBuffer.data, 0, toCopy);
}

void MaterialInstance::SetPCBuffer(const String& name, void* data) {
	delete[] pCBuffer.data;
	pCBuffer = shader->GetPSConstantBufferInfo(name);

	pCBuffer.data = new byte[pCBuffer.structSize];

	uint32 size = pCBuffer.structSize;
	uint32 toCopy = size;

	if (size != pCBuffer.structSize) {
		if (pCBuffer.structSize > size) {
			FD_WARNING("[Material] Not all of the buffer will be set! Constant Buffer \"%s\" is size %u, supplied size is %u.", *name, pCBuffer.structSize, size);
		} else {
			toCopy = pCBuffer.structSize;
			FD_WARNING("[Material] Truncating data! Constant Buffer \"%s\" is size %u, supplied size is %u.", *name, pCBuffer.structSize, size);
		}
	}

	if (data) memcpy(pCBuffer.data, data, toCopy);
	else memset(pCBuffer.data, 0, toCopy);
}



void MaterialInstance::SetVCBufferElement(const String& name, void* data) {
	vCBuffer.SetElement(name, data);
}

void MaterialInstance::SetPCBufferElement(const String& name, void* data) {
	pCBuffer.SetElement(name, data);
}

void MaterialInstance::SetVCBufferElement(uint32 index, void* data) {
	vCBuffer.SetElement(index, data);
}

void MaterialInstance::SetPCBufferElement(uint32 index, void* data) {
	pCBuffer.SetElement(index, data);
}

}