#pragma once  
#include "Singleton.h"  
#include <windows.h> // 추가된 헤더 파일

// 불가피한 값 교환 시 결합도를 그나마 줄이기 위한 방책  
class Mediator : public Singleton<Mediator>  
{  
   friend class Singleton<Mediator>;  

private:  
   Mediator() = default;  
   ~Mediator() = default;  

   POINT position; // POINT는 windows.h에 정의되어 있음

public:  
	void Initialize() {};
	void SetPosition(POINT pos) { position = pos; }
	POINT GetPosition() const { return position;  }
	POINT GetPositionUV{
	
	};
	void Reset() { position = { 0, 0 }; } // 위치 초기화
};