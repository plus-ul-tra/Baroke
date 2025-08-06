#pragma once
#include "pch.h"
#include "ShaderManager.h"
#include "SpriteManager.h"
#include "FileDirectory.h"
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

void ShaderManager::ReadAllShaders()
{
	D3D11_INPUT_ELEMENT_DESC quadLayout[] = {
	   {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	   {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT numQuadElements = ARRAYSIZE(quadLayout);
	// ����� ���̴� ���⿡ �߰�
	HRESULT hr;

	hr = ReadShaderResource("..\\Shader\\PassThrough_VS.cso", "..\\Shader\\PassThrough_PS.cso", "PassThrough", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "PassThrough���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\PassThrough_VS.cso", "..\\Shader\\CRTFilter_PS.cso", "CRTFilter", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "CRTFilter���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\DefaultShader_PS.cso", "DefaultShader", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "DefaultShader���� ����" << endl;
	}

	// noise
	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\NoiseBlend_PS.cso", "NoiseBlend", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "NoiseBlend���� ����" << endl;
	}

	// GrayScale
	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\GrayScale_PS.cso", "GrayScale", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "GrayScale���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\Holo_PS.cso", "Holo", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "Holo���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\PassThrough_VS.cso", "..\\Shader\\BlackHole_PS.cso", "BlackHole", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "black���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\Othello_PS.cso", "Othello", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "Othello���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\SetRed_PS.cso", "SetRed", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "SetRed���� ����" << endl;
	}

	hr = ReadShaderResource("..\\Shader\\DefaultShader_VS.cso", "..\\Shader\\Checker_PS.cso", "Checker", quadLayout, numQuadElements);
	if (FAILED(hr)) {
		cout << "Checker���� ����" << endl;
	}
}



ShaderManager::ShaderManager(ID3D11Device* pDevice) : m_pDevice(pDevice)
{
	
	//���⿡ ����� ��� Shader �߰�, Vertex Shader���� ��� �⺻������ ����
	
	ReadAllShaders();

	// ���� shader �ڿ� load
}

// ������Ʈ�� ���̴� �˻�
const ShaderSet& ShaderManager::GetOBJShaderSet(const string& shaderName)
{	
	auto it = m_shaderSets.find(shaderName);
	if (it != m_shaderSets.end()) {
		return it->second;
	}
	return m_shaderSets.at("NoiseBlend"); // ��ã�� ���
}

// ����Ʈ ���μ��̿� ���̴� �˻�
const ShaderSet& ShaderManager::GetPostShaderSet(const string& shaderName)
{	
	auto it = m_shaderSets.find(shaderName);
	if (it != m_shaderSets.end()) {
		return it->second;
	}
	return m_shaderSets.at("PassThrough"); // ��ã�� ���

}

const ID3D11ShaderResourceView* ShaderManager::GetShaderResource(const string& resourceName)
{
	auto it = m_shaderResources.find(resourceName);
	if (it != m_shaderResources.end()) {
		return it->second.Get();
	}
	return nullptr; // ��ã�� ���
}

void ShaderManager::ReleaseShaders()
{
	m_shaderSets.clear();
	m_shaderResources.clear();
}
