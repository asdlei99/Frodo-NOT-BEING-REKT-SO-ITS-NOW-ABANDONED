#include "shadowmap2d.h"
#include <core/log.h>

namespace FD {

ShadowMap2D::ShadowMap2D(uint32 width, uint32 height) {
	this->width = width;
	this->height = height;
	this->bits = 32;

	D3D11_TEXTURE2D_DESC t2d = { 0 };

	t2d.ArraySize = 1;
	t2d.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	t2d.Format = DXGI_FORMAT_R32_TYPELESS;
	t2d.Width = width;
	t2d.Height = height;
	t2d.MipLevels = 1;
	t2d.SampleDesc.Count = 1;
	t2d.Usage = D3D11_USAGE_DEFAULT;

	D3DContext::GetDevice()->CreateTexture2D(&t2d, 0, (ID3D11Texture2D**)&resource);

	FD_ASSERT(resource == nullptr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srv;
	ZeroMemory(&srv, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srv.Format = DXGI_FORMAT_R32_FLOAT;
	srv.Texture2D.MipLevels = 1;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	D3DContext::GetDevice()->CreateShaderResourceView(resource, &srv, &resourceView);

	FD_ASSERT(resourceView == nullptr);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv;
	ZeroMemory(&dsv, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsv.Format = DXGI_FORMAT_D32_FLOAT;
	dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	

	D3DContext::GetDevice()->CreateDepthStencilView(resource, &dsv, &depthView);

	FD_ASSERT(depthView == nullptr);
}

ShadowMap2D::~ShadowMap2D() {

}

void ShadowMap2D::BindAsRenderTarget() {
	D3DContext::SetRenderTargets(0, nullptr, depthView);
	D3DContext::SetViewPort(0, 0, (float32)width, (float32)height);
}

}