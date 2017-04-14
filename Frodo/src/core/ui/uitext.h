#pragma once

#include <math/math.h>
#include <util/string.h>
#include <graphics/render/renderer/fontrenderer.h>
#include "uiitem.h"

namespace FD {
	

class FDAPI UIText {
private:
	friend class UIItem;
protected:
	UIItem* parent;
	String name;

	String text;
	Font* font;

	vec2 position;
	vec2 offset;
	vec2 scale;
	vec4 color;

	FD_TEXT_ALIGNMENT alignment;
public:
	UIText(const String& name, vec2 position, Font* font, const String& text) : name(name), text(text), font(font), position(position), scale(vec2(1, 1)), color(vec4(1, 1, 1, 1)) {}
	virtual ~UIText() {}

	inline UIItem* GetParent() const { return parent; }
	inline const String& GetName() const { return name; }
	inline const String& GetText() const { return text; }
	inline Font* GetFont() const { return font; }
	inline vec2 GetPosition() const { return position; }
	inline vec2 GetOffset() const { return offset; }
	inline vec2 GetScale() const { return scale; }
	inline vec4 GetColor() const { return color; }
	inline FD_TEXT_ALIGNMENT GetTextAlignment() const { return alignment; }

	inline void SetParent(UIItem* parent) { this->parent = parent; }
	inline void SetName(const String& name) { this->name = name; }
	virtual void SetText(const String& name) { this->text = text; }
	inline void SetFont(Font* font) { this->font = font; }
	inline void SetPosition(const vec2& position) { this->position = position; }
	inline void SetOffset(const vec2& offset) { this->offset = offset; }
	inline void SetScale(const vec2& scale) { this->scale = scale; }
	inline void SetColor(const vec4& color) { this->color = color; }
	virtual void SetTextAlignment(FD_TEXT_ALIGNMENT alignment) { this->alignment = alignment; }
};

class FDAPI UITextAutoResize : public UIText {
protected:
	enum FD_SCALE_MODE {
		FD_SCALE_XY,
		FD_SCALE_ONLY_X,
		FD_SCALE_ONLY_Y
	};

	void RecalculateScale(FD_SCALE_MODE mode);

	vec2 orgPosition;
	vec2 lastPosition;

	vec2 margin;
	vec2 adjustSize;

public:
	UITextAutoResize(const String& name, vec2 position, vec2 adjustSize, Font* font, const String& text);

	inline vec2 GetAdjustSize() const { return adjustSize; }

	void SetMargin(const vec2& margin);
	void SetAdjustSize(const vec2& size);
	virtual void SetText(const String& text) override;
	virtual void SetTextAlignment(FD_TEXT_ALIGNMENT align) override;
};

class FDAPI UITextHorizontalScroll : public UITextAutoResize {
private:
	friend class UIItem;

	class UITextHorizontalScroll_Cursor : public UIItem {
	public:
		UITextHorizontalScroll_Cursor(const String& name, vec2 position, vec2 size);
	};

private:
	uint32 cursorLocation;

	String orgText;

	void CalculateString();

public:
	UITextHorizontalScroll(const String& name, vec2 position, vec2 size, Font* font);

	void Append(const String& text);
	void Append(const char character);

	void SetText(const String& text) override;
	inline uint32 GetCursorLocation() const { return cursorLocation; }

	inline void SetCursorLocation(uint32 cursorLocation) { this->cursorLocation = cursorLocation; }
};

#ifndef FD_BUILD
extern template UIText* UIItem::GetText<UIText>(const String&);

#endif

}