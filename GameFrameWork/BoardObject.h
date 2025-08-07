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

	bool m_isTextureChanged = false; // 텍스처 변경 여부

public:
	BoardObject(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset = 0,int padding = 0)
	{
		m_cell = _cell;
		m_stoneOffset = _stoneOffset;
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(offX, offY, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);
		
		m_bitmapRender = AddComponent<BitmapRender3D>("Cyberpunk.png", drawW, drawH);
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
	}

	void BoardSync();
	
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
	
	if (!m_isTextureChanged)
	{
		for (int jokerType : stoneTypeAmount)
		{
			if (jokerType >= 5)
			{
				m_isTextureChanged = true;
				
				//ChangeThema 같은 함수로 묶을 것임
				m_bitmapRender->ChangeTexture("Forest.png", 4.0f);
				m_bitmapRender->ChangeBackGroundColor(XMFLOAT4(0.2f, 0.9f, 0.2f, 1.0f));
				// 오브젝트 등장

				//해줘 나뭇잎
				break;
			}

		}
	}
}





