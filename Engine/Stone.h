#pragma once
#include "Object.h"
#include "Transform.h"
#include "BitmapRender.h"

enum StoneType // �� ����
{
	White,
	Black,
	Joker
};
enum StoneAbility // �ɷ� Ȥ�� �̸�
{
	None, // �浹, �鵹

	JokerAbility1,
	JokerAbility2,
	JokerAbility3
};

class Stone : public Object
{
protected:
	Transform* m_transform {};
	BitmapRender* m_sprite {};

	POINT m_position = { -1, -1 }; // �� ��ġ

public:
	Stone() = default;

	void Update(double) override {}

	POINT GetPosition() const { return m_position; } // �� ��ġ ��ȯ
};

class WhiteStone : public Stone
{
public:
	WhiteStone(POINT position, float size)
	{
		m_position = position;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) - size / 2, static_cast<float>(position.y) - size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender>("Sample.png", size, size);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

class BlackStone : public Stone
{
public:
	BlackStone(POINT position, float size)
	{
		m_position = position;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) - size / 2, static_cast<float>(position.y) - size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender>("Sample.png", size, size);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

struct JokerInfo // ��Ŀ �� ����
{
	string fileName = "Sample.png"; // ��Ŀ �� �̹��� ���� �̸�
	string description = "Sample Description"; // ��Ŀ �� �ɷ� ����
	int cost = 0; // ��Ŀ �� �ɷ� ��� ���(�浹)
	int returnCost = 0; // ��Ŀ �� �ɷ� ��� �� ��ȯ ���(�浹)
	int cooldown = 0; // ��Ŀ �� �ɷ� ��Ÿ��
};
extern unordered_map<StoneAbility, JokerInfo> m_jokerInfoMap; // ��Ŀ �� �ɷ� ���� �� // �ɷ��� �⺻�� �����

class JokerStone : public Stone
{
	JokerInfo m_jokerInfo; // ��Ŀ �� �ɷ� ����

public:
	JokerStone(POINT position, float size, StoneAbility ability)
	{
		m_jokerInfo = m_jokerInfoMap[ability]; // ��Ŀ �� �ɷ� ���� ��������(����)
		m_position = position;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) - size / 2, static_cast<float>(position.y) - size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender>(m_jokerInfo.fileName.c_str(), size, size);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};