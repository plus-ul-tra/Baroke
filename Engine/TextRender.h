#pragma once
#include "Engine.h"
#include "BaseRender.h"

class TextRender : public BaseRender
{
	string m_text;
	string m_renderText;

	float m_textLength = 0.0f;
	float m_textDelay; // 텍스트가 나타나는 속도
	double m_elapsedTime = 0.0;

	ComPtr<IDWriteTextFormat> m_textFormat = nullptr;
	D2D1::ColorF m_textColor;

	float m_width;
	float m_height;

public:
	TextRender(const string& text, float width, float height, float fontSize, D2D1::ColorF m_textColor = D2D1::ColorF(D2D1::ColorF::Black), float textDelay = 0.0f, const wchar_t* fontName = L"Arial");

	void Update(double deltaTime) override;
	void Render(Renderer& renderer) override;
};