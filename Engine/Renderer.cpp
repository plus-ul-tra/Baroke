#include "pch.h"
#include "Renderer.h"

//void Renderer::SetTransform(const D2D1_MATRIX_3X2_F tm)
//{
//	if (m_pd2dContext) m_pd2dContext->SetTransform(tm);
//}

void Renderer::CreateDeviceAndSwapChain(HWND hwnd)
{
	//1. D3D11 ����̽� ����
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

	// 2. DXGI ����ü�� ����
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

	// 3. ID2D1Factory4 ����
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

	// 4. ID2D1Device4 ����
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

	// 5. ID2D1DeviceContext7 ����
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

	m_ptextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING); // ���� ����
	m_ptextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR); // ���� ����
	m_ptextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP); // �ٹٲ�
}


void Renderer::CreateFullScrennQuad()
{
	Vertex quadVertices[] =
	{
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

	// 1. Direct3D ����ۿ� RenderTargetView ���� (���� Present �� D3D�� �������ϴ� ���)
	hr = m_pd3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_pd3dRenderTV);

	m_screenWidth = backBufferDesc.Width;
	m_screenHeight = backBufferDesc.Height;
	// 2. Direct2D�� �׸� Off-screen �ؽ�ó (Shader Resource�� ���� �ؽ�ó) ����
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = m_screenWidth;
	texDesc.Height = m_screenHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // D2D�� ������ ����
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // D2D�� �׸��� ���̴��� ����

	hr = m_pd3dDevice->CreateTexture2D(&texDesc, nullptr, &m_renderTargetTex);
	// 3. Off-screen �ؽ�ó�� ���� RenderTargetView (Direct2D�� �� �ؽ�ó�� �׸�)
	hr = m_pd3dDevice->CreateRenderTargetView(m_renderTargetTex.Get(), nullptr, &m_offScreenTargetView);
	// 4. Off-screen �ؽ�ó�� ���� ShaderResourceView (Direct3D ���̴��� �� �ؽ�ó�� ����)
	hr = m_pd3dDevice->CreateShaderResourceView(m_renderTargetTex.Get(), nullptr, &m_renderTargetSRV);
	// 5. Off-screen �ؽ�ó�� Direct2D Bitmap���� ���� (Direct2D�� ������ ���)
	
	// this
	ComPtr<IDXGISurface> dxgiOffscreenSurface;
	hr = m_renderTargetTex.As(&dxgiOffscreenSurface); // m_pOffscreenRenderTargetTexture ���
	
	

	D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
	bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET; // CANNOT_DRAW ����

	hr = m_pd2dContext->CreateBitmapFromDxgiSurface(dxgiOffscreenSurface.Get(), &bitmapProps, &m_ptargetBitmap);

	// 6. Direct2D ���ؽ�Ʈ�� ���� Ÿ���� �� off-screen ��Ʈ������ ���� (������ ���� ���� �ٽ� ������ ���� ����)
	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());

	// 7. �⺻ D2D �귯�� ����
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
	
	m_hwnd = hwnd;

	CreateDeviceAndSwapChain(hwnd);

	InitializeShader(hwnd);
	CreateShaderRenderTargets();
	CreateFullScrennQuad();
	//CreateTimeShaderBuffer();
	CreateWriteResource();

	//Window Imaging Component factory ����
	ComPtr<IWICImagingFactory> wicFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory));
	
	//����ó�� ����
	m_pwicFactory = wicFactory;
}

