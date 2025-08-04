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
	if (m_isPressed)
	{
		BoardManager::GetInstance().SetStoneType(m_stoneType);
		BoardManager::GetInstance().SetStoneAbility(m_jokerAbility);
	}
}