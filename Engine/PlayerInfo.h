#pragma once

struct PlayerInfo
{
	int m_BlackStone = 20;
	int m_blackStoneUpgrade = 0; // Èæµ¹ ¾÷±×·¹ÀÌµå È½¼ö
	int m_tempBlackStone = 0;
	int m_whiteLeft = 0;
	int m_money = 100;
	int m_rerollCount = 0; // »óÁ¡ ¸®·Ñ È½¼ö
	int m_waxMoney = 0;

	int GetBlackCount() {
		return m_BlackStone + m_tempBlackStone;
	}
	void incBlackCount(int count) { m_tempBlackStone += count; }
	void decBlackCount(int count) { m_tempBlackStone -= count; }
	void ResetRount() { m_tempBlackStone = 0; }
	void initGame() {
		m_BlackStone = 12;
		m_tempBlackStone = 0;
		m_whiteLeft = 0;
		m_money = 6;
		m_waxMoney = 0;
	}
};