void Renderer::InitializeShader(HWND hwnd)
{
	// D3D ����̽��� ��ȿ���� Ȯ��
	if (!m_pd3dDevice) {
		std::cerr << "ERROR: D3D Device not created. Cannot initialize shaders." << std::endl;
		return;
	}

	// ���̴� �ε� �� ���� 
	ComPtr<ID3DBlob> vsBlob = nullptr; // ���� ���̴� ����Ʈ
	ComPtr<ID3DBlob> psBlob = nullptr; // �ȼ� ���̴� ����Ʈ
	HRESULT hr;

	hr = D3DReadFileToBlob(L"..\\Shader\\PassThrough_VS.cso", &vsBlob);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to read Vertex Shader CSO! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// �ȼ� ���̴��� ������ ���Ͽ��� �ε��մϴ�.
	hr = D3DReadFileToBlob(L"..\\Shader\\PassThrough_PS.cso", &psBlob);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to read Pixel Shader CSO! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// 2. ���̴� ��ü ����
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

	// 3. �Է� ���̾ƿ� ����
	// HLSL ���̴��� VS_INPUT ����ü�� ��ġ
	// Ǯ��ũ�� ����� ���� �����ʹ� ���� {��ġ, �ؽ�ó ��ǥ}�� ����

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	UINT numElements = ARRAYSIZE(layout);

	// �Է� ���̾ƿ��� ���� ���̴��� ����Ʈ �ڵ带 ������� ����
	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Input Layout! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

	// 4. ���÷� ���� ����
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // �ε巯�� ���͸�
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;   // �ؽ�ó ��ǥ�� ������ ��� �� Ŭ����
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX; // ��� �Ӹ� ���� ���

	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_samplerState);
	if (FAILED(hr)) {
		std::cerr << "ERROR: Failed to create Sampler State! HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}

}

void Renderer::Uninitialize()
{
	ReleaseRenderTargets();

	m_pwicFactory = nullptr;

	m_ptargetBitmap = nullptr;
	m_pbrush = nullptr;

	m_pd2dContext = nullptr;
	m_pd2dDevice = nullptr;

	m_pswapChain = nullptr;
	m_pd3dDevice = nullptr;

	//�ʱ�ȭ �߰��ؾ���
}

void Renderer::Resize(UINT width, UINT height)
{
	if (nullptr == m_pswapChain) return;
	ReleaseRenderTargets();
	//CreateRenderTargets();
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

void Renderer::DrawMessage(const wchar_t* text, float left, float top, float width, float height, const D2D1::ColorF& color)
{
	if (nullptr == m_ptextBrush)
	{
		HRESULT hr = m_pd2dContext->CreateSolidColorBrush(D2D1::ColorF(color), &m_ptextBrush);
	}
	m_ptextBrush->SetColor(color);
	D2D1_RECT_F layoutRect = D2D1::RectF(left, top, left + width, top + height);
	m_pd2dContext->DrawTextW(
		text,
		static_cast<UINT32>(wcslen(text)),
		m_ptextFormat.Get(),
		layoutRect,
		m_ptextBrush.Get(),
		D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL);

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

	// Direct2D ��Ʈ�� �Ӽ� (premultiplied alpha, B8G8R8A8_UNORM)
	D2D1_BITMAP_PROPERTIES1 bmpProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_NONE,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	// �� DeviceContext���� WIC ��Ʈ�����κ��� D2D1Bitmap1 ����
	hr = m_pd2dContext->CreateBitmapFromWicBitmap(converter.Get(), &bmpProps, &outBitmap);


}

void Renderer::RenderBegin()
{
	if (!m_pd2dContext || !m_offScreenTargetView) {
		std::cerr << "ERROR: D2D context or shader render target view not initialized for ShaderRenderBegin." << std::endl;
		return;
	}

	// 1. Direct3D ���� Ÿ���� off-screen �ؽ�ó�� ���� (D2D�� �׸� ���)
	ID3D11RenderTargetView* rtv = m_offScreenTargetView.Get();
	m_pd3dContext->OMSetRenderTargets(1, &rtv, nullptr); 

	// 2. Direct3D off-screen �ؽ�ó�� Ŭ���� (�ʿ��ϴٸ�)
	float clearColor[4] = { 0.0f, 1.0f, 1.0f, 1.0f }; 
	m_pd3dContext->ClearRenderTargetView(m_offScreenTargetView.Get(), clearColor);

	m_pd2dContext->SetTarget(m_ptargetBitmap.Get());

	m_pd2dContext->BeginDraw();

	// D2D ����� ���� �� D2D ���� Ÿ�ٵ� Ŭ����
	m_pd2dContext->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray)); // D2D�� �׸� ������ Ŭ����
	
}

