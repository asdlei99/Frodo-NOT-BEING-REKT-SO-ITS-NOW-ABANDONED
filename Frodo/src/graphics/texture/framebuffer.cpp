#include "framebuffer.h"

namespace FD {

void Framebuffer::Bind(uint32 slot) const {

	D3DContext::GetDeviceContext()->PSSetShaderResources(slot, 1, &resourceView);
}

}