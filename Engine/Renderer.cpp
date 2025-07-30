#include "pch.h"
#include "Renderer.h"
#include "SpriteManager.h" // SpriteManager 초기화 호출을 위해 포함
//#include "Utils.h" // RETURN_IF_FAILED, OutputDebugStringA (추가 필요)

#include <d3dcompiler.h> // 쉐이더 컴파일 (D3DCompile)
#include <DirectXMath.h> // DirectX Math

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") // D3DCompile 함수를 위해 링크

using namespace DirectX;

// D3D11CreateDeviceWithSwapChain 이 아니라 D3D11CreateDevice로 시작하는 기존 방식 유지

Renderer::~Renderer()
{
	Uninitialize();
}

void Renderer::Initialize(HWND hwnd)
{
	m_hwnd = hwnd;
	// 기존 D3D11 Device, DXGI SwapChain, D2D Device/Context 생성 로직 유지
	CreateDeviceAndSwapChain(hwnd);

	// 기존 ShaderManager 초기화 유지
	// ShaderManager가 Renderer 내부에 있는 m_pd3dDevice를 사용해야 함
	m_shaderManager = std::make_unique<ShaderManager>(m_pd3dDevice.Get());
	// ShaderManager에서 SpriteShader_VS.hlsl, SpriteShader_PS.hlsl을 로드하도록 설정해야 합니다.
	// 예: m_shaderManager->LoadShader("SpriteShader", L"Shaders/SpriteShader_VS.cso", L"Shaders/SpriteShader_PS.cso", layout_for_sprite_shader);

	// 기존 Render Targets, FullScreen Quad, WriteResource 생성 유지
	CreateShaderRenderTargets();
	CreateFullScrennQuad();
	CreateWriteResource();

	// 기존 PassThrough 쉐이더 초기화 유지
	InitializeShader(hwnd);

	// 기존 WIC Factory 및 샘플러 상태 생성 유지
	ComPtr<IWICImagingFactory> wicFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory));
	//DX::ThrowIfFailed(hr); // 예외 처리 생략됨
	m_pwicFactory = wicFactory;

	// (기존) Post-processing용 샘플러 상태
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_samplerState);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Post-processing Sampler State! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// --- 새로 추가될 D3D 스프라이트 렌더링 관련 초기화 ---
	hr = CreateSpriteConstantBuffers();
	if (FAILED(hr)) return; // 오류 시 조기 리턴

	hr = CreateSpriteSamplerState();
	if (FAILED(hr)) return;

	SetupSpriteCameraMatrices(m_screenWidth, m_screenHeight);

	// SpriteManager 초기화 (이제 Renderer가 D3D Device/Context를 제공)
	// 기존 초기화 위치에 따라 조정. 여기서는 Renderer 초기화의 마지막에 호출
	SpriteManager::GetInstance().Initialize(m_pd3dDevice.Get(), m_pd3dContext.Get());

	OutputDebugStringA("INFO: Renderer initialized successfully.\n");
}

void Renderer::Uninitialize()
{
	// SpriteManager 리소스 먼저 해제 (D3D 리소스를 사용하므로)
	SpriteManager::GetInstance().Release();

	// 새로 추가된 D3D 스프라이트 렌더링 관련 리소스 해제
	m_pSpriteSamplerState.Reset();
	m_pTextureAtlasCBuffer.Reset();
	m_pObjectTransformCBuffer.Reset();

	// 기존 D2D/D3D 리소스 해제
	m_shaderManager.reset();
	ReleaseRenderTargets(); // 이 안에 D2D 관련 Reset 포함
	m_pwicFactory.Reset();
	m_ptargetBitmap.Reset();
	m_pbrush.Reset();
	m_ptextBrush.Reset();
	m_ptextFormat.Reset();

	m_InputLayout.Reset(); // PassThrough 쉐이더용
	m_PixelShader.Reset();
	m_VertexShader.Reset();

	m_samplerState.Reset(); // Post-processing용

	m_pd2dContext.Reset();
	m_pd2dDevice.Reset();

	m_pswapChain.Reset();
	m_pd3dContext.Reset();
	m_pd3dDevice.Reset();

	OutputDebugStringA("INFO: Renderer resources released.\n");
}

