#include "pch.h"
#include "Renderer.h"
#include "SpriteManager.h" 
//#include "InputManager.h"

#include <d3dcompiler.h> // 쉐이더 컴파일 
#include <DirectXMath.h> 

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib") 

using namespace DirectX;

Renderer::~Renderer()
{
	Uninitialize();
}

void Renderer::Initialize(HWND hwnd)
{
	m_hwnd = hwnd;

	CreateDeviceAndSwapChain(hwnd);

	m_shaderManager = std::make_unique<ShaderManager>(m_pd3dDevice.Get());

	CreateShaderRenderTargets();
	CreateFullScrennQuad();
	CreateWriteResource();
	
	ComPtr<IWICImagingFactory> wicFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory));

	m_pwicFactory = wicFactory;

	// Post-processing용 샘플러 상태
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

	//D3D11_RASTERIZER_DESC rsDesc = {};
	//rsDesc.FillMode = D3D11_FILL_SOLID;
	//rsDesc.CullMode = D3D11_CULL_NONE; // 컬링 비활성화 
	//rsDesc.FrontCounterClockwise = FALSE; // Direct3D 기본
	//rsDesc.DepthBias = 0;
	//rsDesc.DepthBiasClamp = 0.0f;
	//rsDesc.SlopeScaledDepthBias = 0.0f;
	//rsDesc.DepthClipEnable = TRUE;
	//rsDesc.ScissorEnable = FALSE;
	//rsDesc.MultisampleEnable = FALSE;
	//rsDesc.AntialiasedLineEnable = FALSE;
	//HRESULT hr_rs = m_pd3dDevice->CreateRasterizerState(&rsDesc, &m_pRasterizerState);

	CreateTimeCBuffer();
	CreaetePositionCBuffer();
	CreateColorCBuffer();

	hr = CreateSpriteConstantBuffers();
	if (FAILED(hr)) return;

	hr = CreateSpriteSamplerState();
	if (FAILED(hr)) return;

	hr = CreateBlendState();
	if (FAILED(hr)) return;

	SetupSpriteCameraMatrices(m_screenWidth, m_screenHeight);

	SpriteManager::GetInstance().Initialize(m_pd3dDevice.Get(), m_pd3dContext.Get());

	OutputDebugStringA("INFO: Renderer initialized successfully.\n");
}

void Renderer::CreateTimeCBuffer()
{
	D3D11_BUFFER_DESC cbd{};
	cbd.ByteWidth = sizeof(TimeCBuffer); // 구조체 크기
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	cbd.MiscFlags = 0;
	cbd.StructureByteStride = 0;

	HRESULT hr = m_pd3dDevice->CreateBuffer(&cbd, nullptr, &m_pTimeCBuffer);
	if (FAILED(hr)) {
		OutputDebugStringA("Failed to create Time Constant Buffer\n");
	}
}

void Renderer::CreaetePositionCBuffer() {
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 상수 버퍼로 사용
	cbDesc.Usage = D3D11_USAGE_DEFAULT;            // 매 프레임 업데이트 가능
	cbDesc.CPUAccessFlags = 0;
	cbDesc.ByteWidth = sizeof(PositionCBuffer);    
	HRESULT hr = m_pd3dDevice->CreateBuffer(&cbDesc, nullptr, &m_pPositionCBuffer);
	if (FAILED(hr))
	{
		std::cerr << "Failed to create PositionCBuffer. HRESULT: 0x"
			<< std::hex << hr << std::endl;
	}
}
void Renderer::CreateColorCBuffer() {
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
	cbDesc.Usage = D3D11_USAGE_DEFAULT;            
	cbDesc.CPUAccessFlags = 0;
	cbDesc.ByteWidth = sizeof(ColorCBuffer);

	m_pd3dDevice->CreateBuffer(&cbDesc, nullptr, &m_pColorCBuffer);
	m_pd3dDevice->CreateBuffer(&cbDesc, nullptr, &m_pUIColorCBuffer);

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
	ReleaseRenderTargets();
	m_pwicFactory.Reset();
	m_ptargetBitmap.Reset();
	m_pbrush.Reset();

	m_smallFormat.Reset();
	m_middleFormat.Reset();
	m_largeFormat.Reset();

	m_InputLayout.Reset();
	m_PixelShader.Reset();
	m_VertexShader.Reset();

	m_samplerState.Reset();

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
	CreateShaderRenderTargets();
	// D3D 스프라이트 렌더링 카메라 행렬도 업데이트
	SetupSpriteCameraMatrices(m_screenWidth, m_screenHeight);
}


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

	// TextureAtlasCBuffer 
	cbDesc.ByteWidth = sizeof(TextureAtlasCBuffer);
	hr = m_pd3dDevice->CreateBuffer(&cbDesc, nullptr, m_pTextureAtlasCBuffer.GetAddressOf());


	OutputDebugStringA("INFO: Sprite Constant Buffers created.\n");
	return S_OK;
}

