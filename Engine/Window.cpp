#pragma once
#include "pch.h"
#include "Window.h"

LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	Window* pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (pWindow != nullptr)
	{
		if (pWindow->OnWndProc(hwnd, msg, wparam, lparam))
			return 0;
	}
	//msg ó��
	switch (msg)
	{
	case WM_SIZE:
		if (pWindow) pWindow->OnResize(LOWORD(lparam), HIWORD(lparam));
		return 0;

	case WM_CLOSE:
		if (pWindow) pWindow->OnClose();

		pWindow->Destroy();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

void Window::OnResize(int width, int height)
{
	m_width = width;
	m_height = height;
}


void Window::OnClose()
{
	std::cout << "closed" << std::endl;
}


bool Window::Create(const wchar_t* className, const wchar_t* windowName, int width, int height) {
	//WNDCLASSEXW wc;
	WNDCLASSEXW wc = {}; // ���� ȣȯ��
	HINSTANCE hInstance = GetModuleHandle(NULL);
	

	//windowStyle = WS_POPUP;  //<- �׵θ� ���� â, �߰����� �ʿ� //Draw�ϰų� �⺻ ���� �ְų�, ��ġ ���� �ϰų�
	windowStyle = WS_OVERLAPPEDWINDOW;

	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = className;
	wc.lpfnWndProc = NzWndProc; // window procedure ���
	wc.hInstance = hInstance;
	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	
	// ���� custom �ʿ�� ���
	/*wc.hIcon = LoadIcon(gameInstance, MAKEINTRESOURCE(IDI_DOGEENGINE));
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);*/

	ATOM classId = 0;

	if (!GetClassInfoEx(hInstance, className, &wc)) {
		classId = RegisterClassEx(&wc);
		if (classId == 0) {
			return false;
		}
	}
	m_width = width;
	m_height = height;

	RECT rc{0,0,m_width,m_height};
	AdjustWindowRect(&rc, windowStyle, FALSE);
	m_hWnd = CreateWindowW(className, windowName, windowStyle,
		100, 100, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	
	if (m_hWnd == NULL) {
		DWORD err = GetLastError();
		std::cout << "CreateWindow failed: " << err << std::endl;
		return false;
	}

	SetWindowLongPtr((HWND)m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow((HWND)m_hWnd, SW_SHOW);
	UpdateWindow((HWND)m_hWnd);

	return TRUE;
}

void Window::Destroy() {
	if (m_hWnd != NULL) {
		DestroyWindow((HWND)m_hWnd);
		m_hWnd = NULL;
	}
}


