#pragma once
#include "Engine.h"
#include "StoneObject.h"

unordered_map<string, function<void(JokerInfo&)>> jokerFunctions =
{
	{"JokerStone1Function", [](JokerInfo& info) { cout << "Function: " << info.functionName << ", Description: " << info.description << endl; }},
	{"JokerStone2Function", [](JokerInfo& info) { cout << "Function: " << info.functionName << ", Description: " << info.description << endl; }}
};

class Joker : public StoneObject
{
	JokerInfo jokerInfo;

public:
	Joker(const JokerInfo& joker, float posX, float posY, float size) : jokerInfo(joker)
	{
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(posX - size / 2, posY - size / 2, 0.0f, 1.0f));
		std::cout << "위치" << posX << " " << posY << ", 사이즈 :" << size << std::endl;
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender>(jokerInfo.fileName, size, size);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);

		jokerFunctions[jokerInfo.functionName](jokerInfo); // 함수 호출
	}
};