void Renderer::Resize(UINT width, UINT height)
{
	if (nullptr == m_pswapChain) return;

	// 기존 D2D Render Targets 해제 후 다시 생성
	ReleaseRenderTargets();
	CreateShaderRenderTargets(); // m_screenWidth, m_screenHeight 업데이트 됨

	// D3D 스프라이트 렌더링 카메라 행렬도 업데이트
	SetupSpriteCameraMatrices(m_screenWidth, m_screenHeight);
}

// --- D3D 기반 스프라이트 렌더링을 위한 헬퍼 함수 구현 ---

HRESULT Renderer::CreateSpriteConstantBuffers()
{
	HRESULT hr = S_OK;

	// ObjectTransformCBuffer (월드-뷰-프로젝션 행렬)
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.ByteWidth = sizeof(ObjectTransformCBuffer);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;         // 런타임에 데이터 변경
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU에서 쓰기 가능
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	hr = m_pd3dDevice->CreateBuffer(&cbDesc, nullptr, m_pObjectTransformCBuffer.GetAddressOf());
	//RETURN_IF_FAILED(hr);

	// TextureAtlasCBuffer (텍스처 아틀라스 정보)
	cbDesc.ByteWidth = sizeof(TextureAtlasCBuffer);
	hr = m_pd3dDevice->CreateBuffer(&cbDesc, nullptr, m_pTextureAtlasCBuffer.GetAddressOf());
	//RETURN_IF_FAILED(hr);

	OutputDebugStringA("INFO: Sprite Constant Buffers created.\n");
	return S_OK;
}

HRESULT Renderer::CreateSpriteSamplerState()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 픽셀 아트에 적합
	// sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 부드러운 필터링
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // UV 좌표 범위 넘어갈 시 랩핑
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, m_pSpriteSamplerState.GetAddressOf());
	//RETURN_IF_FAILED(hr);

	OutputDebugStringA("INFO: Sprite Sampler State created.\n");
	return S_OK;
}



