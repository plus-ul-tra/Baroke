#pragma once
#include "Engine.h"

struct EffectType
{
	string effectKey = "Leaf6.png";
	float width = 200.0f;
	float height = 200.0f;
	int amount = 50;
	float speed = 100.0f;
	CreateObject::direction exclusiveDirection = CreateObject::all;
	float rotation = 0.0f;
};

inline unordered_map<int, EffectType> effectTypes =
{
	{-1, {"Sample.png", 0.0f, 0.0f, 0, 0.0f, CreateObject::all}},

	// 숲
	{0, {"Leaf6.png", 100.0f, 200.0f, 10, 100.0f, CreateObject::left, 0.5f}},
	{1, {"Leaf11.png", 100.0f, 200.0f, 10, 100.0f, CreateObject::right, 0.5f}},
	{2, {"Leaf1.png", 100.0f, 200.0f, 20, 100.0f, CreateObject::up, 0.5f}},
	{3, {"Leaf8.png", 200.0f, 200.0f, 20, 100.0f, CreateObject::down, 0.2f}},

	// 우주
	{4, {"Leaf12.png", 200.0f, 200.0f, 20, 100.0f, CreateObject::left}},
	{5, {"Leaf13.png", 200.0f, 200.0f, 20, 100.0f, CreateObject::right}},
	{6, {"Leaf14.png", 200.0f, 200.0f, 20, 100.0f, CreateObject::up}},
	{7, {"Leaf15.png", 200.0f, 200.0f, 20, 100.0f, CreateObject::down}},

	// 단청
	{8, {"Mountain1.png", 200.0f, 200.0f, 5, 100.0f, CreateObject::down}},
	{9, {"Mountain2.png", 200.0f, 300.0f, 5, 100.0f, CreateObject::down}},

	// 할로윈
	{10, {"SmallNet.png", 250.0f, 200.0f, 10, 100.0f, CreateObject::up}},
	{11, {"gravestone3.png", 200.0f, 300.0f, 5, 100.0f, CreateObject::down}},
	{12, {"Pumpkin1.png", 250.0f, 200.0f, 5, 100.0f, CreateObject::down}},
	{13, {"Pumpkin2.png", 250.0f, 200.0f, 5, 100.0f, CreateObject::down}},
};

struct BoardType
{
	string textureKey = "Normal.png";
	float changeDuration = 0.0f;
	XMFLOAT4 backgroundColor = XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f);
	EffectType effects[4] = { effectTypes[-1], effectTypes[-1], effectTypes[-1], effectTypes[-1] };
};

inline unordered_map<int, BoardType> boardTypes =
{
	{0, {"Normal.png",						3.0f, XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)}},
	{1, {"Forest.png",						3.0f, XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f), {effectTypes[0], effectTypes[1], effectTypes[2], effectTypes[3]}}},
	{2, {"Space.png",						3.0f, XMFLOAT4(0.4f, 0.02f, 0.6f, 1.0f), {effectTypes[0], effectTypes[1], effectTypes[2], effectTypes[3]}}},
	{3, {"T_Dancheong_Main_Glow.png",		3.0f, XMFLOAT4(0.9f, 0.8f, 0.6f, 1.0f), {effectTypes[8], effectTypes[9], effectTypes[-1], effectTypes[-1]}}},
	{4, {"T_Halloween_Main_Glow.png",		3.0f, XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f), {effectTypes[10], effectTypes[11], effectTypes[12], effectTypes[13]}}},
	{5, {"Cyberpunk.png",					3.0f, XMFLOAT4(0.3f, 0.3f, 0.9f, 1.0f), {effectTypes[-1], effectTypes[-1], effectTypes[-1], effectTypes[-1]}}}
};

class BoardObject : public Object
{
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
	int m_cell;
	int m_stoneOffset;
	BoardManager& m_boardManager = BoardManager::GetInstance(); // 싱글톤 보드 매니저
	Board m_board;
	std::vector<Stone*> m_stones;

	bool m_isBoardChanged = false; // 보드 타입이 변경되었는지 여부
	vector<unique_ptr<Object>> m_screenEffectObjects; // 화면에 그려질 오브젝트들

public:
	BoardObject(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset = 0,int padding = 0)
	{
		m_cell = _cell;
		m_stoneOffset = _stoneOffset;
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(offX, offY, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);
		
		m_bitmapRender = AddComponent<BitmapRender3D>("Normal.png", drawW, drawH);
		m_bitmapRender->SetOrder(0);
		m_bitmapRender->SetActive(true);
		m_boardManager.Initialize(offX, offY, drawW, drawH, _cell, m_stoneOffset, padding); // 보드 매니저 초기화
	}
	//바둑돌 그리기
	void Render(Renderer& r) override
	{	
		for (auto& sp : m_stones)
			if (auto* bmp = sp->GetComponent<BitmapRender3D>())
			{
				//sp->Render(r); // 돌 이펙트 그리기
				if (bmp->IsActive()) bmp->Render(r); // 돌그리기
			}

		for (auto& obj : m_screenEffectObjects)
			if (auto* bmp = obj->GetComponent<BitmapRender3D>())
				if (bmp->IsActive()) bmp->Render(r); // 화면 효과 오브젝트 그리기

	}

