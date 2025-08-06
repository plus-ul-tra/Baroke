#include "pch.h"
#include "Button.h"

void Button::CheckInput(const MouseEvent& mouseEvent)
{
	if (!m_isActive) { m_isPressed = false; return; }

	POINT mousePos = mouseEvent.pos;
	bool isInX =
		(
			mousePos.x >= m_transform->GetPosition().m128_f32[0] - m_width / 2
			&& mousePos.x <= m_transform->GetPosition().m128_f32[0] + m_width / 2
		);

	bool isInY =
		(
			mousePos.y >= m_transform->GetPosition().m128_f32[1] - m_height / 2
			&& mousePos.y <= m_transform->GetPosition().m128_f32[1] + m_height / 2
		);

	if (isInX && isInY)
	{
		m_isHovered = true;
		m_inputType = mouseEvent.type;
		if (m_inputType == MouseType::LDown) m_isPressed = true;
		else m_isPressed = false;
	}
	else
	{
		m_isHovered = false;
	}
}

void JokerButton::ButtonFunction()
{
// 	if (m_isPressed)
// 	{
// 		BoardManager::GetInstance().SetStoneType(m_stoneType);
// 		BoardManager::GetInstance().SetStoneAbility(m_jokerAbility);
// 	}

	//if (m_isPressed && m_isActive)
	//{
	//	auto& bm = BoardManager::GetInstance();
	//	bm.SetStoneType(m_stoneType);
	//	bm.SetStoneAbility(m_jokerAbility);
	//}

	if (m_isPressed && m_isActive)
    {

		std::cout << "button click" << std::endl;

		BoardManager::GetInstance().SetMode(UIMode::Sacrifice);
		BoardManager::GetInstance().SetStoneType(m_stoneType);
		BoardManager::GetInstance().SetStoneAbility(m_jokerAbility);
		m_isPressed = false;
        
    }
}

void ShopJokerButton::SetShowAndActive(bool active)
{
	if (m_bitmapRender) m_bitmapRender->SetActive(active);

	m_isActive = active;
}

void ShopJokerButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		if (m_boardManager.m_playerInfo.m_money >= m_jokerInfo.costWhite)
		{
			m_boardManager.m_playerInfo.m_money -= m_jokerInfo.costWhite;
			m_jokerButton->SetButtonJoker(m_jokerInfo.stoneType, m_jokerAbility);

			std::cout << "Money : " << m_boardManager.m_playerInfo.m_money << std::endl;
		}
		else
		{
			std::cout << "Not enough money to buy this joker." << std::endl;
		}

		m_isPressed = false;
	}
}

bool ShopEndButton::IsEndButtonPressed()
{
	if (!m_isEndButtonPressed) return false;

	m_isEndButtonPressed = false;
	return true;
}

void ShopEndButton::SetShowAndActive(bool active)
{
	if (m_bitmapRender) m_bitmapRender->SetActive(active);

	m_isActive = active;
	m_isEndButtonPressed = false;
}

void ShopEndButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
	{
		m_isEndButtonPressed = true;
		std::cout << "Shop End Button Pressed" << std::endl;

		m_isPressed = false;
	}
}