HRESULT Renderer::CreateSpriteSamplerState()
{
	HRESULT hr = S_OK;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // 픽셀 아트에 적합
	// sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 부드러운 필터링
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
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



HRESULT Renderer::CreateBlendState()
{
	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pd3dDevice->CreateBlendState(&bd, &m_blendState);
	return S_OK;
}

void Renderer::SetupSpriteCameraMatrices(UINT width, UINT height)
{

	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	m_viewMatrix = XMMatrixLookAtLH(Eye, lookAt, Up);

	// 중앙(0,0) 기준 좌표 → 좌상단(0,0) 기준 픽셀 좌표계로 맞춤
	float halfW = width / 2.0f;
	float halfH = height / 2.0f;

	m_projectionMatrix = XMMatrixOrthographicLH(width, height, 0.1f, 100.0f); //직교투영

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

	hr = m_pd3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_pd3dRenderTV);

	m_screenWidth = backBufferDesc.Width;
	m_screenHeight = backBufferDesc.Height;

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

	hr = m_pd3dDevice->CreateRenderTargetView(m_renderTargetTex.Get(), nullptr, &m_offScreenRTV);

	hr = m_pd3dDevice->CreateShaderResourceView(m_renderTargetTex.Get(), nullptr, &m_renderTargetSRV);


	// this text
	ComPtr<IDXGISurface> dxgiOffscreenSurface;
	hr = m_renderTargetTex.As(&dxgiOffscreenSurface);

	D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
	bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET; // CANNOT_DRAW 제거

	hr = m_pd2dContext->CreateBitmapFromDxgiSurface(dxgiOffscreenSurface.Get(), &bitmapProps, &m_ptargetBitmap);
	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());
	hr = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pbrush);

}


void Renderer::RenderBegin()
{

	//ID3D11RenderTargetView* rtv = m_pd3dRenderTV.Get();
	ID3D11RenderTargetView* rtv = m_offScreenRTV.Get();
	m_pd3dContext->OMSetRenderTargets(1, &rtv, nullptr); // rederTarget m_offScreenView

	FLOAT backgroundColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_pd3dContext->ClearRenderTargetView(rtv, backgroundColor);
	// m_pd3dContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0); // 깊이 버퍼 사용 시

	// ------------------------- shader 모드 설정분리 ------------------------
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

	m_pd3dContext->OMSetDepthStencilState(nullptr, 0); // 깊이 테스트 비활성화 
	m_pd3dContext->OMSetBlendState(m_blendState.Get(), nullptr, 0xFFFFFFFF);
}

