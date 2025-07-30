#include "pch.h"
#include "TextRender.h"

TextRender::TextRender(const string& text, float width, float height, float fontSize, D2D1::ColorF m_textColor, float textDelay, const wchar_t* fontName)
	: m_text(text), m_width(width), m_height(height), m_textColor(m_textColor), m_textDelay(textDelay)
{
	ComPtr<IDWriteFactory> writeFactory;
	DWriteCreateFactory
	(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf())
	);

	writeFactory->CreateTextFormat
	(
		fontName,
		nullptr,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		L"",
		&m_textFormat
	);

	if (textDelay < 0.01f) m_renderText = text;
	else m_textLength = static_cast<float>(text.length() + 1);
}

void TextRender::Update(double deltaTime)
{
	if (m_textDelay <= 0.01f) { return; }
	float elapsedTime = static_cast<float>(m_elapsedTime);
	if (!m_isActive || !m_transform || m_textLength < elapsedTime / m_textDelay) { return; }

	m_elapsedTime += deltaTime;

	size_t charCount = static_cast<size_t>(elapsedTime / m_textDelay);
	if (charCount > m_textLength) charCount = static_cast<size_t>(m_textLength);
	m_renderText = m_text.substr(0, charCount);
}

void TextRender::Render(Renderer& renderer)
{
	if (!m_isActive || !m_transform) { return; }

	XMVECTOR worldPosition = m_transform->GetPosition();
	XMVECTOR worldScale = m_transform->GetScale();

	float objWorldX = XMVectorGetX(worldPosition);
	float objWorldY = XMVectorGetY(worldPosition);

	D2D1_RECT_F textRect = D2D1::RectF
	(
		objWorldX,
		objWorldY,
		objWorldX + (m_width * XMVectorGetX(worldScale)),
		objWorldY + (m_height * XMVectorGetY(worldScale))
	);

	renderer.DrawMessage
	(
		std::wstring(m_renderText.begin(), m_renderText.end()).c_str(),
		textRect,
		m_textColor,
		m_textFormat
	);
}