#pragma once

#include "framebuffer.h"

namespace FD {

class FDAPI ShadowMap2D : public Framebuffer {
public:
	ShadowMap2D(uint32 width, uint32 height);
	~ShadowMap2D();

	void BindAsRenderTarget() override;
};

}
