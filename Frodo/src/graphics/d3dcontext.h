#pragma once
#include <fd.h>

#include <d3d11.h>
#include <d3d10.h>
#include <dxgi.h>

#include <Windows.h>

#define DX_FREE(x) if (x != nullptr) { x->Release(); x = nullptr;}

class FDAPI D3DContext {
private:
	friend class Window;
private:
	static D3DContext* pContext;

private:
	ID3D11RenderTargetView* activeRenderTarget;
	ID3D11DepthStencilView* activeDepthStencilView;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* renderTarget;
	ID3D11DepthStencilView* depthStencilView;
	IDXGISwapChain* swapChain;

	Window* window;

	D3DContext();
	~D3DContext();
public:

	static void CreateContext(Window* window);
	static void Dispose();

	static void Present(unsigned int syncInterval, unsigned int flags);
	static void Clear();

	static void SetRenderTarget(ID3D11RenderTargetView* target);
	static void SetRenderTargets(ID3D11RenderTargetView* target, ID3D11DepthStencilView* depthView);
	static void SetViewPort(float topLeftX, float topLeftY, float width, float height);
	static void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology);

	__forceinline static D3DContext* GetContext() { return pContext; }
	__forceinline static ID3D11Device* GetDevice() {return pContext->device;}
	__forceinline static ID3D11DeviceContext* GetDeviceContext() { return pContext->context; }
	__forceinline static IDXGISwapChain* GetSwapChain() { return  pContext->swapChain; }


};
