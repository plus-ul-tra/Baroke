#pragma once

struct PlayerInfo
{
	int m_BlackStone = 30;
	int m_tempBlackStone = 0;
	int m_whiteLeft = 0;
	int m_money = 10;
	int m_waxMoney = 0; // 실링 왁스 머니

	int GetBlackCount() {
		return m_BlackStone + m_tempBlackStone;
	}
	void incBlackCount(int count) { m_tempBlackStone += count; }
	void decBlackCount(int count) { m_tempBlackStone -= count; }
};