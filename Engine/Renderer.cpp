#include "pch.h"
#include "Renderer.h"

//void Renderer::SetTransform(const D2D1_MATRIX_3X2_F tm)
//{
//	if (m_pd2dContext) m_pd2dContext->SetTransform(tm);
//}

void Renderer::CreateDeviceAndSwapChain(HWND hwnd)
{
	//1. D3D11 디바이스 생성
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	ComPtr<ID3D11Device> d3dDevice;
	ComPtr<ID3D11DeviceContext> d3dContext;

	HRESULT hr = D3D11CreateDevice(
		nullptr,                            //[in, optional]  IDXGIAdapter* pAdapter
		D3D_DRIVER_TYPE_HARDWARE,           //D3D_DRIVER_TYPE DriverType
		nullptr,                            //HMODULE         Software
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,   //UINT            Flags
		featureLevels,                      //[in, optional]  const D3D_FEATURE_LEVEL* pFeatureLevels
		ARRAYSIZE(featureLevels),           //UINT            FeatureLevels
		D3D11_SDK_VERSION,                  //UINT            SDKVersion
		&d3dDevice,                         //[out, optional] ID3D11Device** ppDevice
		nullptr,                            //[out, optional] D3D_FEATURE_LEVEL* pFeatureLevel
		&d3dContext);                       //[out, optional] ID3D11DeviceContext** ppImmediateContext
	if (FAILED(hr))
    {
        
        std::cout << "ERROR: D3D11CreateDevice failed! HRESULT: 0x" << std::hex << hr << std::endl;
        MessageBoxW(hwnd, L"Failed to create D3D11 Device. Check console/debug output.", L"DirectX Error", MB_OK | MB_ICONERROR);
     
        return; 
    }

	//DX::ThrowIfFailed(hr);

	// 2. DXGI 스왑체인 생성
	ComPtr<IDXGIDevice> dxgiDevice;
	hr = d3dDevice.As(&dxgiDevice);

	if (FAILED(hr)) {
		std::cerr << "HRESULT1 = 0x" << std::hex << hr << std::endl;
		return;
	}

	//DX::ThrowIfFailed(hr);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);

	if (FAILED(hr)) {
		std::cerr << "HRESULT2 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	ComPtr<IDXGIFactory2> dxgiFactory;
	hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

	if (FAILED(hr)) {
		std::cerr << "HRESULT3 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 2;
	//scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scDesc.Scaling = DXGI_SCALING_STRETCH;

	ComPtr<IDXGISwapChain1> swapChain;
	hr = dxgiFactory->CreateSwapChainForHwnd(
		d3dDevice.Get(), hwnd, &scDesc, nullptr, nullptr, &swapChain);

	if (FAILED(hr)) {
		std::cerr << "HRESULT4 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	// 3. ID2D1Factory4 생성
	D2D1_FACTORY_OPTIONS opts = {};
	ComPtr<ID2D1Factory7> d2dFactory;

#if defined(_DEBUG)
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory8),
		&opts,
		reinterpret_cast<void**>(d2dFactory.GetAddressOf()));
	if (FAILED(hr)) {
		std::cerr << "HRESULT5 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	// 4. ID2D1Device4 생성
	ComPtr<ID2D1Device> baseDevice;
	hr = d2dFactory->CreateDevice(dxgiDevice.Get(), &baseDevice);
	if (FAILED(hr)) {
		std::cerr << "HRESULT6 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	ComPtr<ID2D1Device7> d2dDevice;
	hr = baseDevice.As(&d2dDevice);
	if (FAILED(hr)) {
		std::cerr << "HRESULT7 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	// 5. ID2D1DeviceContext7 생성
	ComPtr<ID2D1DeviceContext7> d2dContext;//
	hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
	if (FAILED(hr)) {
		std::cerr << "HRESULT8 = 0x" << std::hex << hr << std::endl;
		return;
	}
	//DX::ThrowIfFailed(hr);

	m_pd3dDevice = d3dDevice;
	m_pd3dContext = d3dContext;
	m_pswapChain = swapChain;

	m_pd2dDevice = d2dDevice;
	m_pd2dContext = d2dContext;
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

	m_ptextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); // 왼쪽 정렬
	m_ptextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); // 위쪽 정렬
	m_ptextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP); // 줄바꿈
}


void Renderer::CreateFullScrennQuad()
{
	Vertex quadVertices[] =
	{	// x,y,z                     u,v
		{ {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f} },
		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} },

		{ { 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f} },
		{ { 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f} },
		{ {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f} }
	};

	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DEFAULT;
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


