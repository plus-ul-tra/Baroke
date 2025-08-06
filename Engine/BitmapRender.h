#pragma once
#include "BaseRender.h"
#include "SpriteManager.h"

#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
using namespace Microsoft::WRL;

//struct SpriteVertex
//{
//	DirectX::XMFLOAT3 position; // 3D ��ġ (x, y, z)
//	DirectX::XMFLOAT2 texcoord; // �ؽ�ó ��ǥ (u, v)
//};

class BitmapRender3D : public BaseRender {
private:
	string m_shaderType = "DefaultShader"; // ���̴� Ÿ��
	float m_width;
	float m_height;

	SpriteAnimator m_animator; //
	bool m_isAnimated = false;
	float m_shaderTimeElapsed = 0.0f; // ���̴� ȿ�� �ð�
	float m_shaderEffectDuration = 5.0f; // ���̴� ȿ�� ���� �ð�

	ComPtr<ID3D11Buffer> m_vertexBuffer; // quad vertex
	ComPtr<ID3D11Buffer> m_indexBuffer;
	// m_textureSRV�� ���� �̹��� ��������Ʈ��.
   // �ִϸ��̼� ��������Ʈ�� ���, m_animator�� �����Ӻ� SRV�� �����ؾ�...

	ComPtr<ID3D11ShaderResourceView> m_textureSRV; // ����
	ComPtr<ID3D11ShaderResourceView> m_nextTextureSRV; // ���� �ؽ�ó SRV (�ִϸ��̼ǿ�)

	ID3D11Device* m_pDevice = nullptr;  // �Ϲ� ������

	bool Extension(const string& bitmapKey, const string& ext = ".json") const;

	//shader Manager���� ���� ���� �� ���� ��
	//void Initialize3DResource(ID3D11Device* pDevice, const std::string& bitmapkey,float width, float height);
	void CreateQuadBuffers();

public:
	BitmapRender3D(const string& spriteKey, float width, float height);
	~BitmapRender3D() = default;

	void Update(double deltaTime) override;
	void Render(Renderer& renderer) override;

	//void SetAnimator(const string& animationKey) { m_animator.SetClips(&SpriteManager::GetInstance().GetAnimationClips(animationKey)); }
	//SpriteAnimator& GetAnimator() { return m_animator; }

	XMMATRIX GetWorldMatrix();

	void SetSize(float width, float height);
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }


	void ChangeTexture(const string& nextTextureKey, float duration = 1.0f) {
		m_shaderTimeElapsed = 0.0f; // ���̴� ȿ�� �ð� �ʱ�ȭ
		m_shaderEffectDuration = duration; // ���̴� ȿ�� ���� �ð� ����
		std::cout << "Changing Board Texture" << std::endl;
		SpriteManager& spriteManager = SpriteManager::GetInstance();
		m_nextTextureSRV = spriteManager.GetTextureSRV(nextTextureKey); //�ؽ�Ʈ�� �ΰ�
		// ���̴�ȿ�� �ְ�
		SetShaderType("Checker");
		if (m_shaderTimeElapsed > m_shaderEffectDuration)
		{
			m_textureSRV = m_nextTextureSRV; // ���� �ؽ�ó�� ����
			SetShaderType("DefaultShader");
		}
	}

	void SetShaderType(const string& type) { m_shaderType = type; } // ���̴� Ÿ�� ����
	const string& GetShaderType() const { return m_shaderType; } // ���� ���̴� Ÿ�� ��ȯ
};
