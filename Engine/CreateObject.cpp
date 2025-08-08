#include "pch.h"
#include "CreateObject.h"

NewObject::NewObject(float posX, float posY, float width, float height, float rotation, const std::string& bitmapKey, float speed, XMVECTOR destination, int order)
{
	m_transform = AddComponent<Transform>();
	m_bitmapRender = AddComponent<BitmapRender3D>(bitmapKey, width, height);
	m_bitmapRender->SetOrder(order);
	m_bitmapRender->SetActive(true);

	m_transform->SetPosition(DirectX::XMVectorSet(posX, posY, 0.0f, 1.0f));
	m_transform->SetScale(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	m_transform->SetRotation(rotation);

	m_destination = destination;
	m_speedPerSec = speed;
}

void NewObject::Update(double deltaTime)
{
	if (DirectX::XMVector3Equal(m_destination, DirectX::XMVectorZero())) return;

	XMVECTOR currentPos = m_transform->GetPosition();

	XMVECTOR targetPos = m_destination;

	XMVECTOR moveDir = DirectX::XMVectorSubtract(targetPos, currentPos);
	float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(moveDir));

	float moveAmount = m_speedPerSec * static_cast<float>(deltaTime);
	if (moveAmount > distance) moveAmount = distance;

	moveDir = DirectX::XMVector3Normalize(moveDir);
	moveDir = DirectX::XMVectorScale(moveDir, moveAmount);

	XMVECTOR newPos = DirectX::XMVectorAdd(currentPos, moveDir);
	m_transform->SetPosition(newPos);

	if (DirectX::XMVector3Equal(newPos, targetPos)) m_destination = { 0.0f, 0.0f, 0.0f, 1.0f };
}

void CreateObject::CreateObjectsOutOfScreen(vector<unique_ptr<Object>>& objects, string imageKey, float screenWidth, float screenHeight, float width, float height, int count, float speed, direction exclusiveDirection)
{
	random_device rd;
	mt19937 gen(rd());

	uniform_real_distribution<float> distX(-(screenWidth / 2), screenWidth / 2);
	uniform_real_distribution<float> distY(-(screenHeight / 2), screenHeight / 2);
	uniform_real_distribution<float> distDirection(-(screenWidth + screenHeight), screenWidth + screenHeight);
	uniform_real_distribution<float> rotationDist(0.0f, 60.0f);

	//ComPtr<ID3D11ShaderResourceView> texture = SpriteManager::GetInstance().LoadTextureSRV(imageKey);
	//ComPtr<ID3D11Resource> texture2D;
	//texture->GetResource(texture2D.GetAddressOf());
	//float width = 0.0f, height = 0.0f;
	//if (texture2D)
	//{
	//	D3D11_TEXTURE2D_DESC desc;
	//	static_cast<ID3D11Texture2D*>(texture2D.Get())->GetDesc(&desc);
	//	width = static_cast<float>(desc.Width);
	//	height = static_cast<float>(desc.Height);
	//}

	if (exclusiveDirection)
	{
		for (int i = 0; i < count; ++i)
		{
			float posX = distX(gen);
			float posY = distY(gen);
			float rotation = rotationDist(gen);
			if (exclusiveDirection == direction::left)
			{
				objects.emplace_back(make_unique<NewObject>(-(screenWidth / 2 + width), posY, width, height, rotation, imageKey, speed, XMVectorSet(-screenWidth / 2, posY, 0.0f, 1.0f)));
			}
			else if (exclusiveDirection == direction::right)
			{
				objects.emplace_back(make_unique<NewObject>(screenWidth / 2 + width, posY, width, height, rotation, imageKey, speed, XMVectorSet(screenWidth / 2, posY, 0.0f, 1.0f)));
			}
			else if (exclusiveDirection == direction::up)
			{
				objects.emplace_back(make_unique<NewObject>(posX, screenHeight / 2 + height, width, height, rotation, imageKey, speed, XMVectorSet(posX, screenHeight / 2, 0.0f, 1.0f)));
			}
			else if (exclusiveDirection == direction::down)
			{
				objects.emplace_back(make_unique<NewObject>(posX, -(screenHeight / 2 + height), width, height, rotation, imageKey, speed, XMVectorSet(posX, -screenHeight / 2, 0.0f, 1.0f)));
			}
		}
		return;
	}
	for (int i = 0; i < count; ++i)
	{
		float dir = distDirection(gen);
		float posX = distX(gen);
		float posY = distY(gen);
		float rotation = rotationDist(gen);

		if (dir < 0)
		{
			if (dir < -screenWidth)
			{
				objects.emplace_back(make_unique<NewObject>(-(screenWidth / 2 + width), posY, width, height, rotationDist(gen), imageKey, speed, XMVectorSet(-screenWidth / 2, posY, 0.0f, 1.0f)));
			}
			else
			{
				objects.emplace_back(make_unique<NewObject>(posX, -(screenHeight / 2 + height), width, height, rotationDist(gen), imageKey, speed, XMVectorSet(posX, -screenHeight / 2, 0.0f, 1.0f)));
			}
		}
		else
		{
			if (dir > screenWidth)
			{
				objects.emplace_back(make_unique<NewObject>(screenWidth / 2 + width, posY, width, height, rotationDist(gen), imageKey, speed, XMVectorSet(screenWidth / 2, posY, 0.0f, 1.0f)));
			}
			else
			{
				objects.emplace_back(make_unique<NewObject>(posX, screenHeight / 2 + height, width, height, rotationDist(gen), imageKey, speed, XMVectorSet(posX, screenHeight / 2, 0.0f, 1.0f)));
			}
		}
	}
}
