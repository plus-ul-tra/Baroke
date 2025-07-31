#include "pch.h"
#include "BoardManager.h"

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

void BoardManager::Initialize()
{
	// �ٵ��� �ʱ�ȭ
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
	m_stoneType = StoneType::White; // �⺻ �� Ÿ���� �� ��
	m_stoneAbility = StoneAbility::None; // �⺻ �� �ɷ��� ����
}

void BoardManager::PlaceRandomStones(int amount)
{
	// ���� �̱���
}

void BoardManager::InputBasedGameLoop(POINT mousePos)
{
	m_selectedPosition = MouseToBoardPosition(mousePos);
	// ���� ���� ���Ⱑ �����ߴٸ� ����
	if (!PlaceStone(m_selectedPosition, m_stoneType, m_stoneAbility)) return;

	JokerAbilityUpdate(); // ��Ŀ �ɷ� ������Ʈ

	// �缮 ����

	m_selectedPosition = { -1, -1 }; // ���������� ���õ� ��ġ �ʱ�ȭ
	m_stoneType = StoneType::White; // �� Ÿ�� �ʱ�ȭ
	m_stoneAbility = StoneAbility::None; // �� �ɷ� �ʱ�ȭ
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
		m_board[selectedPosition.x][selectedPosition.y] = make_shared<WhiteStone>(BoardToScreenPosition(selectedPosition), 50.0f);
	}
	else
	{
		if (stoneAbility == StoneAbility::None)
		{
			// ���� �ɷ��� ���� �浹�̶�� �Ϲ� �浹 ����
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<BlackStone>(BoardToScreenPosition(selectedPosition), 50.0f);
		}
		else
		{
			// �ɷ��� �ֵ�� ��Ŀ ���� ���� // �浹������ �ɷ��� �ִ� ��쵵 ��Ŀ ���� ����
			m_board[selectedPosition.x][selectedPosition.y] = make_shared<JokerStone>(BoardToScreenPosition(selectedPosition), 50.0f, stoneAbility);
			// �ɷ��� �ִ� ��Ŀ ���� ��� �ɷ� ���Ϳ� �߰�
			m_jokerPositions.emplace_back(selectedPosition, stoneAbility);
		}
	}

	// �缮 ������ �� Ÿ�� ���� // JokerStone�̶� StoneType::Black �̶�� �浹�� ����
	m_stoneTypeMap[selectedPosition] = stoneType;

	return true;
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