void Renderer::SetupSpriteCameraMatrices(UINT width, UINT height)
{
	// 뷰 행렬: 2D 게임이므로 카메라를 (0,0, -1)에 두고 (0,0,0)을 바라보는 것으로 설정
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_viewMatrix = XMMatrixLookAtLH(Eye, At, Up); // Left-Handed Coordinate System

	// 투영 행렬: 직교 투영 (2D 렌더링에 적합)
	// 일반적으로 화면 중앙을 (0,0)으로 두는 것이 편리
	// 게임의 월드 좌표계에 맞게 orthographicHeight를 조절
	// 예: 1000은 화면 전체 높이가 1000 월드 유닛임을 의미
	float orthographicHeight = static_cast<float>(height); // 화면 픽셀 높이를 월드 높이로 사용
	float orthographicWidth = static_cast<float>(width);   // 화면 픽셀 너비를 월드 너비로 사용

	// Renderer가 (0,0)을 좌상단으로 사용하는 윈도우 좌표계를 기본으로 한다면
	// left, right, bottom, top 값을 직접 설정해야 합니다.
	// D3D의 클립 공간은 X, Y가 -1.0 ~ +1.0, Z는 0.0 ~ 1.0 (또는 -1.0 ~ 1.0) 입니다.
	// 윈도우 좌상단 (0,0)에 매핑하려면
	m_projectionMatrix = XMMatrixOrthographicOffCenterLH(
		0.0f,                 // Left
		static_cast<float>(width),    // Right
		static_cast<float>(height),   // Bottom (Y축 상향 기준, Direct3D는 보통 Y 상향)
		0.0f,                 // Top (하지만 보통 Direct2D처럼 Y 하향을 원하므로 Top/Bottom 교체 필요)
		0.1f,                 // Near Z
		100.0f                // Far Z
	);

	// 만약 Direct2D처럼 Y축이 아래로 증가하는 좌표계를 원한다면:
	m_projectionMatrix = XMMatrixOrthographicOffCenterLH(
		0.0f,                      // Left
		static_cast<float>(width), // Right
		static_cast<float>(height),// Bottom (DirectX Math의 LH Ortho는 Y가 위로 증가)
		0.0f,                      // Top
		0.1f,                      // Near Z
		100.0f                     // Far Z
	);
	// Y축 뒤집기: 뷰 행렬에서 Y 스케일을 -1.0f로 하거나,
	// 프로젝션 행렬에서 bottom과 top 값을 뒤집어 주거나,
	// 픽셀 쉐이더에서 Texcoord.y = 1.0f - Texcoord.y; 를 하는 방법 등이 있습니다.
	// 여기서는 가장 간단하게, 뷰 행렬에서 Y축을 뒤집는 방법을 사용해봅시다.
	// (그러나 이 방법은 텍스처 좌표 Y축도 뒤집힐 수 있으므로, 텍스처 좌표가 문제가 되면 쉐이더에서 직접 뒤집는 것이 더 확실합니다.)

	// 표준 D3D 뷰 행렬에서 Y축을 뒤집는 것은 권장되지 않으며, 보통 프로젝션 행렬이나 픽셀 쉐이더에서 처리합니다.
	// 픽셀 쉐이더에서 텍스처 좌표 Y축을 1.0 - input.tex.y 로 변환하는 것이 일반적입니다.

	OutputDebugStringA("INFO: Sprite Camera matrices (View/Projection) set.\n");
}
void Renderer::CreateFullScrennQuad()
{
	SpriteVertex quadVertices[] =
	{	// x,y,z                     u,v
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f} },
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },

		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }
	};

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DEFAULT; // 사용 옵션 필요시 변경
	vbDesc.ByteWidth = sizeof(quadVertices);
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = quadVertices;

	HRESULT hr = m_pd3dDevice->CreateBuffer(&vbDesc, &initData, &m_fullScreenVB);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create fullscreen quad vertex buffer. HRESULT: " << std::hex << hr << std::endl;
	}
}

void Renderer::CreateShaderRenderTargets() {


	ComPtr<ID3D11Texture2D> backBuffer;
	HRESULT hr = m_pswapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	D3D11_TEXTURE2D_DESC backBufferDesc;
	backBuffer->GetDesc(&backBufferDesc);

	// 1. Direct3D 백버퍼용 RenderTargetView 생성 (최종 Present 전 D3D가 렌더링하는 대상)
	hr = m_pd3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_pd3dRenderTV);

	m_screenWidth = backBufferDesc.Width;
	m_screenHeight = backBufferDesc.Height;
	// 2. Direct2D가 그릴 Off-screen 텍스처 (Shader Resource로 사용될 텍스처) 생성
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = m_screenWidth;
	texDesc.Height = m_screenHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // D2D와 동일한 포맷
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // D2D가 그리고 셰이더가 읽음

	hr = m_pd3dDevice->CreateTexture2D(&texDesc, nullptr, &m_renderTargetTex);
	// 3. Off-screen 텍스처에 대한 RenderTargetView (Direct2D가 이 텍스처에 그림)
	hr = m_pd3dDevice->CreateRenderTargetView(m_renderTargetTex.Get(), nullptr, &m_offScreenTargetView);
	// 4. Off-screen 텍스처에 대한 ShaderResourceView (Direct3D 셰이더가 이 텍스처를 읽음)
	hr = m_pd3dDevice->CreateShaderResourceView(m_renderTargetTex.Get(), nullptr, &m_renderTargetSRV);
	// 5. Off-screen 텍스처를 Direct2D Bitmap으로 래핑 (Direct2D의 렌더링 대상)

	// this
	ComPtr<IDXGISurface> dxgiOffscreenSurface;
	hr = m_renderTargetTex.As(&dxgiOffscreenSurface); // m_pOffscreenRenderTargetTexture 사용



	D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
	bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET; // CANNOT_DRAW 제거

	hr = m_pd2dContext->CreateBitmapFromDxgiSurface(dxgiOffscreenSurface.Get(), &bitmapProps, &m_ptargetBitmap);

	// 6. Direct2D 컨텍스트의 렌더 타겟을 이 off-screen 비트맵으로 설정 (렌더링 시작 전에 다시 설정할 수도 있음)
	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());

	// 7. 기본 D2D 브러시 생성
	hr = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pbrush);

}

