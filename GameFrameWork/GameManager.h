#pragma once
#include "GameScene.h"
#include "TitleScene.h"

class GameManager : public Singleton<GameManager> {

	friend class Singleton<GameManager>;

private:
	GameManager() { std::cout << "GameManager ����" << std::endl; }
	~GameManager() = default;
	
	bool m_isGameOver = FALSE;
	bool m_isDeBug = false;

public:
	void Initialize();

	bool GetIsGameOver() const { return m_isGameOver; }
	bool SetIsGameOver(bool flag) { m_isGameOver = flag; }

	void SetGameScene();
	void GetModeState(); //����� ��� ���� ��������

};