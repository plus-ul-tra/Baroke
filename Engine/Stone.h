#pragma once
#include "Object.h"
#include "Transform.h"
#include "BitmapRender.h"

enum StoneType // 돌 종류
{
	White,
	Black,
	Joker
};
enum StoneAbility // 능력 혹은 이름
{
	None, // 흑돌, 백돌

	JokerEgg,
	JokerOstrichEgg,
	JokerPeacock,
	JokerCombination,
	JokerBite
};

class Stone : public Object
{
protected:
	Transform* m_transform {};
	BitmapRender3D* m_sprite {};

	POINT m_position = { -1, -1 }; // 돌 위치

public:
	Stone() = default;

	void Update(double) override {}

	POINT GetPosition() const { return m_position; } // 돌 위치 반환
};

class WhiteStone : public Stone
{
public:
	WhiteStone(POINT position, float size, int offset)
	{
		m_position = position;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>("White2.png", size- offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

class BlackStone : public Stone
{
public:
	BlackStone(POINT position, float size, int offset)
	{
		m_position = position;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>("Black2.png", size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

struct JokerInfo // 조커 돌 정보
{
	string fileName = "JokerStone1.png"; // 조커 돌 이미지 파일 이름

	int cost = 0; // 조커 돌 능력 사용 비용(흑돌)
	int returnCost = 0; // 조커 돌 능력 사용 후 반환 비용(흑돌)
	int cooldown = 0; // 조커 돌 능력 쿨타임
};
extern unordered_map<StoneAbility, JokerInfo> m_jokerInfoMap; // 조커 돌 능력 정보 맵 // 능력의 기본값 저장용

class JokerStone : public Stone
{
	JokerInfo m_jokerInfo; // 조커 돌 능력 정보

public:
	JokerStone(POINT position, float size, int offset, StoneAbility ability)
	{
		m_jokerInfo = m_jokerInfoMap[ability]; // 조커 돌 능력 정보 가져오기(복사)
		m_position = position;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>(m_jokerInfo.fileName.c_str(), size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};