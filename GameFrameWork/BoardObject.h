#pragma once
#include "Engine.h"      
#include "Board.h"                


class BoardObject : public Object
{
private:
	Board* m_board;
	BoardLayoutComponent* m_layout = nullptr;
	Transform* m_transform = nullptr;
	BitmapRender* m_bitmapRender = nullptr;

public:

    BoardObject(Board* board, float offX, float offY, float drawW, float drawH, int padding = 0);

    void Update(double) override {}

    Board* GetBoard()       noexcept { return m_board; }
    const Board* GetBoard() const noexcept { return m_board; }

    POINT BoardToScreen(int r, int c) const { return m_layout->BoardToScreen(r, c); }
    std::pair<int, int> ScreenToBoard(int x, int y) const { return m_layout->ScreenToBoard(x, y); }


};

