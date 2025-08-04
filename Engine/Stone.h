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

	JokerEvolve,
	JokerEgg,
	JokerOstrichEgg,
	JokerBite,
	JokerPeacock
};

class Stone : public Object
{
protected:
	Transform* m_transform {};
	BitmapRender3D* m_sprite {};

	POINT m_position = { -1, -1 }; // �� ��ġ
	float m_size = 0; // �� ũ��
	float m_offset = 0; // �� ����

	double m_lerpTime = 0.0; // �̵� ���� �ð�
	double m_lerpElapsedTime = 0.0; // ��� �ð�
	XMVECTOR m_lerpStartPosition = XMVectorZero(); // �̵� ���� ��ġ
	XMVECTOR m_lerpEndPosition = XMVectorZero(); // �̵� �� ��ġ

public:
	Stone() = default;

	void Update(double deltaTime) override; // �� ������Ʈ �Լ�
	void Move(POINT position, double duration = 1.0); // �� �̵� �Լ�

	POINT GetPosition() const { return m_position; } // �� ��ġ ��ȯ
	void SetPosition(POINT position) { m_position = position; } // �� ��ġ ����
};

class WhiteStone : public Stone
{
public:
	WhiteStone(POINT position, float size, int offset)
	{
		m_position = position;
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>("White.png", size - offset, size - offset);
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
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>("Black.png", size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}
};

struct JokerInfo // ��Ŀ �� ����
{
	string fileName = "JokerEgg.png"; // ��Ŀ �� �̹��� ���� �̸�

	int blackReturn = 0; // ��Ŀ �� �ɷ� ��� �� ��ȯ ���(�浹)

	int coolTime = 0; // ��Ŀ �� �ɷ� ��Ÿ��
	int lifeSpan = 0; // ��Ŀ �� �ɷ� ���� �ð�

	int functionDuration = 0; // ��Ŀ �� �ɷ� �Լ� ���� �ð� // 0�̸� �ѹ��� ����
	int functionVariable = 0; // ��Ŀ �� �ɷ� �Լ��� ���Ǵ� ���� // �ɷ¿� ���� �ٸ�
};
extern unordered_map<StoneAbility, JokerInfo> m_jokerInfoMap; // ��Ŀ �� �ɷ� ���� �� // �ɷ��� �⺻�� �����

class JokerStone : public Stone
{
public:
	JokerStone(POINT position, float size, int offset, StoneAbility ability)
	{
		m_jokerInfo = m_jokerInfoMap[ability]; // ��Ŀ �� �ɷ� ���� ��������(�� ����)
		m_position = position;
		m_size = size;
		m_offset = offset;

		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(static_cast<float>(position.x) + size / 2, static_cast<float>(position.y) + size / 2, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_sprite = AddComponent<BitmapRender3D>(m_jokerInfo.fileName.c_str(), size - offset, size - offset);
		m_sprite->SetOrder(1);
		m_sprite->SetActive(true);
	}

	JokerInfo m_jokerInfo; // ��Ŀ �� �ɷ� ����
};