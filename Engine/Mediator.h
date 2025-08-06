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
   XMFLOAT3 m_prevColor =	{ 0.5f,0.5f,0.5f };
   XMFLOAT3 m_targetColor = { 0.5f,0.5f,0.5f };

public:  
	void Initialize() {};
	void SetPosition(POINT pos) { m_position = pos; }
	POINT GetPosition() const { return m_position;  }
	void Reset() { m_position = { 0, 0 }; } // ��ġ �ʱ�ȭ
	void SetBackGroundColor(XMFLOAT3 prev, XMFLOAT3 target) { m_prevColor = prev, m_targetColor = target; }
	XMFLOAT3 GetPrevColor() const { return m_prevColor; }
	XMFLOAT3 GetTargetColor() const { return m_targetColor; }
};