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

	virtual void ButtonFunction() {}; // 버튼 기능을 구현하는 함수, 자식 클래스에서 오버라이드

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
			//-------------------------------- 일반 (set 1)
		case jokerDouble:   // 흑돌 2개 이상
			return [&bm]() { return bm.CountStones(Black) >= 2; };
		case jokerOmok:   // 흑돌 5개 이상
			return [&bm]() { return bm.HasStraightLine(Black,5); };
		case jokerSamok:   // 조커돌 4개 이상
			return [&bm]() { return bm.HasStraightLine(Joker, 4); };
		case jokerSammok:   // 조커돌 3개 이상
			return [&bm]() { return bm.HasStraightLine(Joker, 3); };

			//-------------------------------- 야생 (set 2)
		case jokerEgg:   // 흑돌 5개 이상
			return [&bm]() { return bm.CountStones(Black) >= 5; };

 		case jokerOstrichEgg:   // 항상 트루
			return [&bm]() { return true; };

		case jokerPeacock:   // 흰돌 5개 이상
			return [&bm]() { return bm.CountStones(White) >= 5; };

		case jokerEvolution:   // 야생 돌2개 이상
			return [&bm]() { return true; };

		case jokerDansu:   // 자유도가 1인 흰돌이 존재하는 경우
			return [&bm]() { return bm.WhiteLibOne(); };

			//-------------------------------- 우주 (set 3)
		case jokerTeleport:   // 
			return [&bm]() { return  bm.CountStones(Black) >= 1; };

		case jokerExplode:   // 착수 지점을 기준 3*3범위에 흑돌이 5개 이상
			return [&bm]() { return bm.HasBombReady(5); };

		case jokerMagnetic:   // 착수 지점을 기준 3*3범위에 흑돌 == 백돌
			return [&bm]() { return bm.IsSamaBlackWhite(); };

		case jokerBlackhole:   // 자유도가 0인 지점이 존재하는 경우
			return [&bm]() { return bm.IsLibZero(); };

			//-------------------------------- 단청 (set 4)
		case jokerFusion:   // 
			return [&bm]() { return  bm.CountStones(Black) >= 1; };

		case jokerTriunion:   // 착수 지점을 기준 3*3범위에 흑돌이 5개 이상
			return [&bm]() { return bm.HasBombReady(5); };

		case jokerQuadunion:   // 착수 지점을 기준 3*3범위에 흑돌 == 백돌
			return [&bm]() { return bm.IsSamaBlackWhite(); };


		default:
			return []() { return true; };
		}
	}

	// 조커를 눌렀을 때 희생모드? 
	// 이때 보드판을 클릭을해 >> 클릭 한곳이 블랙이야 >> 이친구 임시 그룹에 넣어두고 색깔 그레이스케일

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