void Renderer::SetShaderMode(const string& mode, float timer) {

	// cso 바인딩
	// 추가 자원(normal, noise) 있는 경우 if 로 분기하죠

	const ShaderSet& DefaultShaderSet = m_shaderManager->GetOBJShaderSet(mode);
	//const ShaderSet& DefaultShaderSet = m_shaderManager->GetShaderSet(mode); //적용 ★★★

	if (!DefaultShaderSet.vs || !DefaultShaderSet.ps || !DefaultShaderSet.inputLayout) {
		//OutputDebugStringA("ERROR: 'DefaultShader' ShaderSet is incomplete or not found in Renderer. Cannot set D3D states.\n");
		//cout << "spreiteShader fail" << endl;
		return;
	}

	m_pd3dContext->VSSetShader(DefaultShaderSet.vs.Get(), nullptr, 0);
	m_pd3dContext->PSSetShader(DefaultShaderSet.ps.Get(), nullptr, 0);
	m_pd3dContext->IASetInputLayout(DefaultShaderSet.inputLayout.Get());
	m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 스프라이트 전용 샘플러
	m_pd3dContext->PSSetSamplers(0, 1, m_pSpriteSamplerState.GetAddressOf());
	TimeCBuffer timeData{};

	// 반복시간
	timeData.time = RenderTimer::GetInstance().GetElapsedTime()/2.0f;
	timeData.deltaTime = 1.0f; // 고정 델타타임
	timeData.padding[0] = 0.0f;
	timeData.padding[1] = 0.0f;
	//cout << timeData.time << endl;

	if (mode == "NoiseBlend") {
		// SRV 리턴
		/*timeData.time += timer;
		timeData.deltaTime = 1.0f;
		timeData.padding[0] = 0.0f;
		timeData.padding[1] = 0.0f;*/
		auto noiseSRV = SpriteManager::GetInstance().GetTextureSRV("Seamless2.png");
		auto noiseSRV2 = SpriteManager::GetInstance().GetTextureSRV("PerlinNoise.png");

		ID3D11ShaderResourceView* srvs[2] = { noiseSRV.Get(), noiseSRV2.Get() };
		ID3D11Buffer* cbuffers[1] = { m_pTimeCBuffer.Get() };
		
		m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);
		m_pd3dContext->PSSetConstantBuffers(0, 1, cbuffers);
		m_pd3dContext->PSSetShaderResources(1, 2, srvs);
		
		
		//색상 바인딩
		ColorCBuffer colorData{};
		colorData.prevColor = Mediator::GetInstance().GetBackPrevColor();
		colorData.targetColor = Mediator::GetInstance().GetBackTargetColor();
	
		m_pd3dContext->UpdateSubresource(m_pColorCBuffer.Get(), 0, nullptr, &colorData, 0, 0);
		ID3D11Buffer* cbuffers1[1] = {m_pColorCBuffer.Get() };
		m_pd3dContext->PSSetConstantBuffers(2, 1, cbuffers1);
	}

	if (mode == "Holo"||mode=="SetRed") {	
		ID3D11Buffer* cbuffers[1] = { m_pTimeCBuffer.Get() };
		m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);
		
		m_pd3dContext->PSSetConstantBuffers(0, 1, cbuffers);

	}
	if (mode == "Othello") {
		// 수정 필요
		TimeCBuffer timeData{};

		timeData.time += timer;
		timeData.deltaTime = 1.0f;
		timeData.padding[0] = 0.0f;
		timeData.padding[1] = 0.0f;

		m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);

		
		ID3D11Buffer* cbuffers[1] = { m_pTimeCBuffer.Get() };
		m_pd3dContext->PSSetConstantBuffers(1, 1, cbuffers);
	}

}

void Renderer::SetShaderMode(const string& mode, ComPtr<ID3D11ShaderResourceView> pTextureSRV, float timer) // 외부에서 재료로 쓸 SRV
{
	const ShaderSet& DefaultShaderSet = m_shaderManager->GetOBJShaderSet(mode);
	if (!DefaultShaderSet.vs || !DefaultShaderSet.ps || !DefaultShaderSet.inputLayout) {
		//OutputDebugStringA("ERROR: 'DefaultShader' ShaderSet is incomplete or not found in Renderer. Cannot set D3D states.\n");
		//cout << "spreiteShader fail" << endl;
		return;
	}

	m_pd3dContext->VSSetShader(DefaultShaderSet.vs.Get(), nullptr, 0);
	m_pd3dContext->PSSetShader(DefaultShaderSet.ps.Get(), nullptr, 0);
	m_pd3dContext->IASetInputLayout(DefaultShaderSet.inputLayout.Get());
	m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 스프라이트 전용 샘플러
	m_pd3dContext->PSSetSamplers(0, 1, m_pSpriteSamplerState.GetAddressOf());
	TimeCBuffer timeData{};
	timeData.time += timer;
	timeData.deltaTime = 1.0f; // 고정 델타타임
	timeData.padding[0] = 0.0f;
	timeData.padding[1] = 0.0f;

	ID3D11ShaderResourceView* srvs[1] = { pTextureSRV.Get()};
	m_pd3dContext->PSSetShaderResources(1, 1, srvs); // register 1번 바인딩, (원본은 밑에서 0번 바인딩)

	m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);

	ID3D11Buffer* cbuffers[1] = { m_pTimeCBuffer.Get() };
	m_pd3dContext->PSSetConstantBuffers(0, 1, cbuffers);
}

