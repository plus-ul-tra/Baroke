#include "cpch.h"
#include "Player.h"

using namespace DirectX;
using namespace std;

Player::Player(float x, float y, float width, float height, XMFLOAT4 color)
{
    m_transform = AddComponent<Transform>();
	m_geoRender = AddComponent<GeoRender>(
		width, height, color, ShapeType::Rectangle
	);
    m_bitmapRender = AddComponent<BitmapRender>(L"PandaSpriteSheet", width, height);


    m_transform->SetPosition(XMVectorSet(x, y, 0.0f, 1.0f));
    m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
    m_transform->SetRotation(0.0f);

    m_geoRender->SetActive(true); // ������ Ȱ��ȭ
    m_geoRender->SetOrder(50);
	m_bitmapRender->SetActive(true); // ������ Ȱ��ȭ

}

void Player::FixedUpdate(double deltaTime)
{
	//Components ���� ������Ʈ��
	Object::FixedUpdate(deltaTime);
}

void Player::Update(double deltaTime)
{
    //Components ���� ������Ʈ��
    Object::Update(deltaTime); //�̰� �ƴ� ���� ����
    //��ġ ���� �� ��� component�� ���� ��ȭ(update) �� ���⼭
    // ���� �Ǿ�� �� �� ����.
}

void Player::LateUpdate(double deltaTime)
{
	//Components ���� ������Ʈ��
	Object::LateUpdate(deltaTime);
}




