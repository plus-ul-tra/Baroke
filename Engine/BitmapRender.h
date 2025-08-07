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
//	DirectX::XMFLOAT3 position; // 3D 위치 (x, y, z)
//	DirectX::XMFLOAT2 texcoord; // 텍스처 좌표 (u, v)
//};

class BitmapRender3D : public BaseRender {
private:
	string m_shaderType = "DefaultShader"; // 쉐이더 타입
	float m_width;
	float m_height;

	SpriteAnimator m_animator; //
	bool m_isAnimated = false;
	float m_shaderTimeElapsed = 1.0f; // 쉐이더 효과 시간
	float m_shaderEffectDuration = 1.0f; // 쉐이더 효과 지속 시간
	bool m_isShaderEffectActive = false; // 쉐이더 효과 활성화 여부

	ComPtr<ID3D11Buffer> m_vertexBuffer; // quad vertex
	ComPtr<ID3D11Buffer> m_indexBuffer;
	// m_textureSRV는 단일 이미지 스프라이트용.
   // 애니메이션 스프라이트의 경우, m_animator가 프레임별 SRV를 제공해야...

	ComPtr<ID3D11ShaderResourceView> m_textureSRV; // 현재
	ComPtr<ID3D11ShaderResourceView> m_nextTextureSRV; // 다음 텍스처 SRV (애니메이션용)

	ID3D11Device* m_pDevice = nullptr;  // 일반 포인터

	Mediator& m_mediator = Mediator::GetInstance(); // Mediator 인스턴스

	bool Extension(const string& bitmapKey, const string& ext = ".json") const;

	//shader Manager에서 위임 받을 수 있을 듯
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


	void ChangeBoardTexture(const string& nextTextureKey, float duration = 1.0f) {
		m_isShaderEffectActive = true; // 쉐이더 효과 활성화
		m_shaderTimeElapsed = 0.0f; // 쉐이더 효과 시간 초기화
		m_shaderEffectDuration = duration; // 쉐이더 효과 지속 시간 설정
		std::cout << "Changing Board Texture" << std::endl;
		SpriteManager& spriteManager = SpriteManager::GetInstance();
		m_nextTextureSRV = spriteManager.GetTextureSRV(nextTextureKey); //넥스트에 두고
		// 쉐이더효과 주고
		SetShaderType("Checker");
	}


	void ChangeBackGroundColor(XMFLOAT4 target) {
		m_mediator.SetTargetColor(target);
	}

	void SetShaderType(const string& type) { m_shaderType = type; } // 쉐이더 타입 설정
	//void SetShaderType(const string& type, float duration = 1.0f) { m_shaderType = type; m_shaderTimeElapsed = 0.0f; m_shaderEffectDuration = duration; }

	//일반 텍스쳐 변경으로 사용가능
	void ChangeTexture(const string& nextTextureKey)
	{
		SpriteManager& spriteManager = SpriteManager::GetInstance();
		m_nextTextureSRV = spriteManager.GetTextureSRV(nextTextureKey);
		m_textureSRV = m_nextTextureSRV;
	}

	

	const string& GetShaderType() const { return m_shaderType; } // 현재 쉐이더 타입 반환
};
