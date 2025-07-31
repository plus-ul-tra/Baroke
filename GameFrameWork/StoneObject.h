//// StoneObject.h
//#pragma once
//#include "Engine.h"
//
//class StoneObject : public Object
//{
//public:
//	StoneObject() = default;
//	StoneObject(StoneColor color, float posX, float posY, float size)
//	{
//		m_transform = AddComponent<Transform>();
//		m_sprite = AddComponent<BitmapRender>(
//			color == StoneColor::Black
//			? "Black_Stone.png"
//			: "White_Stone.png",
//			size, size);
//
//		m_transform->SetPosition(XMVectorSet(posX - size / 2 , posY - size / 2, 0.0f, 1.0f));
//		std::cout <<"위치"<< posX << " " << posY <<", 사이즈 :"<<size<< std::endl;
//		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
//		m_transform->SetRotation(0.0f);
//
//		m_sprite->SetOrder(1);
//		m_sprite->SetActive(true); // 렌더링 활성화
//	}
//	void                SetTagPos(int r, int c) { m_rc = { r,c }; }
//	std::pair<int, int> GetTagPos() const { return m_rc; }
//
//	void Update(double) override {}          
//protected:
//	Transform* m_transform{};
//	BitmapRender* m_sprite{};
//	std::pair<int, int> m_rc{ -1,-1 };
//};
//
