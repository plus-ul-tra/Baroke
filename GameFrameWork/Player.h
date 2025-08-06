#pragma once
#include "Engine.h"

//�÷��̾ ������ �־�� �Ǵ� �͵�
// �������� �ٵϵ� ����
// ���� ���� ����
// �������� ������


class Player : public Object {
private:

	Transform* m_transform = nullptr;
	BitmapRender3D* m_bitmapRender = nullptr;
	TextRender* m_textRender = nullptr;




	float m_speedPerSec = 500.f;

	int blackStone = 30;
	


public:
	Player(float posX, float posY, float width, float height, DirectX::XMFLOAT4 color);
	virtual ~Player() = default;

	void FixedUpdate(double deltaTime) override;
	void Update(double deltaTime) override;
	void LateUpdate(double deltaTime) override;

	void Move(float dx, float dy);
	void Move(DirectX::XMVECTOR direction, double deltaTime);




	Transform* GetTransform() { return m_transform; }
	BitmapRender3D* GetBitmapRender() { return m_bitmapRender; }

	int GetBlackStone() { return blackStone; }

};