// --- 렌더링 흐름 제어 함수 수정 ---
void Renderer::RenderBegin()
{
	// 1. Direct3D 백버퍼를 최종 렌더 타겟으로 설정하고 클리어합니다.
	// 이제 D3D 스프라이트는 이 백버퍼에 직접 그려집니다.
	ID3D11RenderTargetView* rtv = m_pd3dRenderTV.Get();
	m_pd3dContext->OMSetRenderTargets(1, &rtv, nullptr); // 깊이 버퍼는 2D 스프라이트에선 선택 사항

	FLOAT backgroundColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // R, G, B, A
	m_pd3dContext->ClearRenderTargetView(rtv, backgroundColor);
	// m_pd3dContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0); // 깊이 버퍼 사용 시

	// 2. D3D 렌더링에 필요한 기본 상태 설정 (스프라이트 쉐이더에 사용)
	const ShaderSet& spriteShaderSet = m_shaderManager->GetShaderSet("SpriteShader");
	if (!spriteShaderSet.vs || !spriteShaderSet.ps || !spriteShaderSet.inputLayout) {
		OutputDebugStringA("ERROR: 'SpriteShader' ShaderSet is incomplete or not found in Renderer. Cannot set D3D states.\n");
		return;
	}

	m_pd3dContext->VSSetShader(spriteShaderSet.vs.Get(), nullptr, 0);
	m_pd3dContext->PSSetShader(spriteShaderSet.ps.Get(), nullptr, 0);
	m_pd3dContext->IASetInputLayout(spriteShaderSet.inputLayout.Get());
	m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 스프라이트 전용 샘플러 상태 설정
	m_pd3dContext->PSSetSamplers(0, 1, m_pSpriteSamplerState.GetAddressOf());

	// 뷰포트 설정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(m_screenWidth);
	viewport.Height = static_cast<float>(m_screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pd3dContext->RSSetViewports(1, &viewport);
	m_pd3dContext->RSSetState(nullptr); // 기본 래스터라이저 상태 (CullMode None)

	m_pd3dContext->OMSetDepthStencilState(nullptr, 0); // 깊이 테스트 비활성화 (2D 스프라이트)
	// 알파 블렌딩 활성화 (필요하다면)
	// D3D11_BLEND_DESC blendDesc = {};
	// blendDesc.AlphaToCoverageEnable = FALSE;
	// blendDesc.IndependentBlendEnable = FALSE;
	// blendDesc.RenderTarget[0].BlendEnable = TRUE;
	// blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	// blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	// blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	// blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	// blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	// blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	// blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	// ComPtr<ID3D11BlendState> pBlendState;
	// m_pd3dDevice->CreateBlendState(&blendDesc, &pBlendState);
	// m_pd3dContext->OMSetBlendState(pBlendState.Get(), nullptr, 0xFFFFFFFF);
}

void Renderer::DrawRect(float left, float top, float right, float bottom, const D2D1::ColorF& color)
{
	m_pbrush->SetColor(color);
	m_pd2dContext->DrawRectangle(D2D1::Rect(left, top, right, bottom), m_pbrush.Get());

}

void Renderer::DrawBitmap3D(
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	ID3D11ShaderResourceView* pTextureSRV,
	const DirectX::XMMATRIX& worldMatrix,
	const RECT* pSourceRect // 텍스처 아틀라스 내의 원본 사각형 (픽셀 단위)
)
{
	if (!m_pd3dContext || !pVertexBuffer || !pIndexBuffer || !pTextureSRV) {
		OutputDebugStringA("WARNING: DrawBitmap3D: Missing required context, buffers or texture SRV.\n");
		return;
	}

	// 1. 버텍스 및 인덱스 버퍼 바인딩
	UINT stride = sizeof(SpriteVertex); // SpriteVertex 구조체 크기 (BitmapRender3D.h에 정의)
	UINT offset = 0;
	m_pd3dContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	m_pd3dContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0); // WORD (unsigned short) 사용시

	// 2. 상수 버퍼 업데이트 및 바인딩 (World-View-Projection)
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dContext->Map(m_pObjectTransformCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to map ObjectTransformCBuffer.\n");
		return;
	}
	ObjectTransformCBuffer* pCBuffer = static_cast<ObjectTransformCBuffer*>(mappedResource.pData);
	pCBuffer->World = XMMatrixTranspose(worldMatrix); // 쉐이더로 넘길 때 전치 (DX11에서 기본적으로 행 우선)
	pCBuffer->View = XMMatrixTranspose(m_viewMatrix);
	pCBuffer->Projection = XMMatrixTranspose(m_projectionMatrix);
	m_pd3dContext->Unmap(m_pObjectTransformCBuffer.Get(), 0);

	m_pd3dContext->VSSetConstantBuffers(0, 1, m_pObjectTransformCBuffer.GetAddressOf()); // 슬롯 0에 바인딩

	// 3. 텍스처 아틀라스 상수 버퍼 업데이트 및 바인딩
	// pSourceRect는 텍셀 단위 픽셀 좌표 (예: {10, 20, 50, 60})
	// 텍스처의 전체 크기를 가져와야 합니다.
	ComPtr<ID3D11Resource> resource;
	pTextureSRV->GetResource(resource.GetAddressOf());
	ComPtr<ID3D11Texture2D> texture2D;
	resource.As(&texture2D);
	D3D11_TEXTURE2D_DESC texDesc;
	texture2D->GetDesc(&texDesc);

	hr = m_pd3dContext->Map(m_pTextureAtlasCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to map TextureAtlasCBuffer.\n");
		return;
	}
	TextureAtlasCBuffer* pAtlasCBuffer = static_cast<TextureAtlasCBuffer*>(mappedResource.pData);
	pAtlasCBuffer->textureWidth = static_cast<float>(texDesc.Width);
	pAtlasCBuffer->textureHeight = static_cast<float>(texDesc.Height);

	// sourceRect가 nullptr 일 경우 전체 텍스처 사용 (비 애니메이션 스프라이트)
	if (pSourceRect) {
		pAtlasCBuffer->sourceRectX = static_cast<float>(pSourceRect->left);
		pAtlasCBuffer->sourceRectY = static_cast<float>(pSourceRect->top);
		pAtlasCBuffer->sourceRectWidth = static_cast<float>(pSourceRect->right - pSourceRect->left);
		pAtlasCBuffer->sourceRectHeight = static_cast<float>(pSourceRect->bottom - pSourceRect->top);
	}
	else {
		// pSourceRect가 nullptr이면 전체 텍스처 사용 (0,0,width,height)
		pAtlasCBuffer->sourceRectX = 0.0f;
		pAtlasCBuffer->sourceRectY = 0.0f;
		pAtlasCBuffer->sourceRectWidth = static_cast<float>(texDesc.Width);
		pAtlasCBuffer->sourceRectHeight = static_cast<float>(texDesc.Height);
	}
	m_pd3dContext->Unmap(m_pTextureAtlasCBuffer.Get(), 0);

	m_pd3dContext->PSSetConstantBuffers(1, 1, m_pTextureAtlasCBuffer.GetAddressOf()); // 슬롯 1에 바인딩

	// 4. 텍스처 SRV 바인딩
	m_pd3dContext->PSSetShaderResources(0, 1, &pTextureSRV); // 슬롯 0에 바인딩

	// 5. 드로우 콜
	m_pd3dContext->DrawIndexed(6, 0, 0); // 인덱스 6개 (쿼드 2개 삼각형), 시작 인덱스 0, 베이스 버텍스 0
}

