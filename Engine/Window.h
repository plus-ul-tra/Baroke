#pragma once
#include <Windows.h>
LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Window
{
protected:

	HWND m_hWnd = HWND();
	int m_width = 0;
	int m_height = 0;

	long long windowStyle;

	friend LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	bool OnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) { return false; }
	void OnResize(int width, int height);
	void OnClose();  //?

	//복사 이동생성자,연산 막기
	Window(const Window&) = delete;
	Window(Window&&) = delete;
	Window& operator =(const Window&) = delete;
	Window& operator =(const Window&&) = delete;
		
public:
	Window() = default;
	virtual ~Window() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
	void Destroy();

	void* GetHandle() const { return m_hWnd; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

};

