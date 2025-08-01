#pragma once
#include<d3dcompiler.h>
#include <wrl/client.h>
#include "Singleton.h"
#include <string>
#include <map>
#include <d3d11.h>
//#include "SpriteManager.h"

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
	// ���� shader ������ ���� ���� �ʿ䰡 ���� normal, noise ��
private:
	//���̴� read
	ID3D11Device* m_pDevice;
	map<string, ShaderSet> m_shaderSets;
	map<string, ComPtr<ID3D11ShaderResourceView>> m_shaderResources;

	HRESULT ReadShaderBlob(const std::string& filePath, ID3DBlob** ppBlobOut);
	HRESULT ReadShaderResource(const string& vertexShaderPath, const string& pixelShaderPath,
							   const string& shaderName,
							   const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT numElements);
	void ReadAllShaders(); // ��� ���̴��� �о���� �Լ�
public:
	ShaderManager(ID3D11Device* pDevice); //������ �ڿ� read
	~ShaderManager() { ReleaseShaders(); }
	
	const ShaderSet& GetOBJShaderSet(const string& shaderName);
	const ShaderSet& GetPostShaderSet(const string& shaderName);

	const ID3D11ShaderResourceView* GetShaderResource(const string& resourceName);
	void ReleaseShaders();


};