void Renderer::RenderEnd()
{
	// 1. Direct3D 렌더링이 모두 끝난 후, 이제 Direct2D UI를 그릴 차례
	// D3D 렌더 타겟을 D2D가 그릴 Off-screen 텍스처로 다시 설정
	// 현재 Renderer::RenderBegin()에서 m_pd3dRenderTV (백버퍼)로 설정했으므로,
	// 여기서 다시 m_offScreenTargetView (D2D용)로 변경
	ID3D11RenderTargetView* rtv = m_offScreenTargetView.Get();
	m_pd3dContext->OMSetRenderTargets(1, &rtv, nullptr); // 깊이 버퍼는 2D에선 nullptr

	// D2D 컨텍스트의 렌더 타겟을 Off-screen 비트맵으로 설정
	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());

	// D2D 드로잉 시작
	m_pd2dContext->BeginDraw();

	// D2D로 그릴 영역을 클리어 (투명하게 클리어하여 D3D 결과 위에 겹쳐지도록)
	m_pd2dContext->Clear(D2D1::ColorF(0, 0, 0, 0)); // 완전 투명

	// 여기에 기존 D2D 기반 UI 그리기 함수 호출들 (DrawCircle, DrawRect, DrawMessage 등)
	// 예: DrawCircle(100, 100, 50, D2D1::ColorF::Red);
	// 예: DrawMessage(L"Hello D2D!", 10, 10, 200, 30, D2D1::ColorF::White);
	// ...

	// D2D 드로잉 종료
	HRESULT hr = m_pd2dContext->EndDraw();
	if (FAILED(hr) && hr != D2DERR_RECREATE_TARGET)
	{
		std::cerr << "D2D EndDraw failed: HRESULT: 0x" << std::hex << hr << std::endl;
		// DXGI_ERROR_DEVICE_REMOVED 또는 DXGI_ERROR_DEVICE_RESET 발생 시
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
			Uninitialize();
			Initialize(m_hwnd);
		}
		return;
	}

	// 2. D2D로 그려진 Off-screen 텍스처를 Direct3D 백버퍼로 합성
	// 최종 렌더 타겟을 다시 D3D 백버퍼로 복구
	ID3D11RenderTargetView* backBufferRTV = m_pd3dRenderTV.Get();
	m_pd3dContext->OMSetRenderTargets(1, &backBufferRTV, nullptr);

	// 백버퍼 클리어 (필요하다면, 이미 RenderBegin에서 클리어했으므로 생략 가능)
	// float backBufferClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	// m_pd3dContext->ClearRenderTargetView(backBufferRTV, backBufferClearColor);

	// PostProcessing 적용, D2D 결과가 그려진 m_renderTargetSRV를 사용하여
	// 이를 D3D 백버퍼에 그립니다.
	const ShaderSet& currentPostProcessShader = m_shaderManager->GetShaderSet(m_postProcessShaderName);
	PostProcessing(currentPostProcessShader);

	// 3. 최종 화면 표시
	Present();
}

