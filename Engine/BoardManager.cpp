#include "pch.h"
#include "BoardManager.h"


static constexpr int DR[4] = { -1, 1, 0, 0 };
static constexpr int DC[4] = { 0, 0,-1, 1 };

inline bool IsOnBoard(int r, int c)
{
	return r >= 0 && r < SIZE_DEFAULT && c >= 0 && c < SIZE_DEFAULT;
}

// �� �ɷ¿� ���� �Լ� �� // �ش��ϴ� ��Ŀ�� ��Ŀ�� ��ġ�� ���� // ���߿� �ٸ� ��ġ�� �̵��� ���� ����
static unordered_map<StoneAbility, function<void(shared_ptr<JokerStone>)>> g_abilityFunctions =
{
	{ StoneAbility::JokerAbility1, [](shared_ptr<JokerStone> jokerStone)
	{
		// ��Ŀ �ɷ� 1
		cout << jokerStone->GetPosition().x << ", " << jokerStone->GetPosition().y << endl;
	} },

	{ StoneAbility::JokerAbility2, [](shared_ptr<JokerStone> jokerStone)
	{
		// ��Ŀ �ɷ� 2
		cout << jokerStone->GetPosition().x << ", " << jokerStone->GetPosition().y << endl;
	} },

	{ StoneAbility::JokerAbility3, [](shared_ptr<JokerStone> jokerStone)
	{
		// ��Ŀ �ɷ� 3
		cout << jokerStone->GetPosition().x << ", " << jokerStone->GetPosition().y << endl;
	} }
};

void BoardManager::Initialize(int offX, int offY, int drawW, int drawH, int _cell, int _stoneOffset, int padding)
{
	m_offX = offX;
	m_offY = offY;
	m_drawW = (drawW / 2) - padding;
	m_drawH = (drawH / 2) - padding;
	m_cell = _cell;
	m_padding = padding;
	m_stoneOffset = _stoneOffset;
	// �ٵ��� �ʱ�ȭ
	ResetStone();
}

void BoardManager::PlaceRandomStones(int amount)
{
	int N = SIZE_DEFAULT;
	std::vector<std::pair<int, int>> allPositions;

	for (int r = 0; r < N; ++r)
		for (int c = 0; c < N; ++c)
			allPositions.emplace_back(r, c);

	std::shuffle(allPositions.begin(), allPositions.end(), std::mt19937(std::random_device{}()));


	int n = std::min(static_cast<int>(allPositions.size()), amount);

	for (int i = 0; i < n; ++i)
	{
		int r = allPositions[i].first;
		int c = allPositions[i].second;
		PlaceStone({ r, c }, StoneType::White, StoneAbility::None);
	}
}


bool BoardManager::InputBasedGameLoop(POINT mousePos)
{
	m_selectedPosition = MouseToBoardPosition(mousePos);
	std::cout << m_selectedPosition.x << " " << m_selectedPosition.y << std::endl;
	// ���� ���� ���Ⱑ �����ߴٸ� ����
	if (!PlaceStone(m_selectedPosition, m_stoneType, m_stoneAbility)) return false;

	JokerAbilityUpdate(); // ��Ŀ �ɷ� ������Ʈ

	m_selectedPosition = { -1, -1 }; // ���������� ���õ� ��ġ �ʱ�ȭ
	m_stoneType = StoneType::Black; // �� Ÿ�� �ʱ�ȭ
	m_stoneAbility = StoneAbility::None; // �� �ɷ� �ʱ�ȭ

	return true;
}

