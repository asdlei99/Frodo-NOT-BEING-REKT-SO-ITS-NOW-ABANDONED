#include "ui.h"

namespace FD {

UITextBox::UITextBox(const String& name, vec2 position, vec2 size) : UIItem(name, position, size) {
	text = new UITextHorizontalScroll("content", vec2(0, 0), size, Font::GetDefaultFont());
	text->SetTextAlignment(FD_TEXT_ALIGN_LEFT);
	text->SetParent(this);

	texts.Push_back(text);

	SetTexture(nullptr);
	SetVisible(true);
	SetInteractable(true);
}

void UITextBox::OnKey(uint32 key) {
	if (key == VK_BACK) {
		text->Remove();
	} else if (key == VK_LEFT) {
	} else if (key == VK_RIGHT) {
	} else {
		Font* font = text->GetFont();
		for (uint_t i = 0; i < font->GetNumRanges(); i++) {
			Font::FD_RANGE<> r = font->GetRanges()[i];
			if (key >= r.start && key <= r.end) {
				text->Append((const char)key);
				break;
			}
		}
	}

	
}

void UITextBox::OnFocus() {
	text->EnableCursor();
}

void UITextBox::OnFocusLost() {
	text->DisableCursor();
}

}