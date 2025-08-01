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

public:
	BoardObject(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset = 0,int padding = 0)
	{
		m_cell = _cell;
		m_stoneOffset = _stoneOffset;
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(offX, offY, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_bitmapRender = AddComponent<BitmapRender3D>("test.png", drawW, drawH);
		m_bitmapRender->SetOrder(0);
		m_bitmapRender->SetActive(true);

		m_boardManager.Initialize(offX, offY, drawW, drawH, _cell, m_stoneOffset, padding); // 보드 매니저 초기화
	}
	// 이건 어디서 호출?
	void Render(Renderer& r) override
	{	// 보드 shader는 따로 설정 렌더 호출을 분리해놓음;;
		//if (m_bitmapRender) m_bitmapRender->Render(r, "NoiseShader"); // 보드가 자기 그림

		// 바둑돌 shader는 따로 설정 렌더 호출을 분리해놓음;;
		for (auto& sp : m_stones)
			if (auto* bmp = sp->GetComponent<BitmapRender3D>())
				if (bmp->IsActive()) bmp->Render(r); // 돌그리기

	}

	void Update(double) override {}

	void BoardSync();
};

inline void BoardObject::BoardSync()
{
	m_stones.clear();

	m_board = m_boardManager.GetBoard();

	int boardSize = m_boardManager.GetBoardSize();

	for (int r = 0; r < boardSize; ++r)
	{
		for (int c = 0; c < boardSize; ++c)
		{
			if (m_board[r][c])
			{
				auto stone = m_board[r][c];
				m_stones.emplace_back(stone.get());
			}
		}
	}
}