void Renderer::Initialize(HWND hwnd)
{
	//std::cout << m_postProcessShaderName << endl;
	//std::cout << "dick" << m_postProcessShaderName << std::endl;
	m_hwnd = hwnd;
	CreateDeviceAndSwapChain(hwnd);

	m_shaderManager = make_unique<ShaderManager>(m_pd3dDevice.Get()); // shader Manager 생성자 및, read all shader자원
	CreateShaderRenderTargets();
	CreateFullScrennQuad();
	//CreateTimeShaderBuffer();
	CreateWriteResource();

	//Window Imaging Component factory 생성
	ComPtr<IWICImagingFactory> wicFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory));
	
	//예외처리 생략
	m_pwicFactory = wicFactory;

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 부드러운 필터링
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;   // 텍스처 좌표가 범위를 벗어날 때 클램프
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX; // 모든 밉맵 레벨 사용

	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_samplerState);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Sampler State! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
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

	// 2. 셰이더 객체 생성
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

void Renderer::Uninitialize()
{
	m_shaderManager.reset(); // unique_ptr 해제
	ReleaseRenderTargets();
	m_pwicFactory.Reset(); // ComPtr는 Reset()으로 명시적 해제
	m_ptargetBitmap.Reset();
	m_pbrush.Reset();
	m_ptextBrush.Reset();
	m_ptextFormat.Reset();

	m_pd2dContext.Reset();
	m_pd2dDevice.Reset();

	m_pswapChain.Reset();
	m_pd3dContext.Reset();
	m_pd3dDevice.Reset();

}

void Renderer::Resize(UINT width, UINT height)
{
	if (nullptr == m_pswapChain) return;
	ReleaseRenderTargets();
	CreateShaderRenderTargets();
}

void Renderer::DrawCircle(float x, float y, float radius, const D2D1::ColorF& color)
{
	m_pbrush->SetColor(color);
	m_pd2dContext->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius), m_pbrush.Get());

}

void Renderer::DrawRect(float left, float top, float right, float bottom, const D2D1::ColorF& color)
{
	m_pbrush->SetColor(color);
	m_pd2dContext->DrawRectangle(D2D1::Rect(left, top, right, bottom), m_pbrush.Get());

}

void Renderer::DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F dest)
{
	m_pd2dContext->DrawBitmap(bitmap, dest);
	
}
void Renderer::DrawBitmap(ID2D1Bitmap1* bitmap, D2D1_RECT_F destRect, D2D1_RECT_F srcRect, float opacity)
{
	m_pd2dContext->DrawBitmap(
		bitmap,
		destRect,
		opacity,
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		srcRect
	); 
}

void Renderer::DrawMessage(const wchar_t* text, D2D1_RECT_F layoutRect, const D2D1::ColorF& color, ComPtr<IDWriteTextFormat> textFormat)
{
	if (nullptr == m_ptextBrush) HRESULT hr = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(color), &m_ptextBrush);
	m_ptextBrush->SetColor(color);

	m_pd2dContext->DrawTextW
	(
		text,
		static_cast<UINT32>(wcslen(text)),
		textFormat.Get(),
		layoutRect,
		m_ptextBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL
	);
}

void Renderer::CreateBitmapFromFile(const wchar_t* path, ID2D1Bitmap1*& outBitmap)
{
	ComPtr<IWICBitmapDecoder>     decoder;
	ComPtr<IWICBitmapFrameDecode> frame;
	ComPtr<IWICFormatConverter>   converter;


	HRESULT hr = m_pwicFactory->CreateDecoderFromFilename(
		path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);

	//DX::ThrowIfFailed(hr);


	hr = decoder->GetFrame(0, &frame);

	//DX::ThrowIfFailed(hr);

	hr = m_pwicFactory->CreateFormatConverter(&converter);

	//DX::ThrowIfFailed(hr);


	hr = converter->Initialize(
		frame.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom
	);

	//DX::ThrowIfFailed(hr);

	// Direct2D 비트맵 속성 (premultiplied alpha, B8G8R8A8_UNORM)
	D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	// ⑥ DeviceContext에서 WIC 비트맵으로부터 D2D1Bitmap1 생성
	hr = m_pd2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bmpProps, &outBitmap);


}

