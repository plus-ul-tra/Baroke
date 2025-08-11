#pragma once

#include "Renderer.h"
#include "Component.h"

using namespace std;

class UIText : public Component {
private:
	float m_posX{};
	float m_posY{};
	float m_width{};
	float m_height{};      //rect size ��
	int m_sizeType{};
	//bool m_dirty{ true }; //�Ƚᵵ �� �� ����

	wstring m_Contents;
	//Microsoft::WRL::ComPtr<IDWriteTextLayout> m_layout;
public:
	UIText(float posX, float posY, float width, float height,int size) : m_posX(posX), m_posY(posY), m_width(width), m_height(height), m_sizeType(size){};
	virtual ~ UIText() = default;

	
	void Update(double deltaTime) override {};
	void OnEvent(const std::string& ev) override {};
	// ���� �޾Ƽ� ����
	void SetText(int value) {
		m_Contents = std::to_wstring(value);
	}

	void TextRender(Renderer& renderer) {
		renderer.DrawUIText(m_Contents,m_posX,m_posY,m_width,m_height, m_sizeType);
	}
};