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
	unique_ptr<Object> m_textObject = nullptr;

	float m_width = 0;
	float m_height = 0;

	bool m_isActive = true;
	bool m_isPressed = false;
	bool m_isPressedRight = false; // 우클릭 여부
	bool m_isHovered = false;

	MouseType m_inputType = MouseType::Move;
	BoardManager& m_boardManager = BoardManager::GetInstance();

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
	void SetIsActive(bool active) { m_isActive = active; m_isPressed = false; }

	void CheckInput(const MouseEvent& mouseEvent);

	void BindEnabledPredicate(std::function<bool()> pred)
	{
		m_isEnabledPredicate = std::move(pred);
	}

	
	void Update(double dt) override
	{
		ButtonFunction();
		Object::Update(dt);
		if (m_isActive && m_isHovered && m_textObject) m_textObject->Update(dt);
	}
	void Render(Renderer& renderer) override;
};

class JokerButton : public Button
{
	BitmapRender3D* m_tooltipRender = nullptr;
	StoneType m_stoneType = StoneType::Joker;
	StoneAbility m_jokerAbility = StoneAbility::None;

	static std::function<bool()> BuildPredicate(StoneAbility ability)
	{
		BoardManager& bm = BoardManager::GetInstance();

		switch (ability)
		{
		case StoneAbility::None:   // 능력 없음
			return []() { return false; };
			//-------------------------------- 일반 (set 1)
// 		case jokerDouble:   // 흑돌 2개 이상
// 			return [&bm]() { return bm.CountStones(Black) >= 2; };
		case jokerOmok:   // 흑돌 5개 이상
			return [&bm]() { return bm.HasStraightLine(Black,5)/*&& bm.CountStones(Black) >= 10*/; };
		case jokerSamok:   // 조커돌 4개 이상
			return [&bm]() { return bm.HasStraightLine(Joker, 4); };
		case jokerSammok:   // 조커돌 3개 이상
			return [&bm]() { return bm.HasStraightLine(Joker, 3); };

			//-------------------------------- 야생 (set 2)
		case jokerEgg:   // 흑돌 5개 이상
			return [&bm]() { return bm.CountStones(Black) >= 5; };

 		case jokerOstrichEgg:   // 항상 트루
			return [&bm]() { return true; }; // 진화

		case jokerPeacock:   // 흰돌 5개 이상
			return [&bm]() { return bm.CountStones(White) >= 5; };

		case jokerEvolution:   // 야생 돌2개 이상
			return [&bm]() { return true; };

		case jokerDansu:   // 자유도가 1인 흰돌이 존재하는 경우
			return [&bm]() { return /*bm.WhiteLibOne();*/bm.CountStones(Black) >= 2; };

			//-------------------------------- 우주 (set 3)
		case jokerTeleport:   // 흑돌 1개 이상
			return [&bm]() { return  bm.CountStones(Black) >= 1; };

		case jokerExplode:   // 착수 지점을 기준 3*3범위에 흑돌이 5개 이상
			return [&bm]() { return bm.IsColorCount(StoneType::Black,5); };

		case jokerMagnetic:   // 착수 지점을 기준 3*3범위에 흑돌 == 백돌
			return [&bm]() { return bm.IsSamaBlackWhite(); };

		case jokerBlackhole:   // 자유도가 0인 지점이 존재하는 경우
			return [&bm]() { return bm.IsLibZero(); };

			//-------------------------------- 단청 (set 4)
		case jokerFusion:   // 흰 돌 2개를 연결할 수 있는 지점이 있는 경우
			return [&bm]() { return  bm.IsConnectTwo(); };

		case jokerTriunion:   // 항상 트루
			return [&bm]() { return true; }; // 진화

		case jokerQuadunion:   // 항상 트루
			return [&bm]() { return true; }; // 진화

			//-------------------------------- 할로윈 (set 6)
		case jokerSplit:   // 흑돌 2개 이상
			return [&bm]() { return bm.CountStones(Black) >= 2; };

		case jokerWaxseal:   // 자유도가 1인 흰돌이 존재하는 경우
			return [&bm]() { return bm.WhiteLibOne(); };

		case jokerFlip:   // 착수 지점을 기준 3*3범위에 흰돌이 3개 이상
			return [&bm]() {  return bm.IsColorCount(StoneType::White, 3); };

		case jokerOthello:   // 흑돌 기준 상하좌우 중 백돌3개 이상인 지점이 있는가?
			return [&bm]() { return bm.IsOthello(); };

		case jokerMrchan:   // 항상 트루
			return [&bm]() { return true; }; // 진화

			//-------------------------------- 자연 (set 7)
		case jokerShadow:   // 흰돌 1개 이상
			return [&bm]() { return bm.CountStones(White) >= 1; };

		case jokerLight:   // 흑돌 0개
			return [&bm]() { return bm.CountStones(Black) == 0; };

		case jokerTime:   // 플레이어 흑돌 개수가 5개 미만인 경우
			return [&bm]() {  return bm.m_playerInfo.GetBlackCount() < 5; };

		case jokerWind:   // 흑돌 4개 이상
			return [&bm]() { return bm.CountStones(Black) >= 4; };
		default:
			return []() { return true; };
		}
	}

	void ButtonFunction() override;

public:
	JokerButton(float posX, float posY, float width, float height, const std::string& bitmapFile, int order = 0)
		: Button(posX, posY, width, height, bitmapFile, order) {}


	void SetButtonJoker(StoneType stoneType, StoneAbility ability);
	StoneAbility GetJokerAbility() const { return m_jokerAbility; }
	void Update(double dt) override
	{
		ButtonFunction();
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
		Object::Update(dt);
	}
};

class ShopJokerButton : public Button
{
	BitmapRender3D* m_tooltipRender = nullptr;
	JokerStoneInfo m_jokerInfo;
	StoneAbility m_jokerAbility = StoneAbility::None;
	vector<unique_ptr<JokerButton>>* m_jokerButton = nullptr; // 상점 조커 버튼들

	void ButtonFunction() override;

public:
	ShopJokerButton(float posX, float posY, float width, float height, const std::string& bitmapFile, int order = 0)
		: Button(posX, posY, width, height, bitmapFile, order) {}

	void SetButtonJoker(JokerStoneInfo jokerInfo, StoneAbility ability);
	void SetButton(vector<unique_ptr<JokerButton>>* jokerButton) { m_jokerButton = jokerButton; }

	void SetShowAndActive(bool active);
	StoneAbility GetJokerAbility() const { return m_jokerAbility; }
};

class ShopEndButton : public Button
{
	bool m_isEndButtonPressed = false;

	public:
	ShopEndButton(float posX, float posY, float width, float height, const std::string& bitmapFile, int order = 0)
		: Button(posX, posY, width, height, bitmapFile, order) {}

	bool IsEndButtonPressed();

	void SetShowAndActive(bool active);
	void ButtonFunction() override;
};