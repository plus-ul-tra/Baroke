#pragma once

#include <string>
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h> // XMMATRIX 사용
#include <d2d1_3.h>      // 기존 D2D 관련 헤더 유지
#include <dwrite.h>      // 기존 DWrite 관련 헤더 유지
#include <wincodec.h>    // 기존 WIC 관련 헤더 유지
#include <memory>        // std::make_unique 사용
#include <unordered_map>
// ShaderManager가 정의된 헤더 포함 (현재 코드에 ShaderManager가 있다고 가정)
#include "ShaderManager.h" 

using namespace DirectX;
// BitmapRender3D에서 사용하는 SpriteVertex 구조체는 Renderer.h에도 필요
struct SpriteVertex
{
	DirectX::XMFLOAT3 position; // 3D 위치 (x, y, z)
	DirectX::XMFLOAT2 texcoord; // 텍스처 좌표 (u, v)
};

// 쉐이더로 전달될 상수 버퍼 구조체들
struct alignas(16) ObjectTransformCBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
};

struct alignas(16) TimeCBuffer {

	float time; // 현재 시간 (초 단위)
	float deltaTime; // 마지막 프레임과의 시간 차이 (초 단위)
	float padding[2]; // 16바이트 정렬을 위한 패딩

};

struct alignas(16) TextureAtlasCBuffer
{
	float textureWidth;
	float textureHeight;
	float sourceRectX;
	float sourceRectY;
	float sourceRectWidth;
	float sourceRectHeight;
	float padding[2]; // 16바이트 정렬을 위한 패딩
};


using namespace Microsoft::WRL;

class Renderer
{
private:
	HWND m_hwnd = nullptr;
	UINT m_screenWidth = 0;
	UINT m_screenHeight = 0;

	ComPtr<ID3D11Device>              m_pd3dDevice;
	ComPtr<ID3D11DeviceContext>       m_pd3dContext;
	ComPtr<IDXGISwapChain1>           m_pswapChain; // IDXGISwapChain1 사용 중

	

	ComPtr<ID3D11RenderTargetView>    m_pd3dRenderTV; // D3D 백버퍼용 RTV


	ComPtr<ID3D11Texture2D>           m_renderTargetTex;     // D2D가 그릴 Off-screen 텍스처
	ComPtr<ID3D11RenderTargetView>    m_offScreenRTV; // D2D Off-screen 텍스처용 RTV
	ComPtr<ID3D11ShaderResourceView>  m_renderTargetSRV; // D2D Off-screen 텍스처용 SRV
	ComPtr<ID2D1Bitmap1>              m_ptargetBitmap;          // D2D Off-screen 텍스처 래핑 비트맵  

	// 2D text 용
	ComPtr<ID2D1Device7>              m_pd2dDevice;
	ComPtr<ID2D1DeviceContext7>       m_pd2dContext;
	ComPtr<ID2D1Factory1>             m_pd2dFactory; // D2D 팩토리
	ComPtr<ID2D1SolidColorBrush>      m_pbrush;
	ComPtr<IDWriteFactory>            m_pdwriteFactory; // DWrite 팩토리
	ComPtr<IDWriteTextFormat>         m_ptextFormat;

	ComPtr<IWICImagingFactory>        m_pwicFactory;

	// 풀스크린 쿼드 관련 (포스트 프로세싱용)
	ComPtr<ID3D11Buffer>              m_fullScreenVB;
	ComPtr<ID3D11VertexShader>        m_VertexShader; // PassThrough_VS
	ComPtr<ID3D11PixelShader>         m_PixelShader;   // PassThrough_PS
	ComPtr<ID3D11InputLayout>         m_InputLayout;   // PassThrough 쉐이더용 InputLayout

	ComPtr<ID3D11SamplerState>        m_samplerState; // PostProcessing에 사용

	unique_ptr<ShaderManager>         m_shaderManager;
	string                            m_postProcessShaderName = "PassThrough"; // 기본 포스트 프로세싱 쉐이더 이름


	ComPtr<ID3D11Buffer>              m_pObjectTransformCBuffer; // 월드-뷰-프로젝션 행렬
	ComPtr<ID3D11Buffer>              m_pTextureAtlasCBuffer;
	ComPtr<ID3D11SamplerState>        m_pSpriteSamplerState;
	ComPtr<ID3D11BlendState>          m_blendState;

	//unordered_map<ID3D11Texture2D*, DirectX::XMUINT2> m_textureSizeCache;

	XMMATRIX                          m_viewMatrix;
	XMMATRIX                          m_projectionMatrix;
	ComPtr<ID3D11Buffer>			  m_pTimeCBuffer;
	float 							  m_blackHoleTime = 0.0f; // 블랙홀 효과 시간 / 외 다른 시간 shader 버퍼로 사용
public:
	Renderer() = default;
	~Renderer();

	void Initialize(HWND hwnd);
	void Uninitialize();
	void Resize(UINT width, UINT height);

	// 렌더링 흐름 제어 함수
	void RenderBegin();
	void SetShaderMode(const string& mode);
	void PostProcessing(const ShaderSet& shaderSet); // 기존 유지
	void RenderEnd();
	void Present();

	// D3D 디바이스/컨텍스트 Getter (SpriteManager 초기화에 필요)
	ID3D11Device* GetDevice() const { return m_pd3dDevice.Get(); }
	ID3D11DeviceContext* GetDeviceContext() const { return m_pd3dContext.Get(); }

	void DrawBitmap3D(
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		ID3D11ShaderResourceView* pTextureSRV,
		const DirectX::XMMATRIX& worldMatrix,
		const RECT* pSourceRect
	);
	void SetPostProcessingMode(const string& shaderName) {
		if (m_postProcessShaderName != shaderName) {
			// 모드 변경 감지
			if (shaderName == "BlackHole") {
				m_blackHoleTime = 0.0f; // 초기화
			}
		}
		m_postProcessShaderName = shaderName;
	}
private:

	void CreateDeviceAndSwapChain(HWND hwnd);

	void CreateWriteResource(); //
	void CreateFullScrennQuad();//
	void CreateShaderRenderTargets();
	void CreateTimeCBuffer();
	void ReleaseRenderTargets();
	void InitializeShader(HWND hwnd); // PassThrough 쉐이더 초기화에 사용



	HRESULT CreateSpriteConstantBuffers();
	HRESULT CreateSpriteSamplerState();
	HRESULT CreateBlendState();
	void SetupSpriteCameraMatrices(UINT width, UINT height); // 2D 직교 투영 카메라 설정
};