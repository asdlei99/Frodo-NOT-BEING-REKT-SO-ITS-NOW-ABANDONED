#pragma once
#include <fd.h>
#include <graphics/texture/texture2d.h>
#include <math/math.h>


class FDAPI Renderable2D {
private:
	vec2 position;
	vec2 size;
	float rotation;

	unsigned int color;
	Texture2D* texture;

public:
	Renderable2D(const vec2& position, const vec2& size, float rotation, unsigned int color, Texture2D* texture);
	Renderable2D(const vec2& position, const vec2& size, unsigned int color, Texture2D* texture);
	Renderable2D(const vec2& position, const vec2& size, unsigned int color);
	Renderable2D(const vec2& position, const vec2& size, Texture2D* texture);
	Renderable2D(const vec2& position, unsigned int color, Texture2D* texture);
	Renderable2D(const vec2& position, unsigned int color);
	Renderable2D(const vec2& position, Texture2D* texture);


	inline vec2& GetPosition() { return position; }
	inline vec2& GetScale() { return size; }
	inline float& GetRotation() { return rotation; }
	inline unsigned int& GetColor() { return color; }
	inline Texture2D* GetTexture() { return texture; }
};