void Renderer::ReleaseRenderTargets()
{
	m_offScreenTargetView.Reset();
	m_renderTargetTex.Reset();
	m_renderTargetSRV.Reset();

	m_pd3dRenderTV.Reset();
	m_ptargetBitmap.Reset();

	m_pbrush.Reset();
	m_ptextBrush.Reset();

	if (m_pd2dContext)
	{
		m_pd2dContext->SetTarget(nullptr);
	}
}

void Renderer::InitializeShader(HWND hwnd)
{
	// D3D 디바이스가 유효한지 확인
	if (!m_pd3dDevice) {
		std::cerr << "ERROR: D3D Device not created. Cannot initialize shaders." << std::endl;
		return;
	}

	// 셰이더 로드 및 생성 
	ComPtr<ID3DBlob> vsBlob = nullptr; // 정점 셰이더 바이트
	ComPtr<ID3DBlob> psBlob = nullptr; // 픽셀 셰이더 바이트
	HRESULT hr;
	// Vertex HLSL
	hr = D3DReadFileToBlob(L"..\\Shader\\PassThrough_VS.cso", &vsBlob);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to read Vertex Shader CSO! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// PixelShader HLSL
	hr = D3DReadFileToBlob(L"..\\Shader\\PassThrough_PS.cso", &psBlob);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to read Pixel Shader CSO! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// 셰이더 객체 생성
	hr = m_pd3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_VertexShader);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Vertex Shader! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	hr = m_pd3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_PixelShader);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Pixel Shader! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// 3. 입력 레이아웃 생성
	// HLSL 셰이더의 VS_INPUT 구조체와 일치
	// 풀스크린 쿼드용 정점 데이터는 보통 {위치, 텍스처 좌표}로 구성

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	UINT numElements = ARRAYSIZE(layout);

	// 입력 레이아웃은 정점 셰이더의 바이트 코드를 기반으로 생성
	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Input Layout! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}


}


