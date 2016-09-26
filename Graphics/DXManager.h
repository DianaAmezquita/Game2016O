#pragma once
#include <dxgi.h>
#include <d3d11.h>

#define SAFE_RELEASE(X) {if((X)){ (X)->Release();(X)=NULL;}} 

class CDXManager
{
	IDXGISwapChain* m_pSwapChain;
	ID3D11Device*   m_pDevice;  //Fabricas de recursos GPU/CPU
	ID3D11DeviceContext* m_pContext; //Comandos GPU/CPU
	ID3D11RenderTargetView* m_pRTV; // Main Render Target
	ID3D11Texture2D* m_pDepthStencil; // buffer z
	ID3D11DepthStencilView* m_pDSV; // depth Stencil View

public:
	static IDXGIAdapter* EnumAndChooseAdapter(HWND hWnd);
	bool Initialize(HWND hWnd, IDXGIAdapter* pAdapter);
	void Resize(int cx, int cy); // que cuando la ventana cambie de tama;o tambien la cadena de intercambio
	void Uninitialize(void);
	IDXGISwapChain* GetSwapChain(){ return m_pSwapChain; }
	ID3D11Device* GetDevice(){ return m_pDevice; }
	ID3D11DeviceContext* GetContext(){ return m_pContext; }
	ID3D11RenderTargetView *&GetMainRTV(){ return m_pRTV; }
	ID3D11DepthStencilView*& GetMainDTV() { return m_pDSV; }
	ID3D10Blob* CompileShader(
		wchar_t* pszFileName, char* pszTarget,
		char* pszEntryPoint); //DXIL
	ID3D11VertexShader* CompileVertexShader(
		wchar_t* pszFileName, char*pszEntryPoint,
		ID3D10Blob** ppOutDXIL);
	ID3D11PixelShader* CompilePixelShader(
		wchar_t* pszFileName, char*pszEntryPoint);
	CDXManager();
	~CDXManager();
};