void Renderer::DrawBitmap3D(
	ID3D11Buffer* pVertexBuffer,
	ID3D11Buffer* pIndexBuffer,
	ID3D11ShaderResourceView* pTextureSRV, // object에서 넘겨받은 texture
	const DirectX::XMMATRIX& worldMatrix,
	const RECT* pSourceRect // 텍스처 아틀라스 내의 원본 사각형 (픽셀 단위)
	// 여기서 시간 받기?
)
{
	if (!m_pd3dContext || !pVertexBuffer || !pIndexBuffer || !pTextureSRV) {
		OutputDebugStringA("WARNING: DrawBitmap3D: Missing required context, buffers or texture SRV.\n");
		return;
	}

	// 버텍스 및 인덱스 버퍼 바인딩
	UINT stride = sizeof(SpriteVertex); // SpriteVertex 구조체 크기 (BitmapRender3D.h에 정의)
	UINT offset = 0;
	m_pd3dContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	m_pd3dContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0); // WORD (unsigned short) 사용시

	// 상수 버퍼 업데이트 및 바인딩 (World-View-Projection)
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pd3dContext->Map(m_pObjectTransformCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to map ObjectTransformCBuffer.\n");
		return;
	}
	ObjectTransformCBuffer* pCBuffer = static_cast<ObjectTransformCBuffer*>(mappedResource.pData);

	pCBuffer->World = worldMatrix;
	pCBuffer->View = m_viewMatrix;
	pCBuffer->Projection = m_projectionMatrix;

	// D2D1::Matrix3x2F::Scale(1.0f, -1.0f) * D2D1::Matrix3x2F::Translation(screenW / 2, screenH / 2);

	m_pd3dContext->Unmap(m_pObjectTransformCBuffer.Get(), 0);

	m_pd3dContext->VSSetConstantBuffers(0, 1, m_pObjectTransformCBuffer.GetAddressOf()); // 슬롯 0에 바인딩


	// pSourceRect는 텍셀 단위 픽셀 좌표 (예: {10, 20, 50, 60})
	ComPtr<ID3D11Resource> resource;
	pTextureSRV->GetResource(resource.GetAddressOf()); // texture 자원 넣는곳
	ComPtr<ID3D11Texture2D> texture2D;
	resource.As(&texture2D);
	D3D11_TEXTURE2D_DESC texDesc;
	texture2D->GetDesc(&texDesc);
	//std::cout << "Tex size: " << texDesc.Width << " x " << texDesc.Height << std::endl; 완료
	hr = m_pd3dContext->Map(m_pTextureAtlasCBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: Failed to map TextureAtlasCBuffer.\n");
		return;
	}
	TextureAtlasCBuffer* pAtlasCBuffer = static_cast<TextureAtlasCBuffer*>(mappedResource.pData);
	pAtlasCBuffer->textureWidth = static_cast<float>(texDesc.Width);
	pAtlasCBuffer->textureHeight = static_cast<float>(texDesc.Height);

	// sourceRect가 nullptr 일 경우 전체 텍스처 사용 
	if (pSourceRect) {
		pAtlasCBuffer->sourceRectX = static_cast<float>(pSourceRect->left);
		pAtlasCBuffer->sourceRectY = static_cast<float>(pSourceRect->top);
		pAtlasCBuffer->sourceRectWidth = static_cast<float>(pSourceRect->right - pSourceRect->left);
		pAtlasCBuffer->sourceRectHeight = static_cast<float>(pSourceRect->bottom - pSourceRect->top);
	}
	else {
		// pSourceRect가 nullptr이면 전체 텍스처 사용 
		pAtlasCBuffer->sourceRectX = 0.0f;
		pAtlasCBuffer->sourceRectY = 0.0f;
		pAtlasCBuffer->sourceRectWidth = static_cast<float>(texDesc.Width);
		pAtlasCBuffer->sourceRectHeight = static_cast<float>(texDesc.Height);
	}


	m_pd3dContext->Unmap(m_pTextureAtlasCBuffer.Get(), 0);

	m_pd3dContext->PSSetConstantBuffers(1, 1, m_pTextureAtlasCBuffer.GetAddressOf()); // 슬롯 1에 바인딩

	
	m_pd3dContext->PSSetShaderResources(0, 1, &pTextureSRV);
		


	// 드로우 콜
	m_pd3dContext->DrawIndexed(6, 0, 0); // 인덱스 6개 (쿼드 2개 삼각형), 시작 인덱스 0, 베이스 버텍스 0
}



