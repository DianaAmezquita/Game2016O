#pragma once
#include <d3d11.h>
#include "Matrix4D.h"
#include "DXManager.h"
#include "MeshMathSurface.h"
#include "DXBasicPainter.h"
#include <vector>
using namespace std;
class CFX
{
	CDXManager* m_pOwner;
	ID3D11InputLayout* m_pIL;
	ID3D11Buffer* m_pCB;
	ID3D11VertexShader* m_pVS;
	vector<ID3D11PixelShader*> m_vecFX;
	ID3D11ShaderResourceView* m_pSRVInput0;
	ID3D11ShaderResourceView** m_pSRVBrightPassed;
	ID3D11RenderTargetView* m_pRTVOutput;
	
	//ID3D11VertexShader* m_pVS;
	struct VERTEX
	{
		VECTOR4D Position;
		VECTOR4D TexCoord;
		VECTOR4D Color;

		static D3D11_INPUT_ELEMENT_DESC InputLayout[];
	};
	
	VERTEX m_vFrame[4];
	unsigned long   m_lIndicesFrame[6];
	
public:
	

	struct PARAMS
	{
		VECTOR4D Delta;
		VECTOR4D RadialBlur;
		VECTOR4D DirectionalBlur;
		VECTOR4D Umbral;
	}m_Params;
	CFX(CDXManager* pOwner);
	bool Initialize();
	void Process(unsigned long idEffect, float w, float h);
	void Uninitialize();
	void SetRenderTarget(ID3D11RenderTargetView* pRTV) { m_pRTVOutput = pRTV; }
	void SetInput(ID3D11ShaderResourceView* pSRV) { m_pSRVInput0 = pSRV; }
	void SetInputBrightPassed(ID3D11ShaderResourceView** pSRVBrightPassed) { m_pSRVBrightPassed = pSRVBrightPassed; }
	~CFX();
};

