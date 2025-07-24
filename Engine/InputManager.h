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
	static InputManager& GetInstance() // ��ü �������� InputManater::GetInstance().~~�� �Լ� ��밡��
	{
		static InputManager instance;

		return instance;
	}

	bool Initialize(HWND hwnd);				//���� RawInputDevice ���� �ʿ�

	bool OnHandleMessage(const MSG& msg);	//������ �޽��� �޾Ƽ� ó���ϴ� ��

	bool GetKeyPressed(UINT vk);			//Ű �ٿ� �Է� ó�� 1ȸ
	bool GetKeyDown(UINT vk) const;			//Ű �ٿ� �Է� ó�� ����

	bool GetKeyReleased(UINT vk);			//Ű �� �Է� ó�� 1ȸ		
	bool GetKeyUp(UINT vk) const;			//Ű �� �Է� ó�� ����

	int GetWheelDelta() const { return m_WheelDelta; }		// ���콺 �� ��Ÿ �� ��������

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

	std::array<bool, 256>    m_keyDown = {};   // ���� Down ����
	std::array<KeyEdge, 256> m_keyEdge = {};   // �̹� ������ Edge ���

private:
	int m_WheelDelta = 0;

	InputManager() = default;
	~InputManager() = default;

	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;
};

