#include "cpch.h"

#include "Board.h"
 


std::unique_ptr<Board> CreateBoard(int size)
{
	return std::make_unique<Board>(size);
}


bool Board::PlaceStone(int r, int c, StoneInfo info)
{
	if (!IsOnBoard(r, c))            return false;						// ���� ���̸� ����
	if (m_nodes[r][c].color != StoneColor::None) return false;			// �ش� ��� ���� ���� ������ ����

	m_nodes[r][c] = info;


	int DR[4] = { -1, 1, 0, 0 };
	int DC[4] = { 0, 0,-1, 1 };

	for (int k = 0; k < 4; ++k)													// �� �� �� �� üũ
	{
		int nr = r + DR[k];
		int nc = c + DC[k];
		if (!IsOnBoard(nr, nc)) continue;

		StoneInfo neighbor = m_nodes[nr][nc];
		if (neighbor.color == StoneColor::None || neighbor.color == info.color) // ����ְų� �Ʊ�(�浹)�̸� ��ŵ
			continue; 

		std::vector<std::pair<int, int>> group;
		std::array<std::array<bool, MAX_SIZE>, MAX_SIZE> visited{};
		int liberty = CountLiberty(nr, nc, group, visited);
		if (liberty == 0)
			RemoveGroup(group);
	}

	return true;
}


int Board::CountLiberty(int r, int c,
	std::vector<std::pair<int, int>>& group,
	std::array<std::array<bool, MAX_SIZE>, MAX_SIZE>& visited) const
{
	StoneColor targetColor = m_nodes[r][c].color;
	if (targetColor == StoneColor::None) return 0;

	int DR[4] = { -1, 1, 0, 0 };
	int DC[4] = { 0, 0,-1, 1 };

	std::stack<std::pair<int, int>> st;
	st.push({ r,c });
	visited[r][c] = true;

	int liberties = 0;

	while (!st.empty())
	{
		auto [cr, cc] = st.top(); st.pop();
		group.emplace_back(cr, cc);

		for (int k = 0; k < 4; ++k)
		{
			int nr = cr + DR[k];
			int nc = cc + DC[k];
			if (!IsOnBoard(nr, nc)) continue;

			if (m_nodes[nr][nc].color == StoneColor::None)
			{
				// �� ĭ �߰� �� ������ 1 ���� (�ߺ� �������� �Ű� ���� ����)
				++liberties;
			}
			else if (m_nodes[nr][nc].color == targetColor && !visited[nr][nc])
			{
				visited[nr][nc] = true;
				st.push({ nr, nc });
			}
		}
	}
	return liberties;
}


void Board::RemoveGroup(const std::vector<std::pair<int, int>>& group)
{
	for (auto [r, c] : group)
	{
		StoneInfo captured = m_nodes[r][c];
		m_nodes[r][c] = { StoneColor::None, StoneAbility::None };
		ActivateAbility(captured, r, c);
	}
}


void Board::ActivateAbility(const StoneInfo& stone, int , int )
{

	(void)stone; 
}

