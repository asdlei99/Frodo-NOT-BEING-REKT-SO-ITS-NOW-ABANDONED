#include "fontrenderer.h"

#include <graphics/shader/shaderfactory.h>
#include <math/vec4.h>
#include <math/vec2.h>

#define FD_FONT_SHOW_TEXTURE 0

struct Vertex {
	vec2 position;
	vec2 texCoords;
	vec4 color;
	float tid;
};

FontRenderer::FontRenderer(Window* window, unsigned int max_glyphs) : BatchRenderer(window, max_glyphs) { 
	blending = true; 
	depthTesting = false; 

	BufferLayout layout;

	layout.Push<vec2>("POSITION");
	layout.Push<vec2>("TEXCOORDS");
	layout.Push<vec4>("COLOR");
	layout.Push<float>("TID");

	

	shader = ShaderFactory::GetShader(FD_FONT_DEFAULT);
#if FD_FONT_SHOW_TEXTURE
	shader->SetVSConstantBuffer("view_data", &mat4::Identity());
#else
	shader->SetVSConstantBuffer("view_data", &mat4::Orthographic(0.0f, (float)window->GetWidth(), 0.0f, (float)window->GetHeight(), -0.1f, 0.1f));
#endif

	layout.CreateInputLayout(shader);

	
	unsigned int* indices = new unsigned int[max_glyphs * 6];

	for (unsigned int i = 0; i < max_glyphs; i++) {
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 3;
		indices[i * 6 + 5] = i * 4 + 0;
	}

	ibo = new IndexBuffer(indices, max_glyphs * 6);
	vbo = new VertexBuffer(sizeof(Vertex), max_glyphs * 4);

	delete[] indices;

	CreateBlendStates();
	CreateDepthStates();
}

FontRenderer::~FontRenderer() {

}

void FontRenderer::SubmitText(const String& text, Font* font, vec2 position, vec4 color) {
	//if (buffer == nullptr) Begin();
	float tid  = SubmitTexture(font->GetTexture());

	size_t textLength = text.length;
	float size = (float)font->GetSize();

	ivec2 dpi = window->GetMonitorDpi();

	float xPos = position.x;
	float yPos = position.y;

	Font::FD_GLYPH prevGlyph;

	for (size_t i = 0; i < textLength; i++) {
		unsigned int c = text[i];
		if (c == ' ') {
			xPos += size / 2.0f;
			prevGlyph.unicodeCharacter = 0;
			continue;
		}
		Font::FD_GLYPH glyph = font->GetGlyph(c);

		float xa = xPos + glyph.offset.x;
		float ya = yPos - glyph.offset.y;

#if (!FD_FONT_SHOW_TEXTURE)

		buffer->position = vec2(xa, ya);
		buffer->texCoords = vec2(glyph.u0, glyph.v0);
		buffer->color = color;
		buffer->tid = tid;
		buffer++;

		buffer->position = vec2(xa + size, ya);
		buffer->texCoords = vec2(glyph.u1, glyph.v0);
		buffer->color = color;
		buffer->tid = tid;
		buffer++;

		buffer->position = vec2(xa + size, ya + size);
		buffer->texCoords = vec2(glyph.u1, glyph.v1);
		buffer->color = color;
		buffer->tid = tid;
		buffer++;

		buffer->position = vec2(xa, ya + size);
		buffer->texCoords = vec2(glyph.u0, glyph.v1);
		buffer->color = color;
		buffer->tid = tid;
		buffer++;

#else
	

		buffer->position = vec2(-1, 1);
		buffer->texCoords = vec2(0, 0);
		buffer->tid = tid;
		buffer++;

		buffer->position = vec2(1, 1);
		buffer->texCoords = vec2(1, 0);
		buffer->tid = tid;
		buffer++;

		buffer->position = vec2(1, -1);
		buffer->texCoords = vec2(1, 1);
		buffer->tid = tid;
		buffer++;

		buffer->position = vec2(-1, -1);
		buffer->texCoords = vec2(0, 1);
		buffer->tid = tid;
		buffer++;

#endif

		indexCount += 6;

		xPos += glyph.advance.x;

		prevGlyph = glyph;
	}
}