bool BoardManager::InputBasedGameLoop(int row, int col) // row , col �Է� �޾Ƽ� �ش� �迭�� �׼��� �ؼ� �������
{

/*	m_selectedPosition = BoardToScreenPosition({ row,col });*/

	std::cout <<"row , col = " << row << " " << col << std::endl;
	// ���� ���� ���Ⱑ �����ߴٸ� ����
	if (!PlaceStone({row,col}, m_stoneType, m_stoneAbility)) return false;

	JokerAbilityUpdate(); // ��Ŀ �ɷ� ������Ʈ


	m_selectedPosition = { -1, -1 }; // ���������� ���õ� ��ġ �ʱ�ȭ
	m_stoneType = StoneType::Black; // �� Ÿ�� �ʱ�ȭ
	m_stoneAbility = StoneAbility::None; // �� �ɷ� �ʱ�ȭ

	return true;
}

void BoardManager::JokerAbilityUpdate()
{
	for (const auto& joker : m_jokerPositions)
	{
		POINT position = joker.first; // ��Ŀ ���� ��ġ
		shared_ptr<Stone> stone = GetStone(position); // �ش� ��ġ�� Stone(Object) ��������

		StoneAbility ability = joker.second; // ��Ŀ ���� �ɷ�

		shared_ptr<JokerStone> jokerStone = dynamic_pointer_cast<JokerStone>(stone); // Stone�� JokerStone���� ĳ����

		auto it = g_abilityFunctions.find(ability);
		if (it != g_abilityFunctions.end())
		{
			it->second(jokerStone);
		}
	}
}



POINT BoardManager::MouseToBoardPosition(POINT mousePos) const
{
	return
	{
		(mousePos.x+ m_drawW + m_cell/2)/ m_cell,
		-(mousePos.y - m_drawH - m_cell / 2) / m_cell
//  		(mousePos.x - m_offX - m_padding + m_cell / 2) / m_cell,
//  		(mousePos.y - m_offY - m_padding + m_cell / 2) / m_cell
	};
}

// ���� �ε���(row, col) �� ȭ��(�ȼ�)
POINT BoardManager::BoardToScreenPosition(POINT boardPos) const
{
	return
	{
		boardPos.x*m_cell- m_drawW - m_cell / 2,
		-boardPos.y*m_cell + m_drawH - m_cell / 2
//  		m_offX + m_padding + boardPos.x * m_cell,
//  		m_offY + m_padding + boardPos.y * m_cell
	};
}

bool BoardManager::PlaceStone(POINT selectedPosition, StoneType stoneType, StoneAbility stoneAbility)
{
	if (selectedPosition.x < 0 || selectedPosition.x >= SIZE_DEFAULT || selectedPosition.y < 0 || selectedPosition.y >= SIZE_DEFAULT)
	{
		std::cout << "��ȿ���� ���� ��ġ: (" << selectedPosition.x << ", " << selectedPosition.y << ")" << std::endl;
		return false;
	}
	if (m_board[selectedPosition.x][selectedPosition.y])
	{
		std::cout << "�̹� ���� �ִ� ��ġ: (" << selectedPosition.x << ", " << selectedPosition.y << ")" << std::endl;
		return false;
	}

	if (stoneType == StoneType::White)
	{
		// �� �� ���� // row, col, size�� ���߿� ��ũ���� �´� ��ǥ�� �����ؾ���
		m_board[selectedPosition.x][selectedPosition.y] = make_shared<WhiteStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset);
	}
	else
	{
		if (stoneAbility == StoneAbility::None)
		{
			// ���� �ɷ��� ���� �浹�̶�� �Ϲ� �浹 ����
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<BlackStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset);
		}
		else
		{
			// �ɷ��� �ֵ�� ��Ŀ ���� ���� // �浹������ �ɷ��� �ִ� ��쵵 ��Ŀ ���� ����
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), m_cell , m_stoneOffset, stoneAbility);
			// �ɷ��� �ִ� ��Ŀ ���� ��� �ɷ� ���Ϳ� �߰�
			m_jokerPositions.emplace_back(selectedPosition, stoneAbility);
		}
	}

	// �缮 ������ �� Ÿ�� ���� // JokerStone�̶� StoneType::Black �̶�� �浹�� ����
	m_stoneTypeMap[selectedPosition] = stoneType;


	for (int k = 0; k < 4; ++k) {
		int nr = selectedPosition.x + DR[k], nc = selectedPosition.y + DC[k];
		if (!IsOnBoard(nr, nc)) continue;

		auto it = m_stoneTypeMap.find({ nr, nc });
		if (it == m_stoneTypeMap.end() || it->second == StoneType::Black || it->second == StoneType::Joker) continue;
		std::vector<POINT> grp;
		std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT> vis{};
		if (CountLiberty(nr, nc, grp, vis) == 0)
			RemoveGroup(grp);          // �� ���� ����
	}

	return true;
}