void Renderer::PostProcessing(const ShaderSet& shaderSet)
{
	if (!m_pd3dContext || !m_fullScreenVB || !m_renderTargetSRV) {
		std::cerr << "ERROR" << std::endl;
		return;
	}

	// 쉐이더 세트의 유효성 다시 확인
	if (!shaderSet.vs || !shaderSet.ps || !shaderSet.inputLayout) {
		std::cerr << "ERROR" << std::endl;
		return;
	}

	// IA 설정
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

	// shaderSet에 따라서 다른 상수버퍼바인딩
	if (m_postProcessShaderName == "BlackHole") {
		// BlackHole용 상수 버퍼 업데이트
		TimeCBuffer timeData{};
		m_blackHoleTime += 0.01f;
		if (m_blackHoleTime > 8.0f) m_blackHoleTime = 0.0f;

		timeData.time = m_blackHoleTime;
		timeData.deltaTime = 1.0f; // 고정 델타
		timeData.padding[0] = 0.0f;
		timeData.padding[1] = 0.0f;
		m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);
		
		// blackHole 중심도 버퍼로 넘겨야함..
		PositionCBuffer positionData{};
		float uvX = (static_cast<float>(Mediator::GetInstance().GetPosition().x) / static_cast<float>(m_screenWidth))+ 0.5f;
		float uvY = (static_cast<float>( - Mediator::GetInstance().GetPosition().y) / static_cast<float> (m_screenHeight)) + 0.5f;
		//cout << "uvX: " << uvX << ", uvY: " << uvY << endl;
		positionData.x = uvX;
		positionData.y = uvY;
		positionData.padding[0] = 0.0f;
		positionData.padding[1] = 0.0f;
		m_pd3dContext->UpdateSubresource(m_pPositionCBuffer.Get(), 0, nullptr, &positionData, 0, 0);
		//동시 바인딩으로 변경
		ID3D11Buffer* cbuffers[] = {m_pPositionCBuffer.Get(),m_pTimeCBuffer.Get()};
		m_pd3dContext->PSSetConstantBuffers(0, 2, cbuffers);
		

	}
	if (m_postProcessShaderName == "CRTNoise") {
		TimeCBuffer timeData{};

		// 반복시간
		timeData.time = RenderTimer::GetInstance().GetElapsedTime() / 2.0f;
		timeData.deltaTime = 1.0f; // 고정 델타타임
		timeData.padding[0] = 0.0f;
		timeData.padding[1] = 0.0f;
		m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);
		ID3D11Buffer* cbuffers[1] = { m_pTimeCBuffer.Get() };
		m_pd3dContext->PSSetConstantBuffers(0, 1, cbuffers);
	}
	if (m_postProcessShaderName == "CRTOn"|| m_postProcessShaderName == "CRTOff") {

		TimeCBuffer timeData{};
		m_blackHoleTime += 0.008f;

		//if (m_blackHoleTime > 8.0f) m_blackHoleTime = 0.0f;
		// 반복시간
		timeData.time = m_blackHoleTime;//RenderTimer::GetInstance().GetElapsedTime() / 2.0f;
		timeData.deltaTime = 1.0f; // 고정 델타타임
		timeData.padding[0] = 0.0f;
		timeData.padding[1] = 0.0f;
		m_pd3dContext->UpdateSubresource(m_pTimeCBuffer.Get(), 0, nullptr, &timeData, 0, 0);
		ID3D11Buffer* cbuffers[1] = { m_pTimeCBuffer.Get() };
		m_pd3dContext->PSSetConstantBuffers(0, 1, cbuffers);

	}

	// 뷰포트 설정
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float>(m_screenWidth);
	viewport.Height = static_cast<float>(m_screenHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//m_pd3dContext->RSSetViewports(1, &viewport);
	//m_pd3dContext->RSSetState(nullptr); // 기본 래스터라이저 상태


	m_pd3dContext->OMSetDepthStencilState(nullptr, 0); // 깊이 테스트 비활성화
	m_pd3dContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); // 알파 블렌딩 비활성화


	m_pd3dContext->Draw(6, 0);

	// SRV 해제 (다음 프레임/드로우를 위해 리소스 바인딩을 끊기)
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);
}

void Renderer::RenderEnd()
{
	// SRV 해제
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);
	// 타겟전환
	ID3D11RenderTargetView* backBufferRTV = m_pd3dRenderTV.Get();
	m_pd3dContext->OMSetRenderTargets(1, &backBufferRTV, nullptr);

	const ShaderSet& currentPostProcessShader = m_shaderManager->GetPostShaderSet(m_postProcessShaderName);    //★★ 쉐이더 변경
	PostProcessing(currentPostProcessShader);

	Present();
}

