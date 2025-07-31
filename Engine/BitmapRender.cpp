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

void BitmapRender3D::CreateQuadBuffers()  //Device�� �ʹ� ���� ���� �����ϴ°� �ƴұ�
{
	float halfWidth = m_width / 2.0f;
	float halfHeight = m_height / 2.0f;

	SpriteVertex quadVertex[] = {
		{ { -halfWidth, -halfHeight, 0.0f }, { 0.0f, 1.0f } }, // Bottom-left
		{ { -halfWidth,  halfHeight, 0.0f }, { 0.0f, 0.0f } }, // Top-left
		{ {  halfWidth,  halfHeight, 0.0f }, { 1.0f, 0.0f } }, // Top-right
		{ {  halfWidth, -halfHeight, 0.0f }, { 1.0f, 1.0f } }  // Bottom-right
	};

	// Vertex buffer ����
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.ByteWidth = sizeof(quadVertex);
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE; // �����ʹ� ���� �� ������ �����Ƿ� IMMUTABLE
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vbData = { quadVertex };

	HRESULT hr = m_pDevice->CreateBuffer(&vbDesc, &vbData, m_vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to create Vertex Buffer for BitmapRender3D.\n");
	}

	WORD index[] = {
		0,1,2, // ù��° �ﰢ��
		0,2,3  // �ι�° �ﰢ��	
	}; 

	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.ByteWidth = sizeof(index);
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE; // �����ʹ� ���� �� ������ �����Ƿ� IMMUTABLE
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
		// ũ�Ⱑ ����Ǹ� ���۸� �����
		CreateQuadBuffers();
	}
}

// --------------------------------------------------3D

//������
BitmapRender3D::BitmapRender3D(const string& spriteKey, float width, float height) : m_width(width), m_height(height)
{
	

	SpriteManager& spriteManager = SpriteManager::GetInstance();
	m_pDevice = SpriteManager::GetInstance().GetDevice();
	if (!m_pDevice) {
		throw std::runtime_error("BitmapRender3D: D3D Device is null.");
	}
	// SpriteManager�� �ش� Ű�� �ִϸ��̼� Ŭ���� ������ �ִ��� Ȯ��
	if (spriteManager.IsAnimatedSprite(spriteKey))
	{
		std::cout << "�ִ�" << endl;
		// �ִϸ��̼� Ŭ�� ���� ������ SpriteAnimator�� ����
		m_animator.SetClips(&spriteManager.GetAnimationClips(spriteKey));
		// �⺻ Ŭ���� ����
		m_isAnimated = true;
	}
	else
	{
		std::cout << "�Ϲ�" << endl;
		// �Ϲ� �̹��� �ؽ�ó SRV�� ������
		m_textureSRV = spriteManager.GetTextureSRV(spriteKey);
		if (!m_textureSRV) {
			OutputDebugStringA(("ERROR: BitmapRender3D: Failed to get texture SRV for key: " + spriteKey + "\n").c_str());
			// ���� ó��: �⺻ �ؽ�ó ���� �Ǵ� ���� �߻�
		}
		m_isAnimated = false;
	}

	// ������ ���� ���� ����
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
	RECT currentSourceRect = {}; // �ؽ�ó ��Ʋ�� ���� ���� �簢�� ����

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
		m_vertexBuffer.Get(),   // ���ؽ� ����
		m_indexBuffer.Get(),    // �ε��� ����
		currentSRV,             // ���� �ؽ�ó SRV
		worldMatrix,            // ���� ���
		&currentSourceRect      // �ؽ�ó �� ���� �簢�� (�ִϸ��̼��� �ƴ� �� ��ü �ؽ�ó)
	);
		
}
