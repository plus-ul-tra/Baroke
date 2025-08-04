#pragma once
#include "Object.h"
#include "Transform.h"
#include "BitmapRender.h"
#include "InputManager.h"
#include "InputEvents.h"
#include "BoardManager.h"

class Button : public Object
{
protected:
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;

	float m_width = 0;
	float m_height = 0;

	bool m_isActive = true;
	bool m_isHovered = false;
	bool m_isPressed = false;

	MouseType m_inputType = MouseType::Move;

	virtual void ButtonFunction() {}; // ��ư ����� �����ϴ� �Լ�, �ڽ� Ŭ�������� �������̵�

	std::function<bool()> m_isEnabledPredicate = []() { return true; };

public:
	Button(float posX, float posY, float width, float height)
		: m_width(width), m_height(height) {
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(posX, posY, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);
	}

	Button(float posX, float posY, float width, float height, const std::string& bitmapFile, int order = 0)
		: m_width(width), m_height(height)
	{
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(posX, posY, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_bitmapRender = AddComponent<BitmapRender3D>(bitmapFile, width, height);
		m_bitmapRender->SetOrder(order);
		m_bitmapRender->SetActive(true);
		
	}


	XMVECTOR GetPosition() const { return m_transform->GetPosition(); }
	void SetPosition(XMVECTOR pos) { m_transform->SetPosition(pos); }

	float GetWidth() const { return m_width; }
	void SetWidth(float width) { m_width = width; }

	float GetHeight() const { return m_height; }
	void SetHeight(float height) { m_height = height; }

	bool GetIsActive() const { return m_isActive; }
	void SetIsActive(bool active) { m_isActive = active; }

	void CheckInput(const MouseEvent& mouseEvent);

	void BindEnabledPredicate(std::function<bool()> pred)
	{
		m_isEnabledPredicate = std::move(pred);
	}

	void Update(double dt) override
	{

		m_isActive = m_isEnabledPredicate();

		if (!m_isActive)
		{
			//std::cout << this << "UnDo" << std::endl;
			if (m_bitmapRender) m_bitmapRender->SetShaderType("GrayScale");
		}
		else 
		{
			if (m_bitmapRender) m_bitmapRender->SetShaderType("Holo");
		}
		ButtonFunction();
		Object::Update(dt);
	}
};

class JokerButton : public Button
{
	StoneType m_stoneType = StoneType::Joker;
	StoneAbility m_jokerAbility = StoneAbility::None;

	static std::function<bool()> BuildPredicate(StoneAbility ability)
	{
		BoardManager& bm = BoardManager::GetInstance();

		switch (ability)
		{
			//-------------------------------- �Ϲ� (set 1)
		case jokerDouble:   // �浹 2�� �̻�
			return [&bm]() { return bm.CountStones(Black) >= 2; };
		case jokerOmok:   // �浹 5�� �̻�
			return [&bm]() { return bm.HasStraightLine(Black,5); };
		case jokerSamok:   // ��Ŀ�� 4�� �̻�
			return [&bm]() { return bm.HasStraightLine(Joker, 4); };
		case jokerSammok:   // ��Ŀ�� 3�� �̻�
			return [&bm]() { return bm.HasStraightLine(Joker, 3); };

			//-------------------------------- �߻� (set 2)
		case jokerEgg:   // �浹 5�� �̻�
			return [&bm]() { return bm.CountStones(Black) >= 5; };

 		case jokerOstrichEgg:   // �׻� Ʈ��
			return [&bm]() { return true; };

		case jokerPeacock:   // �� 5�� �̻�
			return [&bm]() { return bm.CountStones(White) >= 5; };

		case jokerEvolution:   // �߻� ��2�� �̻�
			return [&bm]() { return true; };

		case jokerDansu:   // �������� 1�� ���� �����ϴ� ���
			return [&bm]() { return bm.WhiteLibOne(); };

			//-------------------------------- ���� (set 3)
		case jokerTeleport:   // 
			return [&bm]() { return  bm.CountStones(Black) >= 1; };

		case jokerExplode:   // ���� ������ ���� 3*3������ �浹�� 5�� �̻�
			return [&bm]() { return bm.HasBombReady(5); };

		case jokerMagnetic:   // ���� ������ ���� 3*3������ �浹 == �鵹
			return [&bm]() { return bm.IsSamaBlackWhite(); };

		case jokerBlackhole:   // �������� 0�� ������ �����ϴ� ���
			return [&bm]() { return bm.IsLibZero(); };

			//-------------------------------- ��û (set 4)
		case jokerFusion:   // 
			return [&bm]() { return  bm.CountStones(Black) >= 1; };

		case jokerTriunion:   // ���� ������ ���� 3*3������ �浹�� 5�� �̻�
			return [&bm]() { return bm.HasBombReady(5); };

		case jokerQuadunion:   // ���� ������ ���� 3*3������ �浹 == �鵹
			return [&bm]() { return bm.IsSamaBlackWhite(); };


		default:
			return []() { return true; };
		}
	}

	// ��Ŀ�� ������ �� ������? 
	// �̶� �������� Ŭ������ >> Ŭ�� �Ѱ��� ���̾� >> ��ģ�� �ӽ� �׷쿡 �־�ΰ� ���� �׷��̽�����

	void ButtonFunction() override;

public:
	JokerButton(float posX, float posY, float width, float height, const std::string& bitmapFile, int order = 0)
		: Button(posX, posY, width, height, bitmapFile, order) {}


	void SetButtonJoker(StoneType stoneType, StoneAbility ability) {
		m_jokerAbility = ability;
		m_stoneType = stoneType;        
		BindEnabledPredicate(BuildPredicate(ability));
	}

};