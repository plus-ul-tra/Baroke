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

    m_geoRender->SetActive(true); // 렌더링 활성화
    m_geoRender->SetOrder(50);
	m_bitmapRender->SetActive(true); // 렌더링 활성화

}

void Player::FixedUpdate(double deltaTime)
{
	//Components 들의 업데이트임
	Object::FixedUpdate(deltaTime);
}

void Player::Update(double deltaTime)
{
    //Components 들의 업데이트임
    Object::Update(deltaTime); //이건 아닐 수도 있음
    //위치 상태 등 모든 component에 대한 변화(update) 는 여기서
    // 조작 되어야 할 것 같음.
}

void Player::LateUpdate(double deltaTime)
{
	//Components 들의 업데이트임
	Object::LateUpdate(deltaTime);
}




