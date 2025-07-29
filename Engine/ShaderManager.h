#pragma once
#include<d3dcompiler.h>
#include <wrl/client.h>
#include "Singleton.h"
#include <string>
#include <map>
#include <d3d11.h>

using namespace std;
using namespace Microsoft::WRL;

struct ShaderSet {
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11PixelShader>  ps;
	ComPtr<ID3D11InputLayout> inputLayout;
	//�ʿ�� inputlayout;
};

// Renderer���� �� ���̴� â��
class ShaderManager {
	
private:
	//���̴� read
	ID3D11Device* m_pDevice;
	map<string, ShaderSet> m_shaderSets;
	HRESULT ReadShaderBlob(const std::string& filePath, ID3DBlob** ppBlobOut);

	HRESULT ReadShaderResource(const string& vertexShaderPath, const string& pixelShaderPath,
							   const string& shaderName,
							   const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT numElements);

public:
	ShaderManager(ID3D11Device* pDevice); //������ �ڿ� read
	~ShaderManager() { ReleaseShaders(); }
	const ShaderSet& GetShaderSet(const string& shaderName);
	void ReleaseShaders();


};