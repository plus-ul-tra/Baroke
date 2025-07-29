#pragma once

#include <d3d11.h >                 // Direct3D 11
#include <dxgi1_6.h>                 // DXGI 1.6
#include <d2d1_3.h>                  // Direct2D 1.3
#include <d2d1_3helper.h>           
#include <dwrite_3.h>                // 텍스트 엔진
#include <wincodec.h>                // WIC
#include <d2d1helper.h>
#include <wrl/client.h>
#include <d3dcompiler.h>     //ReadFileToBlob 사용
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texcoord;
};

struct TimeShaderConstants
{
	float Time;
	float Padding[3]; // float 3개 = 12바이트. float(4) + Padding(12) = 16바이트
};


using namespace Microsoft::WRL;
// UI, object
// D3D11 메인 렌더 + Direct2D 오버레이
class Renderer {

private:
	HWND m_hwnd = NULL;

	//factory 8->7, device,Context 7->6 로
	
	ComPtr<ID2D1Factory>			m_pd2dFactory;
	ComPtr<IWICImagingFactory>		m_pwicFactory;

	ComPtr<ID3D11Device>			m_pd3dDevice;
	ComPtr<ID2D1Device7>			m_pd2dDevice;
	ComPtr<ID3D11RenderTargetView>  m_pd3dRenderTV;

	ComPtr<IDXGISwapChain1>         m_pswapChain;
	ComPtr<ID2D1DeviceContext7>     m_pd2dContext;
	ComPtr<ID3D11DeviceContext>     m_pd3dContext;

	ComPtr<ID2D1Bitmap1>            m_ptargetBitmap;

	ComPtr<ID2D1SolidColorBrush>    m_pbrush;
	ComPtr<ID2D1SolidColorBrush>    m_ptextBrush;
	ComPtr<IDWriteTextFormat>       m_ptextFormat;

	// new
	ComPtr<ID3D11Buffer>			   m_fullScreenVB;
	ComPtr<ID3D11Texture2D>			   m_renderTargetTex;
	ComPtr<ID3D11RenderTargetView>     m_offScreenTargetView; 
	ComPtr<ID3D11ShaderResourceView>   m_renderTargetSRV;

	ComPtr<ID3D11VertexShader>      m_VertexShader;
	ComPtr<ID3D11PixelShader>       m_PixelShader;
	ComPtr<ID3D11InputLayout>       m_InputLayout;
	ComPtr<ID3D11SamplerState>      m_samplerState;


	UINT m_screenWidth;
	UINT m_screenHeight;

	void CreateDeviceAndSwapChain(HWND hwnd);

	void CreateShaderRenderTargets();
	void CreateWriteResource();
	void CreateFullScrennQuad();
	void ReleaseRenderTargets();

public:
	Renderer() = default;
	~Renderer() { Uninitialize(); }

	void Initialize(HWND hwnd);
	void InitializeShader(HWND hwnd);
	void Uninitialize();
	void Resize(UINT width, UINT height);


	void DrawCircle(float x1, float y1, float radius, const D2D1::ColorF& color);
	void DrawRect(float left, float top, float right, float bottom, const D2D1::ColorF& color);

	//텍스쳐
	void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest);
	void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity = 1.0f);

	void DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color);

	//추후적용
	//void SetTransform(const D2D1_MATRIX_3X2_F matrix);

	void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap);
	
	//getter
	ID3D11Device* GetD3DDevice() const { return m_pd3dDevice.Get(); }
	ID3D11DeviceContext* GetD3DContext() const { return m_pd3dContext.Get(); }
	ID3D11RenderTargetView* GetD3DRenderTargetView() const { return m_pd3dRenderTV.Get(); }

	void RenderBegin();
	void RenderEnd();

	void Present();
};