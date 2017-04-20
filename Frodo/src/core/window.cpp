#include "window.h"
#include "log.h"
#include "input.h"
#include <core/event/eventdispatcher.h>

#define FD_USE_LEGACY_INPUT 0

namespace FD {

Map<HWND, Window*> Window::window_handels;

LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
	Window* window = window_handels.Retrieve(hwnd);
	Event* e;
	static uint32 x = 0;
	static uint32 y = 0;
	static bool keys[65535];
	static bool buttons[3];
	
	switch (msg) {
		case WM_CLOSE:
			window->isOpen = false;
			break;
			#if FD_USE_LEGACY_INPUT
		case WM_LBUTTONDOWN:
			if (!buttons[FD_MOUSE_BUTTON_LEFT]) {
				e = new EventMouseActionButton(FD_PRESSED, FD_MOUSE_BUTTON_LEFT);
				buttons[FD_MOUSE_BUTTON_LEFT] = true;
			} else {
				e = new EventMouseActionButton(FD_HOLD, FD_MOUSE_BUTTON_LEFT);
			}
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_LBUTTONUP:
			e = new EventMouseActionButton(FD_RELEASED, FD_MOUSE_BUTTON_LEFT);
			EventDispatcher::DispatchEvent(e);
			buttons[FD_MOUSE_BUTTON_LEFT] = false;
			break;
		case WM_MBUTTONDOWN:
			if (!buttons[FD_MOUSE_BUTTON_MIDDLE]) {
				e = new EventMouseActionButton(FD_PRESSED, FD_MOUSE_BUTTON_MIDDLE);
				buttons[FD_MOUSE_BUTTON_MIDDLE] = true;
			} else {
				e = new EventMouseActionButton(FD_HOLD, FD_MOUSE_BUTTON_MIDDLE);
			}
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_MBUTTONUP:
			e = new EventMouseActionButton(FD_RELEASED, FD_MOUSE_BUTTON_MIDDLE);
			EventDispatcher::DispatchEvent(e);
			buttons[FD_MOUSE_BUTTON_MIDDLE] = false;
			break;
		case WM_RBUTTONDOWN:
			if (!buttons[FD_MOUSE_BUTTON_RIGHT]) {
				e = new EventMouseActionButton(FD_PRESSED, FD_MOUSE_BUTTON_RIGHT);
				buttons[FD_MOUSE_BUTTON_RIGHT] = true;
			} else {
				e = new EventMouseActionButton(FD_HOLD, FD_MOUSE_BUTTON_RIGHT);
			}
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_RBUTTONUP:
			e = new EventMouseActionButton(FD_RELEASED, FD_MOUSE_BUTTON_RIGHT);
			EventDispatcher::DispatchEvent(e);
			buttons[FD_MOUSE_BUTTON_RIGHT] = false;
			break;
		case WM_MOUSEMOVE:
			x = LOWORD(l);
			y = HIWORD(l);


			e = new EventMouseActionMove(ivec2(x, y));
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_KEYDOWN:
			FD_DEBUG("Key down: %c", (char)w);
			if (!keys[w]) {
				e = new EventKeyboardActionKey(FD_PRESSED, (int32)w);
				keys[w] = true;
			} else {
				e = new EventKeyboardActionKey(FD_HOLD, (int32)w);
			}
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_KEYUP:
			keys[w] = false;
			e = new EventKeyboardActionKey(FD_RELEASED, (int32)w);
			EventDispatcher::DispatchEvent(e);
			break;
			#endif
		case WM_MOVE:
			x = LOWORD(l);
			y = HIWORD(l);
			e = new EventWindowActionMove(ivec2(x, y));
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_SIZE:
			x = LOWORD(l);
			y = HIWORD(l);
			e = new EventWindowActionResize(ivec2(x, y));
			EventDispatcher::DispatchEvent(e);
			break;
		case WM_SYSCOMMAND:
			if ((w & 0xFFF0) == SC_MINIMIZE) {
				e = new EventWindowState(FD_MINIMIZED);
				EventDispatcher::DispatchEvent(e);
			} else if ((w & 0xFFF0) == SC_MAXIMIZE) {
				e = new EventWindowState(FD_MAXIMIZED);
				EventDispatcher::DispatchEvent(e);
			}
			break;
		case WM_SETFOCUS:
			e = new EventWindowState(FD_FOCUS_GAINED);
			EventDispatcher::DispatchEvent(e);
			Input::AcquireMouse();
			Input::AcquireKeyboard();
			break;
		case WM_KILLFOCUS:
			e = new EventWindowState(FD_FOCUS_LOST);
			EventDispatcher::DispatchEvent(e);
			Input::UnacquireMouse();
			Input::UnacquireKeyboard();
			break;
	}

