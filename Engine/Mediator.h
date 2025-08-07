#pragma once  
#include "Singleton.h"  
#include <windows.h> // 추가된 헤더 파일
#include <DirectXMath.h>
// 불가피한 값 교환 시 결합도를 그나마 줄이기 위한 방책  
using namespace DirectX;
class Mediator : public Singleton<Mediator>  
{  
   friend class Singleton<Mediator>;  

private:  
   Mediator() = default;  
   ~Mediator() = default;  

   POINT m_position; // POINT는 windows.h에 정의되어 있음
   XMFLOAT4 m_prevColor =	{ 0.0f,1.0f,1.0f,1.0f};
   XMFLOAT4 m_targetColor = { 0.0f,1.0f,1.0f,1.0f }; //default gray

public:  
	void Initialize() {};
	void SetPosition(POINT pos) { m_position = pos; }
	POINT GetPosition() const { return m_position;  }
	void Reset() { m_position = { 0, 0 }; } // 위치 초기화
	void SetBackGroundColor(XMFLOAT4 prev, XMFLOAT4 target) { m_prevColor = prev, m_targetColor = target; }
	void SetPrevColor(XMFLOAT4 prev) { m_prevColor = prev; }
	void SetTargetColor(XMFLOAT4 target) { m_targetColor = target; }
	XMFLOAT4 GetPrevColor() const { return m_prevColor; }
	XMFLOAT4 GetTargetColor() const { return m_targetColor; }
};