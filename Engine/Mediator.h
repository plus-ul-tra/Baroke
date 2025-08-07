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

public:  
	void Initialize() {};
	void SetPosition(POINT pos) { m_position = pos; }
	POINT GetPosition() const { return m_position;  }
	void Reset() { m_position = { 0, 0 }; } // ��ġ �ʱ�ȭ
	void SetBackGroundColor(XMFLOAT4 prev, XMFLOAT4 target) { m_prevColor = prev, m_targetColor = target; }
	void SetPrevColor(XMFLOAT4 prev) { m_prevColor = prev; }
	void SetTargetColor(XMFLOAT4 target) { m_targetColor = target; }
	XMFLOAT4 GetPrevColor() const { return m_prevColor; }
	XMFLOAT4 GetTargetColor() const { return m_targetColor; }
};