void Renderer::ReleaseRenderTargets()
{
	m_offScreenRTV.Reset();
	m_renderTargetTex.Reset();
	m_renderTargetSRV.Reset();

	m_pd3dRenderTV.Reset();
	//m_ptargetBitmap.Reset();

	m_pbrush.Reset();

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

void Renderer::UITextBegin()
{
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);

	// D2D 타겟 지정
	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());
	m_pd2dContext->BeginDraw();
}

void Renderer::DrawUIText(const std::wstring& s, float x, float y, float width, float height, int sizeType)
{
	// IDWriteTextLayout으로 변경하기
	// 화면 좌표로 보정
	float transX = x + 960.0f;
	float transY = -y + 540.0f;
	D2D1_RECT_F rc = D2D1::RectF(transX, transY, transX + width, transY + height); 
	if (sizeType == 0) {
		m_pd2dContext->DrawTextW(
			s.c_str(), (UINT32)s.size(),
			m_smallFormat.Get(), rc, m_pbrush.Get(),
			D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	}
	else if (sizeType == 1) {
		m_pd2dContext->DrawTextW(
			s.c_str(), (UINT32)s.size(),
			m_middleFormat.Get(), rc, m_pbrush.Get(),
			D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	}
	else if(sizeType == 2) {
		m_pd2dContext->DrawTextW(
			s.c_str(), (UINT32)s.size(),
			m_largeFormat.Get(), rc, m_pbrush.Get(),
			D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	}
	
}

void Renderer::UITextEnd()
{
	HRESULT hr = m_pd2dContext->EndDraw();
}

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
	ComPtr<ID2D1Factory6> d2dFactory; // ID2D1Factory4 -> ID2D1Factory7, IID_PPV_ARGS(&d2dFactory)
#if defined(_DEBUG)
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	// ID2D1Factory8 -> ID2D1Factory7
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory6), // ID2D1Factory8 -> ID2D1Factory7
		&opts,
		reinterpret_cast<void**>(d2dFactory.GetAddressOf()));
	if (FAILED(hr)) { std::cerr << "HRESULT5 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<ID2D1Device> baseDevice;
	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);
	if (FAILED(hr)) { std::cerr << "HRESULT6 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<ID2D1Device6> d2dDevice; // ID2D1Device4 -> ID2D1Device7
	hr = baseDevice.As(&d2dDevice);
	if (FAILED(hr)) { std::cerr << "HRESULT7 = 0x" << std::hex << hr << std::endl; return; }

	ComPtr<ID2D1DeviceContext6> d2dContext; // ID2D1DeviceContext7 유지
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
	if (FAILED(hr)) { std::cerr << "HRESULT8 = 0x" << std::hex << hr << std::endl; return; }

	m_pd3dDevice = d3dDevice;
	m_pd3dContext = d3dContext;
	m_pswapChain = swapChain;

	m_pd2dDevice = d2dDevice;
	m_pd2dContext = d2dContext;
}


// Textformat
void Renderer::CreateWriteResource()
{
	AddFontResourceExW(L".\\Resource\\Galmuri11.ttf",
		FR_PRIVATE, // 현재 프로세스 전용
		nullptr);

	ComPtr<IDWriteFactory> writeFactory = nullptr;
	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf()));

	//DX::ThrowIfFailed(hr);
	// text format 3가지 정도 small middle large

	writeFactory->CreateTextFormat(
		L"Galmuri11",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		70.0f,   // Font Size
		L"", //locale
		&m_largeFormat);

	//DX::ThrowIfFailed(hr);DWRITE_TEXT_ALIGNMENT_CENTER
	//DWRITE_TEXT_ALIGNMENT_TRAILING //오른쪽
	m_largeFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);          //중앙정렬 사용
	m_largeFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);  // 위쪽 정렬
	//m_largeFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);              // 줄바꿈

	writeFactory->CreateTextFormat(
		L"Galmuri11",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		50.0f,   // Font Size
		L"", //locale
		&m_middleFormat);

	//DX::ThrowIfFailed(hr);DWRITE_TEXT_ALIGNMENT_CENTER
	//DWRITE_TEXT_ALIGNMENT_TRAILING //오른쪽
	m_middleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);          //중앙정렬 사용
	m_middleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	writeFactory->CreateTextFormat(
		L"Galmuri11",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,   // Font Size
		L"", //locale
		&m_smallFormat);

	//DX::ThrowIfFailed(hr);DWRITE_TEXT_ALIGNMENT_CENTER
	//DWRITE_TEXT_ALIGNMENT_TRAILING //오른쪽
	m_smallFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);          //중앙정렬 사용
	m_smallFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
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