// 나머지 기존 Renderer 함수들 (CreateDeviceAndSwapChain, CreateWriteResource, 
// CreateFullScrennQuad, CreateShaderRenderTargets, ReleaseRenderTargets,
// InitializeShader, DrawCircle, DrawRect, DrawBitmap, DrawMessage,
// CreateBitmapFromFile, PostProcessing, Present)은 변경 없이 유지됩니다.
// 단, CreateDeviceAndSwapChain은 ID2D1Factory8, ID2D1Device7, ID2D1DeviceContext7으로 수정 필요

// --- CreateDeviceAndSwapChain (기존 코드에서 버전만 수정) ---
void Renderer::CreateDeviceAndSwapChain(HWND hwnd)
{
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	ComPtr<ID3D11Device> d3dDevice;
	ComPtr<ID3D11DeviceContext> d3dContext;

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		featureLevels,
		ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION,
		&d3dDevice,
		nullptr,
		&d3dContext);
	if (FAILED(hr)) {
		std::cout << "ERROR: D3D11CreateDevice failed! HRESULT: 0x" << std::hex << hr << std::endl;
		MessageBoxW(hwnd, L"Failed to create D3D11 Device. Check console/debug output.", L"DirectX Error", MB_OK | MB_ICONERROR);
		return;
	}

	ComPtr<IDXGIDevice> dxgiDevice;
	hr = d3dDevice.As(&dxgiDevice);
	if (FAILED(hr)) { std::cerr << "HRESULT1 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);
	if (FAILED(hr)) { std::cerr << "HRESULT2 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<IDXGIFactory2> dxgiFactory;
	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) { std::cerr << "HRESULT3 = 0x" << std::hex << hr << std::endl; return; }

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 2;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Scaling = DXGI_SCALING_STRETCH;

	ComPtr<IDXGISwapChain1> swapChain;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		d3dDevice.Get(), hwnd, &scDesc, nullptr, nullptr, &swapChain);
	if (FAILED(hr)) { std::cerr << "HRESULT4 = 0x" << std::hex << hr << std::endl; return; }

	D2D1_FACTORY_OPTIONS opts = {};
	ComPtr<ID2D1Factory7> d2dFactory; // ID2D1Factory4 -> ID2D1Factory7, IID_PPV_ARGS(&d2dFactory)
