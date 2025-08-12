#pragma once

struct PlayerInfo
{
	int m_BlackStone = 25;
	int m_blackStoneUpgrade = 0; // 흑돌 업그레이드 횟수
	int m_tempBlackStone = 0;
	int m_whiteLeft = 0;
	int m_money = 100;
	int m_rerollCount = 0; // 상점 리롤 횟수
	int m_waxMoney = 0;
	int m_score = 0;

	PlayerInfo() = default;


	int GetBlackCount() {
		return m_BlackStone + m_tempBlackStone;
	}
	void incBlackCount(int count) { m_tempBlackStone += count; }
	void decBlackCount(int count) { m_tempBlackStone -= count; }
	void ResetRound() { m_tempBlackStone = 0; }
	void initGame() {
		m_BlackStone = 20;
		m_blackStoneUpgrade = 0;
		m_tempBlackStone = 0;
		m_whiteLeft = 0;
		m_money = 6;
		m_rerollCount = 0;
		m_waxMoney = 0;
		m_score = 0;
	}


	// 튜토리얼 초기화
	void TutoinitGame() {
		std::cout << "TutoInit" << std::endl;
		m_BlackStone = 1000;
		m_blackStoneUpgrade = 0;
		m_tempBlackStone = 0;
		m_whiteLeft = 0;
		m_money = 10000;
		m_rerollCount = 0;
		m_waxMoney = 10000;
		m_score = 0;
	}
};