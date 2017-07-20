#pragma once
#include <fd.h>
#include <core/window.h>
#include <graphics/render/camera/camera.h>
#include <entity/entity.h>
#include <graphics/render/light/light.h>

namespace FD {


class RenderCommand {
private:
	struct CBUFFER_DATA {
		uint32 slot;
		void* data;
	};

	byte numVCBuffers = 0;
	byte numPCBuffers = 0;
	
	CBUFFER_DATA* vCBuffers = nullptr;
	CBUFFER_DATA* pCBuffers = nullptr;
	
public:
	Mesh* mesh;
	mat4 transform;
	Shader* shader;

//	void AddVCBuffer();
};

class FDAPI Renderer : public EventListener {
protected:
	Window* window;

	List<RenderCommand> commandQueue;

	Renderer(Window* window) { this->window = window; }
public:
	virtual ~Renderer() {}

	virtual void Present() = 0;

	virtual void Begin(Camera* camera) = 0;
	virtual void Submit(const List<Light*>& lights) {}
	virtual void Submit(Light* light) {}

	virtual void Submit(const RenderCommand& cmd);
	virtual void Submit(Entity3D* e);
	virtual void Submit(Mesh* mesh, mat4 transform);

	virtual void End() = 0;


	inline Window* GetWindow() const { return window; }
	inline List<RenderCommand> GetCommandQueue() const { return commandQueue; }
};

}