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
	//필요시 inputlayout;
};

// Renderer에서 쓸 셰이더 창고
class ShaderManager {
	// 공용 shader 파일을 갖고 있을 필요가 있음 normal, noise 등
private:
	//쉐이더 read
	ID3D11Device* m_pDevice;
	map<string, ShaderSet> m_shaderSets;
	map<string, ComPtr<ID3D11ShaderResourceView>> m_shaderResources;

	HRESULT ReadShaderBlob(const std::string& filePath, ID3DBlob** ppBlobOut);
	HRESULT ReadShaderResource(const string& vertexShaderPath, const string& pixelShaderPath,
							   const string& shaderName,
							   const D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT numElements);
	void ReadAllShaders(); // 모든 셰이더를 읽어오는 함수
public:
	ShaderManager(ID3D11Device* pDevice); //생성시 자원 read
	~ShaderManager() { ReleaseShaders(); }
	
	const ShaderSet& GetOBJShaderSet(const string& shaderName);
	const ShaderSet& GetPostShaderSet(const string& shaderName);

	const ID3D11ShaderResourceView* GetShaderResource(const string& resourceName);
	void ReleaseShaders();


};