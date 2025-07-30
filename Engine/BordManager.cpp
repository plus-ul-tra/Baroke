#include "pch.h"
#include "BordManager.h"

void BordManager::PlaceStone(int row, int col, StoneType stoneType, StoneAbility stoneAbility)
{
	if (row < 0 || row >= SIZE_DEFAULT || col < 0 || col >= SIZE_DEFAULT)
	{
		std::cout << "��ȿ���� ���� ��ġ: (" << row << ", " << col << ")" << std::endl;
		return;
	}
	if (m_board[row][col])
	{
		std::cout << "�̹� ���� �ִ� ��ġ: (" << row << ", " << col << ")" << std::endl;
		return;
	}
	
	m_board[row][col] = make_shared<Stone>(stoneType, stoneAbility);
	m_stoneTypeMap[{row, col}] = stoneType;

	if (stoneType == StoneType::Joker) { m_jokerPositions.push_back({ {row, col}, stoneAbility }); }
}

shared_ptr<Stone> BordManager::GetStone(int row, int col)
{
	if (row < 0 || row >= SIZE_DEFAULT || col < 0 || col >= SIZE_DEFAULT)
	{
		std::cout << "��ȿ���� ���� ��ġ: (" << row << ", " << col << ")" << std::endl;
		return nullptr;
	}

	return m_board[row][col];
}