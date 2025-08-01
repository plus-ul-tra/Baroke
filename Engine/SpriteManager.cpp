#include "pch.h"
#include "SpriteManager.h"
#include "SpriteParser.h"
#include "FileDirectory.h"

#include <locale>
#include <codecvt>
#include <stdexcept>
#include <Windows.h>


void SpriteManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	if (!m_pDevice || !m_pDeviceContext) {
		OutputDebugStringA("ERROR: SpriteManager::Initialize - D3D Device or DeviceContext is null.\n");
		throw std::runtime_error("SpriteManager: D3D Device or DeviceContext is null during initialization.");
	}

	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pWICFactory)
	);
	if (FAILED(hr)) {
		OutputDebugStringA("ERROR: SpriteManager::Initialize - Failed to create WIC Imaging Factory.\n");
		throw std::runtime_error("SpriteManager: Failed to create WIC Imaging Factory.");
	}

	LoadAll();
}

// png -> RSV 전환 로직 필요
void SpriteManager::LoadAll()
{

	filesystem::path solutionRoot;
	try
	{
		solutionRoot = FindSolutionRoot();
	}
	catch (...)
	{
		solutionRoot = GetExecutableDir();
	}
	filesystem::path resourcePath = solutionRoot / L"Resource" / L"Sprits";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(resourcePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension() == L".png")
			{
				LoadTextureSRV(entry.path()); 
			}
			if (entry.path().extension() == L".json")
			{
				LoadAnimationClips(entry.path());
			}
		}
	}
	OutputDebugStringA("INFO: All sprites loaded.\n");
}



ComPtr<ID3D11ShaderResourceView> SpriteManager::LoadTextureSRV(const filesystem::path& filePath)
{
	string key = filePath.filename().string();
	auto it = m_textures.find(key);

	if (it != m_textures.end()) {
		return it->second;
	}


	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<IWICBitmapDecoder> decoder;
	ComPtr<IWICBitmapFrameDecode> frame;
	ComPtr<IWICFormatConverter> converter;

	/*std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strConverter;
	std::wstring wFilePath = strConverter.from_bytes(filePath.string());*/


	HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
		filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf()
	);
	if (FAILED(hr)) { /*예외코드*/return nullptr; }
	hr = decoder->GetFrame(0, frame.GetAddressOf());
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }
	hr = m_pWICFactory->CreateFormatConverter(converter.GetAddressOf());
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }
	hr = converter->Initialize(
		frame.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom
	);
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }

	UINT width, height;
	hr = converter->GetSize(&width, &height);
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }

	std::vector<BYTE> pixels(static_cast<size_t>(width) * height * 4);
	hr = converter->CopyPixels(nullptr, width * 4, pixels.size(), pixels.data());
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }


	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width; texDesc.Height = height; texDesc.MipLevels = 1; texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1; texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE; texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = pixels.data();
	initData.SysMemPitch = width * 4;

	ComPtr<ID3D11Texture2D> texture;
	hr = m_pDevice->CreateTexture2D(&texDesc, &initData, texture.GetAddressOf());
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format; srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels; srvDesc.Texture2D.MostDetailedMip = 0;

	hr = m_pDevice->CreateShaderResourceView(texture.Get(), &srvDesc, srv.GetAddressOf());
	if (FAILED(hr)) { /*예외코드*/ return nullptr; }

	m_textures.emplace(key, srv);
	OutputDebugStringA(("INFO: Successfully loaded D3D11 SRV for " + key + "\n").c_str());
	return srv;
}


void SpriteManager::LoadAnimationClips(const filesystem::path& filePath)
{
	std::string key = filePath.filename().string();
	if (m_animationClips.count(key)) {
		OutputDebugStringA(("INFO: Animation clips '" + key + "' already loaded.\n").c_str());
		return;
	}

	filesystem::path texturePath = filePath;
	texturePath.replace_extension(L".png");

	ComPtr<ID3D11ShaderResourceView> textureSRV = LoadTextureSRV(texturePath);
	if (!textureSRV) { /* ... error handling ... */ return; }

	AnimationClipsMap clips = SpriteParser::Load(filePath); // 수정

	for (auto& pair : clips) {
		pair.second.SetTextureSRV(textureSRV);
	}

	m_animationClips.emplace(key, std::move(clips));
	OutputDebugStringA(("INFO: Animation clips loaded for: " + key + "\n").c_str());
}



const ComPtr<ID3D11ShaderResourceView> SpriteManager::GetTextureSRV(const string& key) const
{
	auto it = m_textures.find(key);
	if (it == m_textures.end()) {
		OutputDebugStringA(("ERROR: Texture SRV '" + key + "' not found.\n").c_str());
		return nullptr;
	}
	return it->second;
}

const AnimationClipsMap& SpriteManager::GetAnimationClips(const string& key) const
{
	auto it = m_animationClips.find(key);
	if (it == m_animationClips.end()) {
		OutputDebugStringA(("ERROR: Animation clips for '" + key + "' not found.\n").c_str());
		throw std::runtime_error("Animation clips not found: " + key);
	}
	return it->second;
}


bool SpriteManager::IsAnimatedSprite(const string& key) const
{
	return m_animationClips.count(key) > 0;
}


void SpriteManager::Release()
{
	m_textures.clear();
	m_animationClips.clear();
	m_pWICFactory.Reset();
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;

	//OutputDebugStringA("INFO: SpriteManager resources released.\n");
}