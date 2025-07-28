#include "cpch.h"
#include "BoardObject.h"

using namespace DirectX;
using namespace std;

enum class boardtype
{
	
};

BoardObject::BoardObject(Board* board, float offX, float offY, float drawW, float drawH, int padding)
	: m_board(board)
{

	m_transform = AddComponent<Transform>();
	m_bitmapRender = AddComponent<BitmapRender>("original_test.png", drawW, drawH);

	int cell = static_cast<int>((drawW - 2 * padding) / board->Size());
	m_layout = AddComponent<BoardLayoutComponent>(
		static_cast<int>(offX), static_cast<int>(offY),
		cell, padding);

	m_transform->SetPosition(XMVectorSet(offX, offY, 0.0f, 1.0f));
	m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	m_transform->SetRotation(0.0f);

	m_bitmapRender->SetActive(true); // 렌더링 활성화
}
