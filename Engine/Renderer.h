#pragma once

#include <string>
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h> // XMMATRIX ���
#include <d2d1_3.h>      // ���� D2D ���� ��� ����
#include <dwrite.h>      // ���� DWrite ���� ��� ����
#include <wincodec.h>    // ���� WIC ���� ��� ����
#include <memory>        // std::make_unique ���

// ShaderManager�� ���ǵ� ��� ���� (���� �ڵ忡 ShaderManager�� �ִٰ� ����)
#include "ShaderManager.h" 

// BitmapRender3D���� ����ϴ� SpriteVertex ����ü�� Renderer.h���� �ʿ�
struct SpriteVertex
{
	DirectX::XMFLOAT3 position; // 3D ��ġ (x, y, z)
	DirectX::XMFLOAT2 texcoord; // �ؽ�ó ��ǥ (u, v)
};

// ���̴��� ���޵� ��� ���� ����ü��
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
	float padding[2]; // 16����Ʈ ������ ���� �е�
};


using namespace Microsoft::WRL;

class Renderer
{
private:
	// --- ���� D2D/D3D ��� ���� ---
	HWND m_hwnd = nullptr;
	UINT m_screenWidth = 0;
	UINT m_screenHeight = 0;

	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pd3dContext;
	ComPtr<IDXGISwapChain1> m_pswapChain; // IDXGISwapChain1 ��� ��

	ComPtr<ID2D1Device6> m_pd2dDevice;
	ComPtr<ID2D1DeviceContext6> m_pd2dContext;

	ComPtr<ID3D11RenderTargetView> m_pd3dRenderTV; // D3D ����ۿ� RTV
	ComPtr<ID3D11Texture2D> m_renderTargetTex;     // D2D�� �׸� Off-screen �ؽ�ó
	ComPtr<ID3D11RenderTargetView> m_offScreenTargetView; // D2D Off-screen �ؽ�ó�� RTV
	ComPtr<ID3D11ShaderResourceView> m_renderTargetSRV; // D2D Off-screen �ؽ�ó�� SRV
	ComPtr<ID2D1Bitmap1> m_ptargetBitmap;          // D2D Off-screen �ؽ�ó ���� ��Ʈ��

	ComPtr<ID2D1SolidColorBrush> m_pbrush;
	ComPtr<ID2D1SolidColorBrush> m_ptextBrush;
	ComPtr<IDWriteTextFormat> m_ptextFormat;
	ComPtr<IWICImagingFactory> m_pwicFactory;

	// Ǯ��ũ�� ���� ���� (����Ʈ ���μ��̿�)
	ComPtr<ID3D11Buffer> m_fullScreenVB;
	ComPtr<ID3D11VertexShader> m_VertexShader; // PassThrough_VS
	ComPtr<ID3D11PixelShader> m_PixelShader;   // PassThrough_PS
	ComPtr<ID3D11InputLayout> m_InputLayout;   // PassThrough ���̴��� InputLayout

	ComPtr<ID3D11SamplerState> m_samplerState; // PostProcessing�� ���
	//ComPtr<ID3D11RasterizerState> m_pRasterizerState;

	std::unique_ptr<ShaderManager> m_shaderManager;
	std::string m_postProcessShaderName = "PassThrough"; // �⺻ ����Ʈ ���μ��� ���̴� �̸�

	// --- ���� �߰��� D3D ��������Ʈ ������ ���� ��� ---
	ComPtr<ID3D11Buffer> m_pObjectTransformCBuffer; // ����-��-�������� ���
	ComPtr<ID3D11Buffer> m_pTextureAtlasCBuffer;    // �ؽ�ó ��Ʋ�� ����
	ComPtr<ID3D11SamplerState> m_pSpriteSamplerState; // ��������Ʈ ���� ���÷� (�ٸ� ���͸� ��)

	// 2D �������� ���� ��/�������� ��� (��ȭ���� �����Ƿ� �̸� ���)
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMMATRIX m_projectionMatrix;

public:
	Renderer() = default;
	~Renderer();

	void Initialize(HWND hwnd);
	void Uninitialize();
	void Resize(UINT width, UINT height);

	// ������ �帧 ���� �Լ�
	void RenderBegin();
	void PostProcessing(const ShaderSet& shaderSet); // ���� ����
	void RenderEnd();
	void Present();

	// D3D ����̽�/���ؽ�Ʈ Getter (SpriteManager �ʱ�ȭ�� �ʿ�)
	ID3D11Device* GetDevice() const { return m_pd3dDevice.Get(); }
	ID3D11DeviceContext* GetDeviceContext() const { return m_pd3dContext.Get(); }

	// ���� D2D ����� �Լ��� ����
	//void DrawCircle(float x, float y, float radius, const D2D1::ColorF& color);
	void DrawRect(float left, float top, float right, float bottom, const D2D1::ColorF& color);
	//void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest);
	//void DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity);
	//void DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color);
	//void CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap); // ��� ���� ��Ȯ��, SpriteManager�� ��ü ����

	// --- D3D ��� ��������Ʈ ������ �Լ� (�ϼ��� �κ�) ---
	void DrawBitmap3D(
		ID3D11Buffer* pVertexBuffer,
		ID3D11Buffer* pIndexBuffer,
		ID3D11ShaderResourceView* pTextureSRV,
		const DirectX::XMMATRIX& worldMatrix,
		const RECT* pSourceRect
	);
	void SetPostProcessingMode(const string& shaderName) { m_postProcessShaderName = shaderName; }
private:
	// ���� �ʱ�ȭ �Լ��� ����
	void CreateDeviceAndSwapChain(HWND hwnd);
	void CreateWriteResource(); //
	void CreateFullScrennQuad();//
	void CreateShaderRenderTargets(); // D2D off-screen �ؽ�ó ������ �ʿ�
	void ReleaseRenderTargets();      // D2D off-screen �ؽ�ó ������ �ʿ�
	void InitializeShader(HWND hwnd); // PassThrough ���̴� �ʱ�ȭ�� ���

	// --- ���� �߰��� �ʱ�ȭ �Լ��� (D3D ��������Ʈ ��������) ---
	
	HRESULT CreateSpriteConstantBuffers();
	HRESULT CreateSpriteSamplerState();
	void SetupSpriteCameraMatrices(UINT width, UINT height); // 2D ���� ���� ī�޶� ����
};