#include "cpch.h"
#include "MainApp.h"
#include "GameManager.h"

bool MainApp::Initialize() {

	const wchar_t* className = L"Test Game";
	const wchar_t* WindowName = L"Test Game";

	if (FALSE == __super::Create(className, WindowName, 1024, 800)) {
		return false;
	}

	return TRUE;
}

void MainApp::Run() {
	MSG msg = { 0 };
	// message;
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// loop
			




		
		}
	}
}


void MainApp::Finalize()
{
	/*if (m_Renderer != NULL) {
		m_Renderer->Unititialize();
		m_Renderer.reset();
	}*/
}


bool MainApp::OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}


void MainApp::OnResize(int width, int height)
{
	__super::OnResize(width, height);
}


void MainApp::OnClose()
{
	std::cout << "OnClose" << std::endl;
}