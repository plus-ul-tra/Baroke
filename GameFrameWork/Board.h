#pragma once



class Board
{
public:
	static constexpr int MAX_SIZE = 15; // ���� ����
	using NodeArray = std::array<std::array<StoneInfo, MAX_SIZE>, MAX_SIZE>;


	Board(int size = MAX_SIZE) : m_size(size) {}

	int  Size() const { return m_size; }		// raw, col ũ�� ��ȯ 

	bool IsOnBoard(int r, int c) const {		// �ش� ��ǥ�� ������ ������ ���� ��ȯ
		return r >= 0 && r < m_size && c >= 0 && c < m_size;
	}


	StoneInfo GetStone(int r, int c) const { return m_nodes[r][c]; }	// Ư�� ��� ���� �ִ� �� ��ȯ

	bool PlaceStone(int r, int c, StoneInfo info);



	const NodeArray& GetNodes() const { return m_nodes; }				// �ٵ��� ��� ��� ��ȯ

private:
	int m_size;          
	NodeArray m_nodes{}; 


	int  CountLiberty(int r, int c,
		std::vector<std::pair<int, int>>& group,
		std::array<std::array<bool, MAX_SIZE>, MAX_SIZE>& visited) const;	// ��� �� ������ �˻�

	void RemoveGroup(const std::vector<std::pair<int, int>>& group);		// ������ 0 �� �� �ش� �׷� ���� ����

	void ActivateAbility(const StoneInfo& stone, int r, int c);				// �ɷ� �ߵ� (�ӽ�)
};


std::unique_ptr<Board> CreateBoard(int size = Board::MAX_SIZE);

