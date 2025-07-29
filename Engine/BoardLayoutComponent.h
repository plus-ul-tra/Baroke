#pragma once
#include "Component.h"

class BoardLayoutComponent : public Component
{
public:
	BoardLayoutComponent(int offX, int offY, int cell, int padding = 0)
		: m_offX(offX), m_offY(offY), m_cell(cell), m_padding(padding) {
		std::cout << m_offX << " " << m_offY <<" "<< m_cell<< std::endl;
	}

	void Update(double) override {}
	void OnEvent(const std::string&) override {}


	POINT BoardToScreen(int r, int c) const {
		return { m_offX + m_padding + c * m_cell,
				 m_offY + m_padding + r * m_cell };
	}
	std::pair<int, int> ScreenToBoard(int px, int py) const {
		int col = (px - m_offX - m_padding + m_cell / 2) / m_cell;
		int row = (py - m_offY - m_padding + m_cell / 2) / m_cell;
		return { row, col };
	}
	int GetCell() { return m_cell; }

private:
	int m_offX, m_offY, m_cell, m_padding;
};
