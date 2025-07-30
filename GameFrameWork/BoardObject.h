#pragma once
#include "Engine.h"      
#include "Board.h"             
#include "StoneObject.h"
#include "Joker.h"

class BoardObject : public Object
{
public:
	BoardObject(Board* b, int offX, int offY,
		int drawW, int drawH, int padding = 0, int stoneOffset = 0);

	void Update(double) override
	{
		SyncStones();          // 매 프레임 board 상태와 오브젝트를 맞춤
	}
	Board* GetBoard()       noexcept { return m_board; }
	const Board* GetBoard() const noexcept { return m_board; }

	POINT BoardToScreen(int r, int c) const { return m_layout->BoardToScreen(r, c); }
	std::pair<int, int> ScreenToBoard(int x, int y) const { return m_layout->ScreenToBoard(x, y); }

	std::vector<std::unique_ptr<StoneObject>>& StealStones() { return m_stones; }

	void Render(Renderer& r) override
	{

		m_bitmapRender->Render(r);  

		for (auto& sp : m_stones)
			if (auto* bmp = sp->GetComponent<BitmapRender>())
				if (bmp->IsActive()) bmp->Render(r);
	}


private:
	void SyncStones()
	{
		const auto& nodes = m_board->GetNodes();
		int N = m_board->Size();

		// 1) 필요 없는 StoneObject 제거
		for (auto it = m_stones.begin(); it != m_stones.end(); )
		{
			auto* stoneobj = it->get();
			auto pos = stoneobj->GetTagPos();
			if (nodes[pos.first][pos.second].color == StoneColor::None)
				it = m_stones.erase(it);        
			else ++it;
		}

		// 2) 새 돌 생성
		for (int r = 0; r < N; ++r)
			for (int c = 0; c < N; ++c)
			{
				if (nodes[r][c].color == StoneColor::None) continue;
				if (HasStone(r, c)) continue;     // 이미 있으면 skip

				POINT p = m_layout->BoardToScreen(r, c);
				float size = static_cast<float>(m_layout->GetCell()) - m_stoneOffset;

				if (nodes[r][c].ability == StoneAbility::None)
				{
					auto stone = std::make_unique<StoneObject>(
						nodes[r][c].color,
						static_cast<float>(p.x),
						static_cast<float>(p.y),
						size);
					stone->SetTagPos(r, c);           // (row,col) 기록
					m_stones.emplace_back(std::move(stone));
				}
				else if (nodes[r][c].ability == StoneAbility::ability1)
				{
					auto stone =
						std::make_unique<Joker>
						(
							JokerManager::GetInstance().GetJokerInfo("JokerStone1"),
							static_cast<float>(p.x),
							static_cast<float>(p.y),
						size
						);
					stone->UpdateAbility();
					stone->SetTagPos(r, c);           // (row,col) 기록
					m_stones.emplace_back(std::move(stone));
				}
				else if (nodes[r][c].ability == StoneAbility::ability2)
				{
					auto stone =
						std::make_unique<Joker>
						(
							JokerManager::GetInstance().GetJokerInfo("JokerStone2"),
							static_cast<float>(p.x),
							static_cast<float>(p.y),
						size
						);
					stone->UpdateAbility();
					stone->SetTagPos(r, c);           // (row,col) 기록
					m_stones.emplace_back(std::move(stone));
				}
			}
	}

	bool HasStone(int r, int c) const
	{
		return std::any_of(m_stones.begin(), m_stones.end(),
			[r, c](auto& s) { return s->GetTagPos() == std::pair{ r,c }; });
	}

	int m_stoneOffset = 0;
	Board* m_board;
	BoardLayoutComponent* m_layout;
	Transform* m_transform = nullptr;
	BitmapRender* m_bitmapRender = nullptr;
	std::vector<std::unique_ptr<StoneObject>> m_stones;
};