void Renderer::RenderEnd()
{
	if (!m_pd2dContext) {
		std::cerr << "ERROR: D2D context not initialized for ShaderRenderEnd." << std::endl;
		return;
	}

	// 1. Direct2D ����� ����
	HRESULT hr = m_pd2dContext->EndDraw();
	if (FAILED(hr) && hr != D2DERR_RECREATE_TARGET)
	{
		std::cerr << "D2D EndDraw failed: HRESULT: 0x" << std::hex << hr << std::endl;
		return;
	}
	//m_pd3dContext->Flush();
	// 
	// 2. ����� RTV�� Ÿ�� ����
	ID3D11RenderTargetView* backBufferRTV = m_pd3dRenderTV.Get();
	m_pd3dContext->OMSetRenderTargets(1, &backBufferRTV, nullptr);

	// 3. ����� Ŭ����
	float backBufferClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //black;
	m_pd3dContext->ClearRenderTargetView(backBufferRTV, backBufferClearColor);

	// 4. ���̴� ���ҽ� üũ
	if (!m_InputLayout || !m_VertexShader || !m_PixelShader || !m_samplerState || !m_fullScreenVB)
	{
		std::cerr << "ERROR: Shader pipeline resources not initialized." << std::endl;
		return;
	}

	// 5. IA �ܰ� ����
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_pd3dContext->IASetInputLayout(m_InputLayout.Get());
	m_pd3dContext->IASetVertexBuffers(0, 1, m_fullScreenVB.GetAddressOf(), &stride, &offset);
	m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 6. ���̴� ����
	m_pd3dContext->VSSetShader(m_VertexShader.Get(), nullptr, 0);
	m_pd3dContext->PSSetShader(m_PixelShader.Get(), nullptr, 0);

	// 7. D2D�� �׸� �ؽ�ó ���ε�
	ID3D11ShaderResourceView* srvs[] = { m_renderTargetSRV.Get() };
	m_pd3dContext->PSSetShaderResources(0, 1, srvs);
	m_pd3dContext->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	viewport.Width = static_cast<float>(m_screenWidth);
	viewport.Height = static_cast<float>(m_screenHeight);

	viewport.MinDepth = 0.0f; 
	viewport.MaxDepth = 1.0f;

	m_pd3dContext->RSSetViewports(1, &viewport);
	m_pd3dContext->RSSetState(nullptr);

	// �⺻ ����/���ٽ� ������Ʈ (���� �׽�Ʈ ��Ȱ��ȭ)
	m_pd3dContext->OMSetDepthStencilState(nullptr, 0);

	// �⺻ ���� ������Ʈ (���� ���� ��Ȱ��ȭ, ������ ������)
	m_pd3dContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

	// 8. Ǯ��ũ�� Quad ��ο�
	m_pd3dContext->Draw(6, 0);
	//m_pd3dContext->Flush();

	// 9. SRV ����
	ID3D11ShaderResourceView* nullSRV[] = { nullptr };
	m_pd3dContext->PSSetShaderResources(0, 1, nullSRV);

	//ID3D11Buffer* nullBuffers[] = { nullptr };
	//m_pd3dContext->PSSetConstantBuffers(0, 1, nullBuffers);
	
	// 10. ȭ�鿡 ǥ��
	Present();

	
}

void Renderer::Present()
{
	// ������ �۾��� ������ ����ü�ο� �������� ǥ��
	HRESULT hr = m_pswapChain->Present(1, 0);

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		Uninitialize();     // ����̽��� ���ŵǰų� ���µ� ���, ���ʱ�ȭ �ʿ�
		Initialize(m_hwnd);
	}
	else
	{
		//DX::ThrowIfFailed(hr);
	}
}