void Renderer::RenderBegin()
{
	if (!m_pd2dContext || !m_offScreenTargetView) {
		std::cerr << "ERROR: D2D context or shader render target view not initialized for ShaderRenderBegin." << std::endl;
		return;
	}

	// 1. Direct3D 렌더 타겟을 off-screen 텍스처로 설정 (D2D가 그릴 대상)
	ID3D11RenderTargetView* rtv = m_offScreenTargetView.Get();
	m_pd3dContext->OMSetRenderTargets(1, &rtv, nullptr); 

	// 2. Direct3D off-screen 텍스처를 클리어 (필요하다면)
	float clearColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f }; 
	m_pd3dContext->ClearRenderTargetView(m_offScreenTargetView.Get(), clearColor);

	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());

	m_pd2dContext->BeginDraw();

	// D2D 드로잉 시작 전 D2D 렌더 타겟도 클리어
	m_pd2dContext->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray)); // D2D로 그릴 영역을 클리어
	
}
void Renderer::PostProcessing(const ShaderSet& shaderSet)
{
	if (!m_pd3dContext || !m_fullScreenVB || !m_renderTargetSRV) {
		std::cerr << "ERROR: Essential resources not initialized for DrawFullScreenQuadWithShader." << std::endl;
		return;
	}

	// 쉐이더 세트의 유효성 다시 확인)
	if (!shaderSet.vs || !shaderSet.ps || !shaderSet.inputLayout) {
		std::cerr << "ERROR: Provided ShaderSet is incomplete for DrawFullScreenQuadWithShader." << std::endl;
		return;
	}

	// 5. IA 단계 설정
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pd3dContext->IASetInputLayout(shaderSet.inputLayout.Get()); // ShaderSet에서 InputLayout 사용
	m_pd3dContext->IASetVertexBuffers(0, 1, m_fullScreenVB.GetAddressOf(), &stride, &offset);
	m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 6. 쉐이더 설정
	m_pd3dContext->VSSetShader(shaderSet.vs.Get(), nullptr, 0);   // ShaderSet에서 VS 사용
	m_pd3dContext->PSSetShader(shaderSet.ps.Get(), nullptr, 0);   // ShaderSet에서 PS 사용

	// 7. D2D가 그린 텍스처 바인딩 (픽셀 쉐이더 입력)
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

	// 깊이/스텐실 및 블렌드 스테이트 설정 (후처리 효과이므로 기본값 사용)
	m_pd3dContext->OMSetDepthStencilState(nullptr, 0); // 깊이 테스트 비활성화
	m_pd3dContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); // 알파 블렌딩 비활성화

	// 8. 풀스크린 Quad 드로우
	m_pd3dContext->Draw(6, 0);

	// 9. SRV 해제 (다음 프레임/드로우를 위해 리소스 바인딩을 끊어줌)
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);
}

void Renderer::RenderEnd()
{
	if (!m_pd2dContext || !m_shaderManager) {
		std::cerr << "ERROR: D2D context not initialized for ShaderRenderEnd." << std::endl;
		return;
	}

	// 1. Direct2D 드로잉 종료
	HRESULT hr = m_pd2dContext->EndDraw();
	if (FAILED(hr) && hr != D2DERR_RECREATE_TARGET)
	{
		std::cerr << "D2D EndDraw failed: HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	//m_pd3dContext->Flush();
	// 
	// 2. 백버퍼 RTV로 타겟 복구
	ID3D11RenderTargetView* backBufferRTV = m_pd3dRenderTV.Get();
	m_pd3dContext->OMSetRenderTargets(1, &backBufferRTV, nullptr);

	// 3. 백버퍼 클리어
	float backBufferClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //black;
	m_pd3dContext->ClearRenderTargetView(backBufferRTV, backBufferClearColor);

	//// 4. 셰이더 리소스 체크
	//if (!m_InputLayout || !m_VertexShader || !m_PixelShader || !m_samplerState || !m_fullScreenVB)
	//{
	//	std::cerr << "ERROR: Shader pipeline resources not initialized." << std::endl;
	//	return;
	//}
	// ---------------------------------------------------------함수로 뺄 수 있음
	
	const ShaderSet& currentPostProcessShader = m_shaderManager->GetShaderSet(m_postProcessShaderName);
	PostProcessing(currentPostProcessShader);

	Present();
	// 5. IA 단계 설정
	//UINT stride = sizeof(Vertex);
	//UINT offset = 0;
	//m_pd3dContext->IASetInputLayout(m_InputLayout.Get());
	//m_pd3dContext->IASetVertexBuffers(0, 1, m_fullScreenVB.GetAddressOf(), &stride, &offset);
	//m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// 6. 셰이더 설정
	//m_pd3dContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	//m_pd3dContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	//// 7. D2D가 그린 텍스처 바인딩
	//ID3D11ShaderResourceView* srvs[] = { m_renderTargetSRV.Get() };
	//m_pd3dContext->PSSetShaderResources(0, 1, srvs);
	//m_pd3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	//D3D11_VIEWPORT viewport = {};
	//viewport.TopLeftX = 0.0f;
	//viewport.TopLeftY = 0.0f;

	//viewport.Width = static_cast<float>(m_screenWidth);
	//viewport.Height = static_cast<float>(m_screenHeight);

	//viewport.MinDepth = 0.0f; 
	//viewport.MaxDepth = 1.0f;

	//m_pd3dContext->RSSetViewports(1, &viewport);
	//m_pd3dContext->RSSetState(nullptr);

	//// 기본 깊이/스텐실 스테이트 (깊이 테스트 비활성화)
	//m_pd3dContext->OMSetDepthStencilState(nullptr, 0);

	//// 기본 블렌드 스테이트 (알파 블렌딩 비활성화, 불투명 렌더링)
	//m_pd3dContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	//// 8. 풀스크린 Quad 드로우
	//m_pd3dContext->Draw(6, 0);
	////m_pd3dContext->Flush();

	//// 9. SRV 해제
	//ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	//m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);

	
	// 10. 화면에 표시
	
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
	else
	{
		//DX::ThrowIfFailed(hr);
	}
}