	return DefWindowProc(hwnd, msg, w, l);
}

Window::Window(const String& title, int32 width, int32 height) : title(title), width(width), height(height) {
	FD_DEBUG("Creating window Title<%s> Width<%d> Height<%d>!", *title, width, height);

	WNDCLASSEX ws;
	
	ws.cbClsExtra = 0;
	ws.cbSize = sizeof(WNDCLASSEX);
	ws.cbWndExtra = 0;
	ws.hbrBackground = 0;
	ws.hCursor = LoadCursor(0, IDC_ARROW);
	ws.hIcon = LoadIcon(0, IDI_WINLOGO);
	ws.hIconSm = 0;
	ws.hInstance = 0;
	ws.lpfnWndProc = (WNDPROC)WndProc;
	ws.lpszClassName = "Frodo Window";
	ws.lpszMenuName = 0;
	ws.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&ws)) {
		FD_FATAL("Failed to register (WNDCLASSEX)");
		return;
	}

	RECT r = { 0,0,width, height };

	AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);

	if (!(hwnd = CreateWindow("Frodo Window", *title, WS_OVERLAPPEDWINDOW, (GetSystemMetrics(SM_CXSCREEN) >> 1) - (width >> 1), (GetSystemMetrics(SM_CYSCREEN) >> 1) - (height >> 1), r.right - r.left, r.bottom - r.top, 0, 0, 0, 0))) {
		FD_FATAL("Failed to create window (HWND)");
		return;
	}

	D3DContext::CreateContext(this);
	
	isOpen = true;
	SetVisible(true);
	SetVSync(0);
	Input::InitializeDirectInput(this);
	Input::InitializeMouse("Mouse");
	Input::InitializeKeyboard("Keyboard");

	window_handels.Add(hwnd, this);

	clearColor[3] = 1;
}


Window::~Window() {
	FD_DEBUG("Closing window");
	D3DContext::Dispose();
}

void Window::SwapBuffers() {

	MSG msg;
	if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	D3DContext::Present(vSync, 0);
}

void Window::Clear() {
	D3DContext::Clear();
}

void Window::SetVisible(bool visible) {
	isVisible = visible;
	if (isVisible)
		ShowWindow(hwnd, SW_SHOW);
	else
		ShowWindow(hwnd, SW_HIDE);

	FD_DEBUG("Window(%s) visibility set to %s", *title, isVisible ? "TRUE" : "FALSE");
}


ivec2 Window::GetMonitorDpi() {
	HDC m = GetDC(0);

	int32 x = GetDeviceCaps(m, LOGPIXELSX);
	int32 y = GetDeviceCaps(m, LOGPIXELSY);

	ReleaseDC(0, m);

	return ivec2(x, y);
}

ivec2 Window::GetWindowDpi() {
	HDC m = GetDC(hwnd);

	int32 x = GetDeviceCaps(m, LOGPIXELSX);
	int32 y = GetDeviceCaps(m, LOGPIXELSY);

	ReleaseDC(hwnd, m);

	return ivec2(x, y);
}

bool Window::OnWindowActionResize(ivec2 size) {

	width = size.x;
	height = size.y;

	return false;
}

}