	void Update(double deltaTime) override
	{
		const auto& sel = m_boardManager.GetSacrificeGroup();
		const auto& sel2 = m_boardManager.GetuseCondGroup();

		// 각 Stone에 대해 Shader 타입 설정
		for (auto& stone : m_stones)
		{
			if (!stone) continue; // nullptr 체크
			stone->Update(deltaTime);
			auto* bmp = stone->GetComponent<BitmapRender3D>();
			if (!bmp || !bmp->IsActive())
				continue;

			// 보드 좌표 얻기
			POINT mousepos = stone->GetPosition();
			POINT rcpos = m_boardManager.MouseToBoardPosition(mousepos);
			bool isSacrificed = std::find(sel.begin(), sel.end(), rcpos) != sel.end();
			bool isSelected = std::find(sel2.begin(), sel2.end(), rcpos) != sel2.end();
			if (isSacrificed)
				bmp->SetShaderType("SetRed");
  			else if (isSelected)
 				bmp->SetShaderType("SetGreen");
			else
 				bmp->SetShaderType("DefaultShader");   // 혹은 기존에 쓰던 기본 Shader 이름
		}

		for (auto& obj : m_screenEffectObjects)
		{
			obj->Update(deltaTime);
			auto* bmp = obj->GetComponent<BitmapRender3D>();
			if (bmp && bmp->IsActive()) bmp->Update(deltaTime);
		}
	}
	void BoardSync();
	void ChangeTheme(int theme);
	void ThemeFixedObjects(int theme, vector<unique_ptr<Object>>& objectsvector);
};

inline void BoardObject::BoardSync()
{
	m_stones.clear();

	int stoneTypeAmount[5] = { 0, 0, 0, 0, 0 };

	m_board = m_boardManager.GetBoard();
	vector<shared_ptr<Stone>> deathRow = m_boardManager.GetDeathRow();

	int boardSize = m_boardManager.GetBoardSize();

	for (int r = 0; r < boardSize; ++r)
	{
		for (int c = 0; c < boardSize; ++c)
		{
			if (m_board[r][c])
			{
				auto& stone = m_board[r][c];
				m_stones.emplace_back(stone.get());
			}
		}
	}
	for (const shared_ptr<Stone>& deathStone : deathRow) m_stones.emplace_back(deathStone.get());
}

inline void BoardObject::ChangeTheme(int theme)
{
	if (theme < 0 || theme >= boardTypes.size()) return;

	BoardType boardType = boardTypes[theme];

	m_bitmapRender->ChangeBoardTexture(boardType.textureKey, boardType.changeDuration);
	m_bitmapRender->ChangeBackGroundColor(boardType.backgroundColor);

	m_screenEffectObjects.clear();

	for (EffectType& effect : boardType.effects)
	{
		CreateObject::CreateObjectsOutOfScreen
		(
			m_screenEffectObjects,
			effect.effectKey,
			1920.0f, 1080.0f,
			effect.width, effect.height,
			effect.amount,
			effect.speed,
			effect.exclusiveDirection,
			effect.rotation
		);
	}
	ThemeFixedObjects(theme, m_screenEffectObjects);

	m_isBoardChanged = true;
}

inline void BoardObject::ThemeFixedObjects(int theme, vector<unique_ptr<Object>>& objectsvector)
{
	if (theme < 0 || theme >= boardTypes.size()) return;

	switch (theme)
	{

	case 2: // Space

		break;

	case 3: // Dancheong
		objectsvector.emplace_back(make_unique<NewObject>(-600.0f, 600.0f, 100.0f, 100.0f, 0.0f, "DancheongSun.png", 100.0f, XMVectorSet(-500.0f, 400.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(600.0f, 600.0f, 100.0f, 100.0f, 0.0f, "Moon.png", 100.0f, XMVectorSet(500.0f, 400.0f, 0.0f, 1.0f)));

		break;

	case 4: // Halloween
		objectsvector.emplace_back(make_unique<NewObject>(-1000.0f, 600.0f, 200.0f, 200.0f, 0.0f, "Spider.png", 100.0f, XMVectorSet(-900.0f, 500.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(1000.0f, -600.0f, 200.0f, 200.0f, 0.0f, "PumpkinCat.png", 100.0f, XMVectorSet(900.0f, -500.0f, 0.0f, 1.0f)));

		break;

	case 5: // Cyberpunk
		objectsvector.emplace_back(make_unique<NewObject>(0.0f, 600.0f, 2000.0f, 250.0f, 0.0f, "CyberAll.png", 100.0f, XMVectorSet(0.0f, 500.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(750.0f, -600.0f, 100.0f, 600.0f, 0.0f, "Building1.png", 100.0f, XMVectorSet(750.0f, -500.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(600.0f, -600.0f, 100.0f, 600.0f, 0.0f, "Building2.png", 100.0f, XMVectorSet(600.0f, -500.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(-850.0f, -600.0f, 300.0f, 600.0f, 0.0f, "Building4.png", 100.0f, XMVectorSet(-850.0f, -500.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(-660.0f, -600.0f, 100.0f, 600.0f, 0.0f, "Building3.png", 100.0f, XMVectorSet(-660.0f, -500.0f, 0.0f, 1.0f)));
		objectsvector.emplace_back(make_unique<NewObject>(-550.0f, -600.0f, 100.0f, 600.0f, 0.0f, "Building5.png", 100.0f, XMVectorSet(-550.0f, -500.0f, 0.0f, 1.0f)));
		break;

	default:
		break;
	}
}