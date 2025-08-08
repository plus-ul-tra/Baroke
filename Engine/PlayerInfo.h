#pragma once

struct PlayerInfo
{
	int m_BlackStone = 30;
	int m_tempBlackStone = 0;
	int m_whiteLeft = 0;
	int m_money = 10;
	int m_waxMoney = 0;

	int GetBlackCount() {
		return m_BlackStone + m_tempBlackStone;
	}
	void incBlackCount(int count) { m_tempBlackStone += count; }
	void decBlackCount(int count) { m_tempBlackStone -= count; }
	void ResetRount() { m_tempBlackStone = 0; }
	void initGame() {
		m_BlackStone = 30;
		m_tempBlackStone = 0;
		m_whiteLeft = 0;
		m_money = 10;
		m_waxMoney = 0;
	}
};