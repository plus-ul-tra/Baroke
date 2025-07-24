#include "cpch.h"
#include "Player.h"

using namespace DirectX;
using namespace std;

Player::Player(float posX, float posY, float width, float height, XMFLOAT4 color) //
{
    // ����ϰ� �������� AddComponent ���� �������Ķ���� X
    // Set �Լ� ���� ���� �ʱ�ȭ (Transform, Collider ����)
    m_transform = AddComponent<Transform>();
    m_Collider  = AddComponent<Collider2D>();
    //GeoRender�� collider2D �ִ� ��츸 ���
    m_geoRender = AddComponent<GeoRender>();
    
    m_transform->SetPosition(XMVectorSet(posX, posY, 0.0f, 1.0f));
    m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
    m_transform->SetRotation(0.0f);

    m_Collider->SetColliderType(ColliderType::Circle);
    m_Collider->SetSize(100.0f, 100.0f); //collider ũ��� ���� ����
    m_Collider->SetColor(color); //�÷��� ��� GeoRender���� �����ص���

    m_geoRender->SetActive(true); //������ Ȱ��ȭ, ���߿� �ϰ��� �ٲ� ��
    m_geoRender->SetOrder(50);

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




