#include "pch.h"
#include "Renderer.h"

void Renderer::Initialize(HWND hwnd)
{
	
	m_hwnd = hwnd;

	CreateDeviceAndSwapChain(hwnd);
	CreateRenderTargets();
	CreateWriteResource();

	//Window Imaging Component factory 생성
	
	ComPtr<IWICImagingFactory> wicFactory;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&wicFactory));
	
	//예외처리 생략
	m_pwicFactory = wicFactory;
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
}

void Renderer::ReleaseRenderTargets()
{
	if (m_pd2dContext)
	{
		m_pd2dContext->SetTarget(nullptr);
	}

	m_pd3dRenderTV.Reset();

	m_ptargetBitmap.Reset();
	m_pbrush.Reset();
	m_ptextBrush.Reset();
}

void Renderer::Resize(UINT width, UINT height)
{
	if (nullptr == m_pswapChain) return;
	ReleaseRenderTargets();
	CreateRenderTargets();

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

//void Renderer::SetTransform(const D2D1_MATRIX_3X2_F tm)
//{
//	if (m_pd2dContext) m_pd2dContext->SetTransform(tm);
//}

void Renderer::RenderBegin()
{
	m_pd2dContext->BeginDraw();
	m_pd2dContext->Clear(D2D1::ColorF(D2D1::ColorF::BurlyWood)); // 배경을 초기화
}
//void Renderer::RenderBeginTest()
//{
//	m_pd2dContext->BeginDraw();
//	m_pd2dContext->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue)); 
//}


void Renderer::RenderEnd() {
	m_pd2dContext->EndDraw();
	Present();
}

void Renderer::RenderEnd(bool bpresent)
{
	m_pd2dContext->EndDraw();

	if (bpresent)
	{
		Present();
	}
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
	ComPtr<ID2D1Factory8> d2dFactory;

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

void Renderer::CreateRenderTargets()
{
	// 6. SwapChain 백버퍼 -> D2D Bitmap1 을 생성하여 렌더 타겟으로 설정

	ComPtr<IDXGISurface> dxgiSurface;
	HRESULT hr = m_pswapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiSurface));

	//DX::ThrowIfFailed(hr);

	D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
	bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bitmapProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProps.dpiX = bitmapProps.dpiY = 96.0f;

	ComPtr<ID2D1Bitmap1> targetBitmap;
	hr = m_pd2dContext->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), &bitmapProps, targetBitmap.GetAddressOf());

	//DX::ThrowIfFailed(hr);

	// 렌더 타겟 설정
	m_pd2dContext->SetTarget(targetBitmap.Get());

	m_ptargetBitmap = targetBitmap;

	hr = m_pd2dContext->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue),
		&m_pbrush);

	//DX::ThrowIfFailed(hr);

	// ImGUI 은 D3D11 렌더 타겟 뷰가 필요!!! 
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = m_pswapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	// RenderTargetView 생성
	ComPtr<ID3D11RenderTargetView> mainRTV;
	hr = m_pd3dDevice->CreateRenderTargetView(
		backBuffer.Get(),      // 텍스처
		nullptr,               // 기본 뷰 설명
		&mainRTV               // 출력 RTV 포인터
	);

	//DX::ThrowIfFailed(hr);

	m_pd3dRenderTV = mainRTV;
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