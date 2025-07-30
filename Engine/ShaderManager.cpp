#pragma once
#include "pch.h"
#include "ShaderManager.h"

HRESULT ShaderManager::ReadShaderBlob(const std::string& filePath, ID3DBlob** ppBlobOut)
{
	std::wstring wFilePath(filePath.begin(), filePath.end());
	return D3DReadFileToBlob(wFilePath.c_str(), ppBlobOut);
}

HRESULT ShaderManager::ReadShaderResource(const string& vertexShaderPath, 
										  const string& pixelShaderPath, const string& shaderName,
										  const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT numElements)
{
	if (m_shaderSets.count(shaderName)) {
	
		return S_FALSE;
	}
	if (!m_pDevice) {

		return E_FAIL;
	}

	ShaderSet newSet;
	HRESULT hr;
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> psBlob = nullptr;

	hr = ReadShaderBlob(vertexShaderPath, vsBlob.GetAddressOf());
	if (FAILED(hr)) return hr;
	hr = m_pDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, newSet.vs.GetAddressOf());
	if (FAILED(hr)) return hr;


	if (inputLayoutDesc && numElements > 0) {
		hr = m_pDevice->CreateInputLayout(inputLayoutDesc, numElements,
			vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), newSet.inputLayout.GetAddressOf());
		if (FAILED(hr)) return hr;
	}

	hr = ReadShaderBlob(pixelShaderPath, psBlob.GetAddressOf());
	if (FAILED(hr)) return hr;

	hr = m_pDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, newSet.ps.GetAddressOf());
	if (FAILED(hr)) return hr;

	// ���������� ������ ���̴� ������ �ʿ� �߰�
	m_shaderSets[shaderName] = newSet;

	return S_OK;

}

ShaderManager::ShaderManager(ID3D11Device* pDevice) : m_pDevice(pDevice)
{
	//���⼭ �� �� �о������
	assert(pDevice != nullptr);
	D3D11_INPUT_ELEMENT_DESC quadLayout[] = {
	   {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	   {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numQuadElements = ARRAYSIZE(quadLayout);

	//���⿡ ����� Shader �߰�, Vertex Shader���� ��� �⺻������ ����
	HRESULT hr;
	hr = ReadShaderResource("..\\Shader\\PassThrough_VS.cso", "..\\Shader\\PassThrough_PS.cso","PassThrough", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "���� ����" << endl;
	}
	hr = ReadShaderResource("..\\Shader\\PassThrough_VS.cso", "..\\Shader\\CRTFilter_PS.cso", "CRTFilter", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "���� ����" << endl;
	}
	hr = ReadShaderResource("..\\Shader\\SpriteShader_VS.cso", "..\\Shader\\SpriteShader_PS.cso", "SpriteShader", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "���� ����" << endl;
	}

}

const ShaderSet& ShaderManager::GetShaderSet(const string& shaderName)
{
	auto it = m_shaderSets.find(shaderName);
	if (it != m_shaderSets.end()) {
		return it->second;
	}
	return m_shaderSets.at("PassThrough"); // ��ã�� ���

}

void ShaderManager::ReleaseShaders()
{
	m_shaderSets.clear();
}
