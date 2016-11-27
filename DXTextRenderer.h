#pragma once
#include "Graphics\DXManager.h"
#include "Graphics\DXBasicPainter.h"
class CDXTextRenderer
{
protected:
	CDXManager* m_pOwner;
	CDXBasicPainter* m_pPainter;
	ID3D11ShaderResourceView* m_pSRVFont;
	ID3D11BlendState* m_pBS; //Blender es mezcladora
public:
	
	void CDXTextRenderer::Uninitialize();
	bool Initialize();
	void RenderText(MATRIX4D W, char const* pszText); 
	CDXTextRenderer(CDXManager* pManager, CDXBasicPainter* pPainter);
	~CDXTextRenderer();
};

