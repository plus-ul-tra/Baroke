#pragma once  
#include "Singleton.h"  
#include <windows.h> // �߰��� ��� ����
#include <DirectXMath.h>
// �Ұ����� �� ��ȯ �� ���յ��� �׳��� ���̱� ���� ��å  
using namespace DirectX;
class Mediator : public Singleton<Mediator>  
{  
   friend class Singleton<Mediator>;  

private:  
   Mediator() = default;  
   ~Mediator() = default;  

   POINT m_position; // POINT�� windows.h�� ���ǵǾ� ����
   XMFLOAT4 m_prevColor =	{ 0.0f,1.0f,1.0f,1.0f};
   XMFLOAT4 m_targetColor = { 0.0f,1.0f,1.0f,1.0f }; //default gray
   XMFLOAT4 m_UIColor = {0.0f,0.0f,0.0f,0.0f};
   int      m_selectedIndex = -1;
public:  
	void Initialize() {};
	void SetPosition(POINT pos) { m_position = pos; }
	POINT GetPosition() const { return m_position;  }
	void Reset() { m_position = { 0, 0 }; } // ��ġ �ʱ�ȭ
	void SetBackGroundColor(XMFLOAT4 prev, XMFLOAT4 target) { m_prevColor = prev, m_targetColor = target; }

	void SetPrevColor(XMFLOAT4 prev) { m_prevColor = prev; }
	void SetTargetColor(XMFLOAT4 target) { m_targetColor = target; }

	void SetSlotIndex(int index) { /*std::cout << "Set Index" << index << std::endl;*/ m_selectedIndex = index; }
	int GetSlotIndex() const { return m_selectedIndex; }

	XMFLOAT4 GetBackPrevColor() const { return m_prevColor; }
	XMFLOAT4 GetBackTargetColor() const { return m_targetColor; }

	void SetUIColor(XMFLOAT4 color) { m_UIColor = color; }
	XMFLOAT4 GetUIColor() const { return m_UIColor; }
};