#include "pch.h"
#include "Engine.h"

void Button::CheckInput(const MouseEvent& mouseEvent)
{

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
		if (!m_isActive) { m_isPressed = false; return; }
		m_inputType = mouseEvent.type;
		if (m_inputType == MouseType::LDown) m_isPressed = true;
		else m_isPressed = false;
	}
	else
	{
		m_isHovered = false;
	}
}

void Button::Render(Renderer& renderer)
{
	if (m_bitmapRender->IsActive() && m_isHovered && m_textObject)
	{
		m_textObject->GetComponent<BitmapRender3D>()->Render(renderer);
	}
}

void JokerButton::ButtonFunction()
{
	if (m_isPressed && m_isActive)
    {

		std::cout << "button click" << std::endl;

		BoardManager::GetInstance().SetMode(UIMode::Sacrifice);
		BoardManager::GetInstance().SetStoneType(m_stoneType);
		BoardManager::GetInstance().SetStoneAbility(m_jokerAbility);
		m_isPressed = false;
        
    }
	if (m_isHovered && m_isActive)
	{

	}
}

void JokerButton::SetButtonJoker(StoneType stoneType, StoneAbility ability)
{
	m_jokerAbility = ability;
	m_stoneType = stoneType;

	m_bitmapRender->ChangeTexture(m_jokerInfoMap[ability].fileName);
	m_textObject = nullptr;
	m_textObject = make_unique<NewObject>(m_transform->GetPosition().m128_f32[0], m_transform->GetPosition().m128_f32[1] + 20.0f, 691.0f, 200.0f, 0.0f, m_jokerInfoMap[ability].toolTipName);

	BindEnabledPredicate(BuildPredicate(ability));
}

void ShopJokerButton::SetButtonJoker(JokerStoneInfo jokerInfo, StoneAbility ability)
{
	m_jokerInfo = jokerInfo;
	m_jokerAbility = ability;

	m_bitmapRender->ChangeTexture(jokerInfo.fileName);
	m_textObject = nullptr;
	m_textObject = make_unique<NewObject>(m_transform->GetPosition().m128_f32[0], m_transform->GetPosition().m128_f32[1] + 20.0f, 691.0f, 200.0f, 0.0f, m_jokerInfoMap[ability].toolTipName);
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
			for (auto& jokerButton : *m_jokerButton)
			{
				if (!jokerButton) continue;
				if (jokerButton->GetJokerAbility() != StoneAbility::None) continue; // �̹� �ɷ��� �ִ� ��Ŀ ��ư�� �ǳʶ�

				jokerButton->SetButtonJoker(m_jokerInfo.stoneType, m_jokerAbility);
				m_boardManager.m_playerInfo.m_money -= m_jokerInfo.costWhite;

				break;
			}
			SetShowAndActive(false); // ���� ��Ŀ ��ư ����

			std::cout << "Money : " << m_boardManager.m_playerInfo.m_money << std::endl;
		}
		else
		{
			std::cout << "Not enough money to buy this joker." << std::endl;
		}

		m_isPressed = false;
	}
	if (m_isHovered && m_isActive)
	{

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