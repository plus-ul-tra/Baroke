#pragma once



class Board
{
public:
	static constexpr int MAX_SIZE = 15; // 점의 개수
	using NodeArray = std::array<std::array<StoneInfo, MAX_SIZE>, MAX_SIZE>;


	Board(int size = MAX_SIZE) : m_size(size) {}

	int  Size() const { return m_size; }		// raw, col 크기 반환 

	bool IsOnBoard(int r, int c) const {		// 해당 좌표가 보드판 위인지 여부 반환
		return r >= 0 && r < m_size && c >= 0 && c < m_size;
	}


	StoneInfo GetStone(int r, int c) const { return m_nodes[r][c]; }	// 특정 노드 위의 있는 돌 반환

	bool PlaceStone(int r, int c, StoneInfo info);



	const NodeArray& GetNodes() const { return m_nodes; }				// 바둑판 모든 노드 반환

private:
	int m_size;          
	NodeArray m_nodes{}; 


	int  CountLiberty(int r, int c,
		std::vector<std::pair<int, int>>& group,
		std::array<std::array<bool, MAX_SIZE>, MAX_SIZE>& visited) const;	// 상대 돌 자유도 검사

	void RemoveGroup(const std::vector<std::pair<int, int>>& group);		// 자유도 0 일 시 해당 그룹 전부 제거

	void ActivateAbility(const StoneInfo& stone, int r, int c);				// 능력 발동 (임시)
};


std::unique_ptr<Board> CreateBoard(int size = Board::MAX_SIZE);

