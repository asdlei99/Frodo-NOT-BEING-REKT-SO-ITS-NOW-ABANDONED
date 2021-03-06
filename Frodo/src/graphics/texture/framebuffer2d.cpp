#include <graphics/texture/framebuffer2d.h>
#include <core/log.h>

namespace FD {

Framebuffer2D::Framebuffer2D(uint32 width, uint32 height, FD_TEXTURE_FORMAT format, bool createDepthStencil) {
	this->width = width;
	this->height = height;

	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));

	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	td.Width = width;
	td.Height = height;
	td.SampleDesc.Count = 1;
	td.MipLevels = 1;
	td.Usage = D3D11_USAGE_DEFAULT;

	switch (format) {
		case FD_TEXTURE_FORMAT_UNKNOWN:
			FD_ASSERT(format == FD_TEXTURE_FORMAT_UNKNOWN);
		case FD_TEXTURE_FORMAT_FLOAT_D32:
			td.Format = DXGI_FORMAT_R32_FLOAT;
			break;
		case FD_TEXTURE_FORMAT_FLOAT_32_32_32_32:
			td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		default:
			FD_ASSERT(format == FD_TEXTURE_FORMAT_UNKNOWN);
	}

	D3DContext::GetDevice()->CreateTexture2D(&td, 0, (ID3D11Texture2D**)&resource);

	FD_ASSERT(resource == nullptr);

	D3D11_SHADER_RESOURCE_VIEW_DESC vd;
	ZeroMemory(&vd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	vd.Format = td.Format;
	vd.Texture2D.MipLevels = 1;
	vd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	D3DContext::GetDevice()->CreateShaderResourceView(resource, nullptr, &resourceView);

	FD_ASSERT(resourceView == nullptr);

	D3D11_RENDER_TARGET_VIEW_DESC rd;
	ZeroMemory(&rd, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	rd.Format = td.Format;
	rd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	D3DContext::GetDevice()->CreateRenderTargetView(resource, &rd, &targetView);

	FD_ASSERT(targetView == nullptr);

	depthView = nullptr;

	if (!createDepthStencil) return;

	ID3D11Texture2D* tmp = nullptr;

	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));

	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.Width = width;
	td.Height = height;
	td.SampleDesc.Count = 1;
	td.MipLevels = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.Format = DXGI_FORMAT_D32_FLOAT;

	D3DContext::GetDevice()->CreateTexture2D(&td, 0, &tmp);

	D3D11_DEPTH_STENCIL_VIEW_DESC dd;
	ZeroMemory(&dd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dd.Format = td.Format;
	dd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	D3DContext::GetDevice()->CreateDepthStencilView(tmp, &dd, &depthView);

	DX_FREE(tmp);

	FD_ASSERT(depthView == nullptr);

}

Framebuffer2D::~Framebuffer2D() {

}

void Framebuffer2D::BindAsRenderTarget() {
	D3DContext::SetRenderTargets(1, &targetView, depthView);
	D3DContext::SetViewPort(0, 0, (float32)width, (float32)height);
}

}