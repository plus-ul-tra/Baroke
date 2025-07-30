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

// ShaderManager가 정의된 헤더 포함 (현재 코드에 ShaderManager가 있다고 가정)
#include "ShaderManager.h" 

// BitmapRender3D에서 사용하는 SpriteVertex 구조체는 Renderer.h에도 필요
struct SpriteVertex
{
	DirectX::XMFLOAT3 position; // 3D 위치 (x, y, z)
	DirectX::XMFLOAT2 texcoord; // 텍스처 좌표 (u, v)
};

// 쉐이더로 전달될 상수 버퍼 구조체들
struct alignas(16) ObjectTransformCBuffer
{
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Projection;
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
	// --- 기존 D2D/D3D 멤버 유지 ---
	HWND m_hwnd = nullptr;
	UINT m_screenWidth = 0;
	UINT m_screenHeight = 0;

	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pd3dContext;
	ComPtr<IDXGISwapChain1> m_pswapChain; // IDXGISwapChain1 사용 중

	ComPtr<ID2D1Device6> m_pd2dDevice;
	ComPtr<ID2D1DeviceContext6> m_pd2dContext;

	ComPtr<ID3D11RenderTargetView> m_pd3dRenderTV; // D3D 백버퍼용 RTV
	ComPtr<ID3D11Texture2D> m_renderTargetTex;     // D2D가 그릴 Off-screen 텍스처
	ComPtr<ID3D11RenderTargetView> m_offScreenTargetView; // D2D Off-screen 텍스처용 RTV
	ComPtr<ID3D11ShaderResourceView> m_renderTargetSRV; // D2D Off-screen 텍스처용 SRV
	ComPtr<ID2D1Bitmap1> m_ptargetBitmap;          // D2D Off-screen 텍스처 래핑 비트맵

	ComPtr<ID2D1SolidColorBrush> m_pbrush;
	ComPtr<ID2D1SolidColorBrush> m_ptextBrush;
	ComPtr<IDWriteTextFormat> m_ptextFormat;
	ComPtr<IWICImagingFactory> m_pwicFactory;

	// 풀스크린 쿼드 관련 (포스트 프로세싱용)
	ComPtr<ID3D11Buffer> m_fullScreenVB;
	ComPtr<ID3D11VertexShader> m_VertexShader; // PassThrough_VS
	ComPtr<ID3D11PixelShader> m_PixelShader;   // PassThrough_PS
	ComPtr<ID3D11InputLayout> m_InputLayout;   // PassThrough 쉐이더용 InputLayout

	ComPtr<ID3D11SamplerState> m_samplerState; // PostProcessing에 사용
	//ComPtr<ID3D11RasterizerState> m_pRasterizerState;

	std::unique_ptr<ShaderManager> m_shaderManager;
	std::string m_postProcessShaderName = "PassThrough"; // 기본 포스트 프로세싱 쉐이더 이름

	// --- 새로 추가될 D3D 스프라이트 렌더링 관련 멤버 ---
	ComPtr<ID3D11Buffer> m_pObjectTransformCBuffer; // 월드-뷰-프로젝션 행렬
	ComPtr<ID3D11Buffer> m_pTextureAtlasCBuffer;    // 텍스처 아틀라스 정보
	ComPtr<ID3D11SamplerState> m_pSpriteSamplerState; // 스프라이트 전용 샘플러 (다른 필터링 등)

	// 2D 렌더링을 위한 뷰/프로젝션 행렬 (변화하지 않으므로 미리 계산)
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;

public:
	Renderer() = default;
	~Renderer();

	void Initialize(HWND hwnd);
	void Uninitialize();
	void Resize(UINT width, UINT height);

	// 렌더링 흐름 제어 함수
	void RenderBegin();
	void PostProcessing(const ShaderSet& shaderSet); // 기존 유지
	void RenderEnd();
	void Present();

	// D3D 디바이스/컨텍스트 Getter (SpriteManager 초기화에 필요)
	ID3D11Device* GetDevice() const { return m_pd3dDevice.Get(); }
	ID3D11DeviceContext* GetDeviceContext() const { return m_pd3dContext.Get(); }

	// 기존 D2D 드로잉 함수들 유지
	//void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color);
	void DrawRect(float left, float top, float right, float bottom, const D2D1::ColorF& color);
	//void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest);
	//void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity);
	//void DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color);
	//void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap); // 사용 여부 불확실, SpriteManager로 대체 권장

	// --- D3D 기반 스프라이트 렌더링 함수 (완성할 부분) ---
	void DrawBitmap3D(
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		ID3D11ShaderResourceView* pTextureSRV,
		const DirectX::XMMATRIX& worldMatrix,
		const RECT* pSourceRect
	);
	void SetPostProcessingMode(const string& shaderName) { m_postProcessShaderName = shaderName; }
private:
	// 기존 초기화 함수들 유지
	void CreateDeviceAndSwapChain(HWND hwnd);
	void CreateWriteResource(); //
	void CreateFullScrennQuad();//
	void CreateShaderRenderTargets(); // D2D off-screen 텍스처 관리에 필요
	void ReleaseRenderTargets();      // D2D off-screen 텍스처 해제에 필요
	void InitializeShader(HWND hwnd); // PassThrough 쉐이더 초기화에 사용

	// --- 새로 추가될 초기화 함수들 (D3D 스프라이트 렌더링용) ---
	
	HRESULT CreateSpriteConstantBuffers();
	HRESULT CreateSpriteSamplerState();
	void SetupSpriteCameraMatrices(UINT width, UINT height); // 2D 직교 투영 카메라 설정
};