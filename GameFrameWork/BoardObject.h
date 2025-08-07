#pragma once
#include "Engine.h"

class BoardObject : public Object
{
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
	int m_cell;
	int m_stoneOffset;
	BoardManager& m_boardManager = BoardManager::GetInstance(); // 싱글톤 보드 매니저
	Board m_board;
	std::vector<Stone*> m_stones;

	int currentBoardType = -1;
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
				if (bmp->IsActive()) bmp->Render(r); // 돌그리기

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
	int IsBoardChanged();
};

struct BoardType
{
	string textureKey = "Original.png";
	float changeDuration = 0.0f;
	XMFLOAT4 backgroundColor = XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f);
	string effectKey = "Leaf6.png"; // 화면 효과 이미지 키
};

inline unordered_map<int, BoardType> boardTypes =
{
	{0, {"Forest.png",		3.0f, XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f)}},
	{1, {"Space.png",		3.0f, XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f)}},
	{2, {"Korea.png",		3.0f, XMFLOAT4(0.9f, 0.8f, 0.6f, 1.0f)}},
	{3, {"Halloween.png",	3.0f, XMFLOAT4(0.2f, 0.5f, 0.8f, 1.0f)}},
	{4, {"Cyberpunk.png",	3.0f, XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f)}}
};

inline void BoardObject::BoardSync()
{
	m_stones.clear();

	int stoneTypeAmount[5] = { 0, 0, 0, 0, 0 };

	m_board = m_boardManager.GetBoard();

	int boardSize = m_boardManager.GetBoardSize();

	for (int r = 0; r < boardSize; ++r)
	{
		for (int c = 0; c < boardSize; ++c)
		{
			if (m_board[r][c])
			{
				auto& stone = m_board[r][c];
				m_stones.emplace_back(stone.get());

				if (stone->m_jokerType != JokerType::Default) stoneTypeAmount[stone->m_jokerType]++;
			}
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		if (currentBoardType == i) continue;

		if (stoneTypeAmount[i] >= 5)
		{
			currentBoardType = i;
			BoardType boardType = boardTypes[i];

			m_bitmapRender->ChangeBoardTexture(boardType.textureKey, boardType.changeDuration);
			m_bitmapRender->ChangeBackGroundColor(boardType.backgroundColor);

			m_screenEffectObjects.clear();
			CreateObject::CreateObjectsOutOfScreen(m_screenEffectObjects, boardType.effectKey, 1920.0f, 1080.0f, 200.0f, 50, 100.0f);
			m_isBoardChanged = true;

			break;
		}
	}
}

inline int BoardObject::IsBoardChanged()
{
	if (m_isBoardChanged)
	{
		m_isBoardChanged = false;
		return currentBoardType;
	}
	return -1;
}