#if defined(_DEBUG)
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	// ID2D1Factory8 -> ID2D1Factory7
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory7), // ID2D1Factory8 -> ID2D1Factory7
		&opts,
		reinterpret_cast<void**>(d2dFactory.GetAddressOf()));
	if (FAILED(hr)) { std::cerr << "HRESULT5 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<ID2D1Device> baseDevice;
	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);
	if (FAILED(hr)) { std::cerr << "HRESULT6 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<ID2D1Device7> d2dDevice; // ID2D1Device4 -> ID2D1Device7
	hr = baseDevice.As(&d2dDevice);
	if (FAILED(hr)) { std::cerr << "HRESULT7 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<ID2D1DeviceContext7> d2dContext; // ID2D1DeviceContext7 유지
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
	if (FAILED(hr)) { std::cerr << "HRESULT8 = 0x" << std::hex << hr << std::endl; return; }

	m_pd3dDevice = d3dDevice;
	m_pd3dContext = d3dContext;
	m_pswapChain = swapChain;

	m_pd2dDevice = d2dDevice;
	m_pd2dContext = d2dContext;
}

void Renderer::PostProcessing(const ShaderSet& shaderSet)
{
	if (!m_pd3dContext || !m_fullScreenVB || !m_renderTargetSRV) {
		std::cerr << "ERROR: Essential resources not initialized for DrawFullScreenQuadWithShader." << std::endl;
		return;
	}

	// 쉐이더 세트의 유효성 다시 확인
	if (!shaderSet.vs || !shaderSet.ps || !shaderSet.inputLayout) {
		std::cerr << "ERROR: Provided ShaderSet is incomplete for DrawFullScreenQuadWithShader." << std::endl;
		return;
	}

	// 5. IA 설정
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	m_pd3dContext->IASetInputLayout(shaderSet.inputLayout.Get()); // ShaderSet에서 InputLayout 사용
	m_pd3dContext->IASetVertexBuffers(0, 1, m_fullScreenVB.GetAddressOf(), &stride, &offset);
	m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 쉐이더 설정
	m_pd3dContext->VSSetShader(shaderSet.vs.Get(), nullptr, 0);   // ShaderSet에서 VS 사용
	m_pd3dContext->PSSetShader(shaderSet.ps.Get(), nullptr, 0);   // ShaderSet에서 PS 사용

	// D2D 텍스처 바인딩 (픽셀 쉐이더 입력)
	ID3D11ShaderResourceView* srvs[] = { m_renderTargetSRV.Get() };
	m_pd3dContext->PSSetShaderResources(0, 1, srvs);
	m_pd3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	// 뷰포트 설정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(m_screenWidth);
	viewport.Height = static_cast<float>(m_screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_pd3dContext->RSSetViewports(1, &viewport);
	m_pd3dContext->RSSetState(nullptr); // 기본 래스터라이저 상태


	m_pd3dContext->OMSetDepthStencilState(nullptr, 0); // 깊이 테스트 비활성화
	m_pd3dContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); // 알파 블렌딩 비활성화

	// Quad 드로우
	m_pd3dContext->Draw(6, 0);

	// SRV 해제 (다음 프레임/드로우를 위해 리소스 바인딩을 끊기)
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);
}

void Renderer::CreateWriteResource()
{
	ComPtr<IDWriteFactory> writeFactory = nullptr;
	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf()));

	//DX::ThrowIfFailed(hr);

	writeFactory->CreateTextFormat(
		L"",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		15.0f,   // Font Size
		L"", //locale
		&m_ptextFormat);

	//DX::ThrowIfFailed(hr);

	m_ptextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);         // 왼쪽 정렬
	m_ptextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);  // 위쪽 정렬
	m_ptextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);              // 줄바꿈
}

void Renderer::Present()
{
	// 렌더링 작업이 끝나면 스왑체인에 프레임을 표시
	HRESULT hr = m_pswapChain->Present(1, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Uninitialize();     // 디바이스가 제거되거나 리셋된 경우, 재초기화 필요
		Initialize(m_hwnd);
	}
}

// CreateWriteResource, CreateFullScrennQuad, CreateShaderRenderTargets, ReleaseRenderTargets, InitializeShader, 
// DrawCircle, DrawRect, DrawBitmap, DrawMessage, CreateBitmapFromFile, PostProcessing, Present 함수는
// 위에 정의된 대로 (혹은 기존 코드와 동일하게) 유지됩니다.