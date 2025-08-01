#pragma once
#include "Engine.h"

class BoardObject : public Object
{
	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
	int m_cell;
	int m_stoneOffset;
	Board m_board;
	std::vector<std::unique_ptr<Stone>> m_stones;

public:
	BoardObject(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset = 0,int padding = 0)
	{
		m_cell = _cell;
		m_stoneOffset = _stoneOffset;
		m_transform = AddComponent<Transform>();
		m_transform->SetPosition(XMVectorSet(offX, offY, 0.0f, 1.0f));
		m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
		m_transform->SetRotation(0.0f);

		m_bitmapRender = AddComponent<BitmapRender3D>("Space.png", drawW, drawH);
		m_bitmapRender->SetOrder(0);
		m_bitmapRender->SetActive(true);

		BoardManager::GetInstance().Initialize(offX, offY, drawW, drawH, _cell, m_stoneOffset, padding); // 보드 매니저 초기화
	}
	void Render(Renderer& r) override
	{
		if (m_bitmapRender) m_bitmapRender->Render(r);

		for (auto& sp : m_stones)
			if (auto* bmp = sp->GetComponent<BitmapRender3D>())
				if (bmp->IsActive()) bmp->Render(r);
	}

	void Update(double) override {}

	void BoardSync();
};

inline void BoardObject::BoardSync()
{
	m_board = BoardManager::GetInstance().GetBoard();

	int boardSize = BoardManager::GetInstance().GetBoardSize();

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





