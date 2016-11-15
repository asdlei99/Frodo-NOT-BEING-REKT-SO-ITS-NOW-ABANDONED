#include "batchrenderer.h"

#include <graphics/shader/shaderfactory.h>

#define FD_FONT_MAX_SIMULTANEOUS_TEXTURES 4

#define FD_FONT_SHOW_TEXTURE 0

struct Vertex {
	vec2 position;
	vec2 texCoords;
	float tid;
};

void BatchRenderer::SetBlendingInternal(bool enable_blending) {
	float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	D3DContext::GetDeviceContext()->OMSetBlendState(blendState[enable_blending ? 1 : 0], factor, 0xFFFFFFFF);
}

void BatchRenderer::SetDepthInternal(bool enable_depthtesting) {
	D3DContext::GetDeviceContext()->OMSetDepthStencilState(depthState[enable_depthtesting ? 1 : 0], 0);
}

void BatchRenderer::CreateBlendStates() {

	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));


	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3DContext::GetDevice()->CreateBlendState(&desc, &blendState[0]);

	// ENABLED

	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3DContext::GetDevice()->CreateBlendState(&desc, &blendState[1]);
}


void BatchRenderer::CreateDepthStates() {


	//DEFAULT
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = false;

	D3DContext::GetDevice()->CreateDepthStencilState(&desc, &depthState[1]);

	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));


	//NO DEPTH

	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	desc.StencilEnable = false;

	D3DContext::GetDevice()->CreateDepthStencilState(&desc, &depthState[0]);
}


void BatchRenderer::Begin() {
	indexCount = 0;
	tids.Clear();
	buffer = (Vertex*)vbo->Map(FD_MAP_WRITE_DISCARD);
}

void BatchRenderer::End() {
	vbo->Unmap();
	buffer = nullptr;
}

float BatchRenderer::SubmitTexture(Texture2D* texture) {
	size_t numTids = tids.GetSize();

	float tid = 0;

	if (tids.Find(texture) == (size_t)-1) {
		if (tids.GetSize() == FD_FONT_MAX_SIMULTANEOUS_TEXTURES) {
			End();
			Render();
			Begin();

			numTids = 0;
		}

		tids.Push_back(texture);
		tid = (float)numTids + 1.0f;

	}
	else {
		tid = (float)tids.Find(texture) + 1.0f;
	}

	return tid;
}

BatchRenderer::BatchRenderer(Window* window, unsigned int max_vertices) : Renderer(window, nullptr) {
	this->maxVertices = max_vertices;
	this->indexCount = 0;
	this->buffer = nullptr;

	unsigned int* indices = new unsigned int[max_vertices * 6];

	for (unsigned int i = 0; i < max_vertices; i++) {
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4 + 0;
	}

	ibo = new IndexBuffer(indices, max_vertices * 6);
	vbo = new VertexBuffer(sizeof(Vertex), max_vertices * 4);

	delete[] indices;

	CreateBlendStates();
	CreateDepthStates();

	/*
	BufferLayout layout;

	layout.Push<vec2>("POSITION");
	layout.Push<vec2>("TEXCOORDS");
	layout.Push<float>("TID");

	shader = ShaderFactory::GetShader(FD_FONT_DEFAULT);
	#if FD_FONT_SHOW_TEXTURE
	shader->SetVSConstantBuffer("view_data", &mat4::Identity());
	#else
	shader->SetVSConstantBuffer("view_data", &mat4::Orthographic(0.0f, (float)window->GetWidth(), 0.0f, (float)window->GetHeight(), -0.1f, 0.1f));
	#endif

	layout.CreateInputLayout(shader);*/
}

BatchRenderer::BatchRenderer(Window* window, Camera* camera, unsigned int max_vertices) : BatchRenderer(window, max_vertices) {
	SetCamera(camera);
}

BatchRenderer::~BatchRenderer() {
	delete ibo;
	delete vbo;
	delete shader;

	DX_FREE(blendState[0]);
	DX_FREE(blendState[1]);
	DX_FREE(depthState[0]);
	DX_FREE(depthState[1]);
}

void BatchRenderer::Submit(Entity* e) {
	//TODO: implement
}

void BatchRenderer::Render() {
	SetBlendingInternal(blending);
	SetDepthInternal(depthTesting);

	shader->Bind();

	size_t num = tids.GetSize();
	for (size_t i = 0; i < num; i++) {
		shader->SetTexture((unsigned int)i, tids[i]);
	}

	vbo->Bind();
	ibo->Bind();

	D3DContext::GetDeviceContext()->DrawIndexed(indexCount, 0, 0);
}