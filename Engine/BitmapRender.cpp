#include "pch.h"
#include "BitmapRender.h"
#include "Transform.h"
#include "SpriteManager.h"

using namespace DirectX;



bool BitmapRender3D::Extension(const string& bitmapKey, const string& ext /*= ".json"*/) const
{
	if (bitmapKey.length() >= ext.length())
	{
		return (bitmapKey.compare(bitmapKey.length() - ext.length(), ext.length(), ext) == 0);
	}
	return false;
}

void BitmapRender3D::CreateQuadBuffers()  //Device가 너무 깊은 곳에 관여하는게 아닐까
{
	float halfWidth = m_width / 2.0f;
	float halfHeight = m_height / 2.0f;

	SpriteVertex quadVertex[] = {
		{ { -halfWidth, -halfHeight, 0.0f }, { 0.0f, 1.0f } }, // Bottom-left
		{ { -halfWidth,  halfHeight, 0.0f }, { 0.0f, 0.0f } }, // Top-left
		{ {  halfWidth,  halfHeight, 0.0f }, { 1.0f, 0.0f } }, // Top-right
		{ {  halfWidth, -halfHeight, 0.0f }, { 1.0f, 1.0f } }  // Bottom-right
	};

	// Vertex buffer 생성
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.ByteWidth = sizeof(quadVertex);
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE; // 데이터는 생성 후 변하지 않으므로 IMMUTABLE
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vbData = { quadVertex };

	HRESULT hr = m_pDevice->CreateBuffer(&vbDesc, &vbData, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to create Vertex Buffer for BitmapRender3D.\n");
	}

	WORD index[] = {
		0,1,2, // 첫번째 삼각형
		0,2,3  // 두번째 삼각형	
	}; 

	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.ByteWidth = sizeof(index);
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE; // 데이터는 생성 후 변하지 않으므로 IMMUTABLE
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA ibData = { index };

	hr = m_pDevice->CreateBuffer(&ibDesc, &ibData, m_indexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to create Index Buffer for BitmapRender3D.\n");
	}

}

XMMATRIX BitmapRender3D::GetWorldMatrix()
{
	
	XMMATRIX scale = XMMatrixScalingFromVector(m_transform->GetScale());
	XMMATRIX rotation = XMMatrixRotationZ(m_transform->GetRotation()); // float OK
	XMMATRIX translation = XMMatrixTranslationFromVector(m_transform->GetPosition());

	return scale * rotation * translation;
	
}




void BitmapRender3D::SetSize(float width, float height)
{
	if (m_width != width || m_height != height) {
		m_width = width;
		m_height = height;
		// 크기가 변경되면 버퍼를 재생성
		CreateQuadBuffers();
	}
}

// --------------------------------------------------3D

//생성자
BitmapRender3D::BitmapRender3D(const string& spriteKey, float width, float height) : m_width(width), m_height(height)
{
	

	SpriteManager& spriteManager = SpriteManager::GetInstance();
	m_pDevice = SpriteManager::GetInstance().GetDevice();
	if (!m_pDevice) {
		throw std::runtime_error("BitmapRender3D: D3D Device is null.");
	}
	// SpriteManager에 해당 키가 애니메이션 클립을 가지고 있는지 확인
	if (spriteManager.IsAnimatedSprite(spriteKey))
	{
		std::cout << "애니" << endl;
		// 애니메이션 클립 맵을 가져와 SpriteAnimator에 설정
		m_animator.SetClips(&spriteManager.GetAnimationClips(spriteKey));
		// 기본 클립을 설정
		m_isAnimated = true;
	}
	else
	{
		std::cout << "일반" << endl;
		// 일반 이미지 텍스처 SRV를 가져옴
		m_textureSRV = spriteManager.GetTextureSRV(spriteKey);
		if (!m_textureSRV) {
			OutputDebugStringA(("ERROR: BitmapRender3D: Failed to get texture SRV for key: " + spriteKey + "\n").c_str());
			// 에러 처리: 기본 텍스처 설정 또는 예외 발생
		}
		m_isAnimated = false;
	}

	// 고유한 쿼드 버퍼 생성
	CreateQuadBuffers();
}



void BitmapRender3D::Update(double deltaTime)
{
	if (m_isAnimated)
	{
		m_animator.Update(static_cast<float>(deltaTime) * 1000.0f);
	}

}

void BitmapRender3D::Render(Renderer& renderer)
{
	if (!m_isActive || !m_transform) { return; }

	ID3D11ShaderResourceView* currentSRV = nullptr;
	RECT currentSourceRect = {}; // 텍스처 아틀라스 내의 원본 사각형 영역

	if (m_isAnimated)
	{
		const AnimationClip* currentClip = m_animator.GetCurrentClip();
		if (currentClip)
		{
			currentSRV = currentClip->GetTextureSRV();
			currentSourceRect = m_animator.GetCurrentFrame().sourceRect;
		}
	}
	else
	{
		 currentSRV = m_textureSRV.Get();
		 ComPtr<ID3D11Resource> resource;
		 currentSRV->GetResource(resource.GetAddressOf());

		 ComPtr<ID3D11Texture2D> texture2D;
		 resource.As(&texture2D);

		 D3D11_TEXTURE2D_DESC desc;
		 texture2D->GetDesc(&desc);

		 currentSourceRect = { 0, 0, (LONG)desc.Width, (LONG)desc.Height };
	}

	if (!currentSRV || !m_vertexBuffer || !m_indexBuffer) {
		cout << "ERROR: BitmapRender3D: Missing resources for rendering." << endl;
		return;
	}

	XMMATRIX worldMatrix = GetWorldMatrix();

	renderer.DrawBitmap3D(
		m_vertexBuffer.Get(),   // 버텍스 버퍼
		m_indexBuffer.Get(),    // 인덱스 버퍼
		currentSRV,             // 현재 텍스처 SRV
		worldMatrix,            // 월드 행렬
		&currentSourceRect      // 텍스처 내 원본 사각형 (애니메이션이 아닐 땐 전체 텍스처)
	);
		
}
