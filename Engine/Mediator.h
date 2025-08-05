#pragma once  
#include "Singleton.h"  
#include <windows.h> // �߰��� ��� ����

// �Ұ����� �� ��ȯ �� ���յ��� �׳��� ���̱� ���� ��å  
class Mediator : public Singleton<Mediator>  
{  
   friend class Singleton<Mediator>;  

private:  
   Mediator() = default;  
   ~Mediator() = default;  

   POINT position; // POINT�� windows.h�� ���ǵǾ� ����

public:  
	void Initialize() {};
	void SetPosition(POINT pos) { position = pos; }
	POINT GetPosition() const { return position;  }
	POINT GetPositionUV{
	
	};
	void Reset() { position = { 0, 0 }; } // ��ġ �ʱ�ȭ
};