void BoardManager::ResetStone()
{
	for (int i = 0; i < SIZE_DEFAULT; ++i)
	{
		for (int j = 0; j < SIZE_DEFAULT; ++j)
		{
			m_board[i][j] = nullptr; // ��� ��ġ�� nullptr�� �ʱ�ȭ
		}
	}
	m_stoneTypeMap.clear(); // �� ���� �� �ʱ�ȭ
	m_jokerPositions.clear(); // ��Ŀ �� ��ġ�� �ɷ� ���� �ʱ�ȭ
	m_selectedPosition = { -1, -1 }; // ���õ� ��ġ �ʱ�ȭ
	m_stoneType = StoneType::Black; // �⺻ �� Ÿ���� �� ��
	m_stoneAbility = StoneAbility::None; // �⺻ �� �ɷ��� ����
}

shared_ptr<Stone> BoardManager::GetStone(POINT position)
{
	if (position.x < 0 || position.x >= SIZE_DEFAULT || position.y < 0 || position.y >= SIZE_DEFAULT)
	{
		std::cout << "��ȿ���� ���� ��ġ: (" << position.x << ", " << position.y << ")" << std::endl;
		return nullptr;
	}

	return m_board[position.x][position.y];
}

bool BoardManager::IsJokerStone(POINT position) const
{
	if (m_jokerPositions.size() > 0) return false;

	for (const auto& joker : m_jokerPositions) // ��Ŀ �� ��ġ�� �ɷ� ���͸� ��ȸ
	{
		if (position == joker.first)
		{
			return true;
		}
	}

	return false;
}

int BoardManager::GetStoneTypeAmount(StoneType type) const
{
	int count = 0;
	for (const auto& pair : m_stoneTypeMap)
	{
		if (pair.second == type && !IsJokerStone(pair.first))
		{
			++count;
		}
	}

	return count;
}

int BoardManager::CountLiberty(
	int r, int c,
	std::vector<POINT>& group,
	std::array<std::array<bool, SIZE_DEFAULT>, SIZE_DEFAULT>& visited) const
{
	auto it0 = m_stoneTypeMap.find({ r,c });
	if (it0 == m_stoneTypeMap.end()) return 0;
	StoneType target = it0->second;

	std::stack<POINT> st;  st.push({ r,c });
	visited[r][c] = true;
	int libs = 0;

	while (!st.empty()) {
		auto [cr, cc] = st.top(); st.pop();
		group.push_back({ cr,cc });

		for (int k = 0; k < 4; ++k) {
			int nr = cr + DR[k], nc = cc + DC[k];
			if (!IsOnBoard(nr, nc)) continue;

			auto it = m_stoneTypeMap.find({ nr,nc });
			if (it == m_stoneTypeMap.end()) { ++libs; continue; }  // ���� �¿� üũ �� ���� �� ã�� ���
			if (it->second ==StoneType::Joker) { ++libs; continue; }  // ���� �¿� üũ �� ���� �� ã�� ���
			if (it->second == target && !visited[nr][nc]) {			// ���� �¿� üũ �� ���� ã�� ���
				visited[nr][nc] = true;  st.push({ nr,nc });
			}
		}
	}
	return libs;
}

void BoardManager::RemoveGroup(const std::vector<POINT>& g)
{
	for (POINT p : g) {
		m_board[p.x][p.y] = nullptr;
		m_stoneTypeMap.erase(p);
	}
}
