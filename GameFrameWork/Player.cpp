#include "cpch.h"
#include "Player.h"

using namespace DirectX;
using namespace std;

Player::Player(float posX, float posY, float width, float height, XMFLOAT4 color) //
{
    // 깔끔하게 보기위해 AddComponent 에서 생성자파라미터 X
    // Set 함수 각자 만들어서 초기화 (Transform, Collider 참고)
    m_transform = AddComponent<Transform>();
    m_bitmapRender = AddComponent<BitmapRender>("PandaSpriteSheet", width, height);
    m_Collider  = AddComponent<Collider2D>();

    //GeoRender는 collider2D 있는 경우만 사용
    m_geoRender = AddComponent<GeoRender>();
  
    m_transform->SetPosition(XMVectorSet(posX, posY, 0.0f, 1.0f));
    m_transform->SetScale(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
    m_transform->SetRotation(0.0f);

    m_Collider->SetColliderType(ColliderType::Circle);
    m_Collider->SetSize(100.0f, 100.0f); //collider 크기는 별도 지정
    m_Collider->SetColor(color); //컬러는 사실 GeoRender에서 결정해도됨

    m_geoRender->SetActive(true); //렌더링 활성화, 나중에 일괄로 바꿀 것
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


// void Player::MoveUp() { Move(0.0f, -1.0f); }
// void Player::MoveDown() { Move(0.0f, 1.0f); }
// void Player::MoveLeft() { Move(-1.0f, 0.0f); }
// void Player::MoveRight() { Move(1.0f, 0.0f); }


// 버니합 발생 수정해야됨! - 준혁
// 수정완료 ( MoveUp, Down, Left, Right 사용 >> 벡터 사용 )
void Player::Move(float dx, float dy)
{
	float distance = m_speedPerSec;
	auto pos = m_transform->GetPosition();
	pos = XMVectorAdd(pos, XMVectorSet(dx * distance, dy * distance, 0, 0));
	m_transform->SetPosition(pos);
}


void Player::Move(DirectX::XMVECTOR direction, double deltaTime)
{
	float distance = static_cast<float>(m_speedPerSec * deltaTime);
	auto pos = m_transform->GetPosition();
	pos = XMVectorAdd(pos, XMVectorScale(direction, distance));

	m_transform->SetPosition(pos);
}