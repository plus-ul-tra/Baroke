#include "pch.h"
#include "InputManager.h"


// inline void GetVkName(const RAWKEYBOARD& kb, WCHAR* out, int cch)
// {
// 	if (cch <= 0 || !out) return;
// 
// 	// 스캔코드+확장 플래그를 lParam 형식으로 변환
// 	UINT scan = kb.MakeCode;
// 	if (kb.Flags & RI_KEY_E0)
// 		scan |= 0xE000;            // 확장 키(예: 오른쪽 Ctrl, 화살표 등)
// 
// 	LONG lParam = static_cast<LONG>(scan << 16);
// 	if (kb.Flags & RI_KEY_E0)
// 		lParam |= (1 << 24);
// 
// 	if (!GetKeyNameTextW(lParam, out, cch))
// 		swprintf_s(out, cch, L"VK 0x%02X", kb.VKey); // 실패 시 VK 코드로 대체
// }


bool InputManager::Initialize(HWND hwnd)
{
#if _USE_RAWINPUTDEVICE
	RAWINPUTDEVICE rid[2] = {};

	// 키보드
	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x06; // Keyboard
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = hwnd;

	// 마우스
	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x02; // Mouse
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = hwnd;

	if (!RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE))) return false;
#endif// _USE_RAWINPUTDEVICE
	return true;
}

bool InputManager::GetKeyPressed(UINT vk)
{
	bool p = m_keyEdge[vk].pressed;
	m_keyEdge[vk].pressed = false;

	return p;
}

bool InputManager::GetKeyDown(UINT vk) const
{
	return m_keyDown[vk];
}

bool InputManager::GetKeyReleased(UINT vk)
{
	bool r = m_keyEdge[vk].released;
	m_keyEdge[vk].released = false;

	return r;
}

bool InputManager::GetKeyUp(UINT vk) const
{
	return !m_keyDown[vk];
}

bool InputManager::OnHandleMessage(const MSG& msg)
{
	switch (msg.message)
	{
	case WM_INPUT:
	{
		HandleRawInput(msg.lParam);
	}
	break;

	case WM_KEYDOWN:
	{
		HandleMsgKeyDown(msg.wParam, msg.lParam);
	}
	break;

	case WM_KEYUP:
	{
		HandleMsgKeyUp(msg.wParam, msg.lParam);
	}
	break;
	case WM_MOUSEWHEEL:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	{
		HandleMsgMouse(msg);
	}
	break;

	default:
		return false; // Unhandled message
	}

	return true;
}

void InputManager::HandleMsgKeyDown(WPARAM wParam, LPARAM lParam)
{
	assert(wParam < 256);

	bool wasDown = (lParam & (1 << 30)) != 0;
	m_keyDown[wParam] = true;

	if (!wasDown)
	{
		m_keyEdge[wParam].pressed = true;   // 처음 눌렸을 때만 true
	}

	m_keyEdge[wParam].released = false; // 눌린 상태이므로 released = false

#ifdef _VERBOSE_KEYBOARD
	char buf[128];
	std::snprintf(buf, sizeof(buf),
		"KEY DOWN  VK=0x%02X  (wasDown=%d)\n",
		static_cast<unsigned int>(wParam), wasDown);

	std::cout << buf;
#endif
}

void InputManager::HandleMsgKeyUp(WPARAM wParam, LPARAM lParam)
{
	assert(wParam < 256);

	m_keyDown[wParam] = false; // 현재 키가 눌리지 않음을 기록
	m_keyEdge[wParam].released = true; // 키가 떼어졌음을 기록

#ifdef _VERBOSE_KEYBOARD
	char buf[64];
	std::snprintf(buf, sizeof(buf),
		"KEY UP    VK=0x%02X\n",
		static_cast<unsigned int>(wParam));

	std::cout << buf;
#endif
}

void InputManager::HandleMsgMouse(const MSG& msg)
{
	int x = GetXFromLParam(msg.lParam) - 960;
	int y = GetYFromLParam(msg.lParam) * -1 + 540; // 나중에 화면 변경시 변경 되도록 변경

	m_CurMouse.pos = { x, y };

	if (msg.message == WM_LBUTTONDOWN)
	{
		m_CurMouse.leftPressed = true;
		SetCapture(msg.hwnd);
	}
	else if (msg.message == WM_RBUTTONDOWN)
	{
		m_CurMouse.rightPressed = true;
		SetCapture(msg.hwnd);
	}
	else if (msg.message == WM_LBUTTONUP)
	{
		m_CurMouse.leftPressed = false;
		ReleaseCapture();
	}
	else if (msg.message == WM_RBUTTONUP)
	{
		m_CurMouse.rightPressed = false;
		ReleaseCapture();
	}
	else if (msg.message == WM_MOUSEWHEEL)
	{
		short delta = GET_WHEEL_DELTA_WPARAM(msg.wParam); // +120 또는 -120 단위
		m_WheelDelta += delta; // 누적 저장 (프레임 기준)
	}

#ifdef _VERBOSE_MOUSE
	char buf[128];
	std::snprintf(buf, sizeof(buf),
		"MOUSE (%d,%d)  left=%d right=%d\n",
		x, y, m_CurMouse.leftPressed, m_CurMouse.rightPressed);

	std::cout << buf;
#endif
}

void InputManager::HandleRawInput(LPARAM lParam)
{
	UINT dwSize = 0;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

	if (dwSize == 0)
	{
		// 오류 처리: dwSize가 0인 경우는 잘못된 입력이거나 지원되지 않는 입력일 수 있음
		if (GetLastError() != ERROR_SUCCESS)
		{
			std::cerr << "GetRawInputData failed with error: " << GetLastError() << std::endl;
		}
		return;
	}

	std::unique_ptr<BYTE[]> lpb(new BYTE[dwSize]);

	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.get(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		std::cerr << "GetRawInputData failed to read data." << std::endl;
		return;
	}

	RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb.get());

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		HandleKeyboardInput(*raw);
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		HandleMouseInput(*raw);
	}
}

void InputManager::HandleKeyboardInput(RAWINPUT& raw)
{
	RAWKEYBOARD& kb = raw.data.keyboard;
	bool breakCode = (kb.Flags & RI_KEY_BREAK) != 0;   // 1 = KeyUp
	UINT vk = kb.VKey;

	if (vk >= 0xFF)  return;      // 0xFF = 잘못된 키 신호


	KeyEdge& edge = m_keyEdge[vk];
	bool& down = m_keyDown[vk];

	if (breakCode) // KeyUp
	{
		if (true == down) edge.released = true;
		down = false;

		std::cout << "Key Up: VK " << vk << "time" << std::endl;

	}
	else // KeyDown
	{
		if (false == down) edge.pressed = true;
		down = true;

		std::cout << "Key Down: VK " << vk << std::endl;
	}
}

void InputManager::HandleMouseInput(RAWINPUT& raw)
{

}

void InputManager::FrameEnd()
{
	// 프레임 종료 시점에 마우스 휠 델타 초기화
	m_WheelDelta = 0;

	// 추후 여기에 다른 프레임 종료 작업 추가 가능
}