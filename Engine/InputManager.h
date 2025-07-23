#pragma once

//#define _VERBOSE_MOUSE
//#define _VERBOSE_KEYBOARD

inline int GetXFromLParam(LPARAM lp) { return (int)(short)(LOWORD(lp)); }
inline int GetYFromLParam(LPARAM lp) { return (int)(short)(HIWORD(lp)); }

struct MouseState
{
	POINT  pos{ 0, 0 };

	bool   leftPressed{ false };
	bool   rightPressed{ false };
};

static bool IsMouseMove(const MouseState& prev, const MouseState& cur)
{
	return (prev.pos.x != cur.pos.x || prev.pos.y != cur.pos.y);
}

struct KeyEdge
{
	bool pressed = false;
	bool released = false;
};

struct InputContext
{
	std::unordered_map<int, bool> keyPressed;
	MouseState mouse;
	int wheelDelta = 0;
};

class InputManager
{
public:
	static InputManager& GetInstance() // 객체 가져오기 InputManater::GetInstance().~~로 함수 사용가능
	{
		static InputManager instance;

		return instance;
	}

	bool Initialize(HWND hwnd);				//추후 RawInputDevice 사용시 필요

	bool OnHandleMessage(const MSG& msg);	//윈도우 메시지 받아서 처리하는 곳

	bool GetKeyPressed(UINT vk);			//키 다운 입력 처리 1회
	bool GetKeyDown(UINT vk) const;			//키 다운 입력 처리 지속

	bool GetKeyReleased(UINT vk);			//키 업 입력 처리 1회		
	bool GetKeyUp(UINT vk) const;			//키 업 입력 처리 지속

	int GetWheelDelta() const { return m_WheelDelta; }		// 마우스 휠 델타 값 가져오기

	void FrameEnd();

	MouseState GetMouseState() const { return m_CurMouse; }

protected:

	void HandleMsgKeyDown(WPARAM wParam, LPARAM lParam);

	void HandleMsgKeyUp(WPARAM wParam, LPARAM lParam);

	void HandleMsgMouse(const MSG& msg);

	void HandleRawInput(LPARAM lParam);

	void HandleKeyboardInput(RAWINPUT& raw);

	void HandleMouseInput(RAWINPUT& raw);

	MouseState m_CurMouse;

	std::array<bool, 256>    m_keyDown = {};   // 현재 Down 상태
	std::array<KeyEdge, 256> m_keyEdge = {};   // 이번 프레임 Edge 결과

private:
	int m_WheelDelta = 0;

	InputManager() = default;
	~InputManager() = default;

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
};

