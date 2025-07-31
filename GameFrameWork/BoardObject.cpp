#include "cpch.h"
#include "BoardObject.h"


using namespace DirectX;
using namespace std;




BoardObject::BoardObject(Board* board, int offX, int offY, int drawW, int drawH, int padding, int _stoneoffset)

	: m_board(board)
{
	m_stoneOffset = _stoneoffset;
	m_transform = AddComponent<Transform>();

	m_bitmapRender = AddComponent<BitmapRender>("Cyberpunk.png", drawW, drawH);


	int cell =(drawW - 2 * padding) / (board->Size() - 1);
	std::cout << "Cell : " << cell << ", board size : " << board->Size() << std::endl;
	m_layout = AddComponent<BoardLayoutComponent>(
		static_cast<int>(offX), static_cast<int>(offY),
		cell, padding);
	m_transform->SetPosition(XMVectorSet(offX, offY, 0.0f, 1.0f));
	m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	m_transform->SetRotation(0.0f);

	m_bitmapRender->SetOrder(0);
	m_bitmapRender->SetActive(true); // 렌더링 활성화
}


//튜토리얼 >> 조커

//튜토리얼 >> UI >> 쉐이더 작업이 끝나고  UI 박스