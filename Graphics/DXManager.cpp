#include "stdafx.h"
#include "DXManager.h"
CDXManager::CDXManager()
{
	m_pContext = NULL;
	m_pDevice = NULL;
	m_pSwapChain = NULL;
	m_pRTV = NULL;
	m_pDSV = NULL;
	m_pDepthStencil = NULL;
}

CDXManager::~CDXManager()
{
	Uninitialize();
}

IDXGIAdapter* CDXManager::EnumAndChooseAdapter(HWND hWnd)
{
	IDXGIFactory* pFactory = NULL;
	CreateDXGIFactory(IID_IDXGIFactory, (void**)&pFactory);
	IDXGIAdapter* pAdapter = NULL;
	unsigned int iAdapter = 0;
	while (1)
	{
		if (FAILED(pFactory->EnumAdapters(iAdapter, &pAdapter)))
			break;
		DXGI_ADAPTER_DESC dad;
		pAdapter->GetDesc(&dad);
		wchar_t szMessage[1024];
		wsprintf(szMessage, L"Description:%s\r\nDedicated Video Memory:%dMB\r\nShared System Memory:%dMB\r\nDedicated System Memory:%dMB\r\n",
			dad.Description,
			dad.DedicatedVideoMemory / (1024 * 1024),
			dad.SharedSystemMemory / (1024 * 1024),
			dad.DedicatedSystemMemory / (1024 * 1024)
			);
		switch (MessageBox(hWnd, szMessage,
			L"Whould you use this device?",
			MB_ICONQUESTION | MB_YESNOCANCEL))
		{
		case IDYES:
			pFactory->Release();
			return pAdapter;
		case IDNO:
			pAdapter->Release();
			break;
		case IDCANCEL:
			pAdapter->Release();
			pFactory->Release();
			return NULL;
		}
		iAdapter++;
	}
	pFactory->Release();
	return NULL;
}

bool CDXManager::Initialize(HWND hWnd, 
	IDXGIAdapter* pAdapter)
{

	D3D_FEATURE_LEVEL FeatureLevel =
		D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC dscd;
	memset(&dscd, 0, sizeof(dscd));
	dscd.BufferCount = 2;
	dscd.BufferUsage = 
		DXGI_USAGE_RENDER_TARGET_OUTPUT |
		DXGI_USAGE_UNORDERED_ACCESS;
	dscd.OutputWindow = hWnd;
	dscd.Flags = 0;
	dscd.Windowed = true;
	dscd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	RECT rc;
	GetClientRect(hWnd, &rc);
	dscd.BufferDesc.Width = rc.right;
	dscd.BufferDesc.Height = rc.bottom;
	dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	dscd.BufferDesc.ScanlineOrdering =
		DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	dscd.BufferDesc.RefreshRate.Numerator = 0;
	dscd.BufferDesc.RefreshRate.Denominator = 0;
	dscd.SampleDesc.Count = 1;
	dscd.SampleDesc.Quality = 0;
	D3D_FEATURE_LEVEL DetectedFeatureLevel;
	HRESULT hr =
		D3D11CreateDeviceAndSwapChain(pAdapter,
		D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, &FeatureLevel,
		1, D3D11_SDK_VERSION, &dscd, &m_pSwapChain,
		&m_pDevice, &DetectedFeatureLevel, &m_pContext);

	if (FAILED(hr))
		return false;

	ID3D11Texture2D* pBackBuffer = 0;
	m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
	m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTV);

	//Creacion del buffer z
	D3D11_TEXTURE2D_DESC dtd;
	pBackBuffer->GetDesc(&dtd);
	dtd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // quiero esos 8 bits adicionales para hacer efectos chidos
	dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	m_pDevice->CreateTexture2D(&dtd, NULL, &m_pDepthStencil);

	m_pDevice->CreateDepthStencilView(m_pDepthStencil, NULL, &m_pDSV);

	SAFE_RELEASE(pBackBuffer);
	return true;
}
void CDXManager::Resize(int cx, int cy)
{
	if (m_pDevice && m_pSwapChain)
	{

		SAFE_RELEASE(m_pDSV);
		SAFE_RELEASE(m_pRTV);
		SAFE_RELEASE(m_pDepthStencil);

		m_pContext->ClearState();
		m_pSwapChain->ResizeBuffers(2, cx, cy, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		ID3D11Texture2D* pBackBuffer = 0;
		m_pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer);
		m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRTV);

		//Creacion del buffer z
		D3D11_TEXTURE2D_DESC dtd;
		pBackBuffer->GetDesc(&dtd);
		dtd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dtd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		m_pDevice->CreateTexture2D(&dtd, NULL, &m_pDepthStencil);
		m_pDevice->CreateDepthStencilView(m_pDepthStencil, NULL, &m_pDSV);

		SAFE_RELEASE(pBackBuffer);
	}
}
void CDXManager::Uninitialize()
{
	SAFE_RELEASE(m_pContext);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pRTV);
	SAFE_RELEASE(m_pDSV);
	SAFE_RELEASE(m_pDepthStencil);
}
#include <d3dcompiler.h>
ID3D10Blob* CDXManager::CompileShader(
	wchar_t* pszFileName,
	char* pszTarget,
	char* pszEntryPoint)
{
	//DirectX Intermediate Language
	ID3D10Blob* pDXIL=NULL;
	ID3D10Blob* pErrors = NULL;
	HRESULT hr = D3DCompileFromFile(
		pszFileName, NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pszEntryPoint, pszTarget,
#ifdef _DEBUG
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 
#else
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3,
#endif
		0,
		&pDXIL, &pErrors
		);
	if (pErrors)
	{
		MessageBoxA(NULL,
			(char*)pErrors->GetBufferPointer(),
			"Se han encontrado errores o advertencias",
			MB_ICONEXCLAMATION);
		SAFE_RELEASE(pErrors);
	}
	if (pDXIL)
	{
		return pDXIL;
	}
	return NULL;
}

ID3D11VertexShader* CDXManager::CompileVertexShader(
	wchar_t* pszFileName, char*pszEntryPoint,
	ID3D10Blob** ppOutDXIL)
{
	ID3D11VertexShader* pVS = NULL;
	*ppOutDXIL = CompileShader(pszFileName,
		"vs_5_0", pszEntryPoint);
	if (*ppOutDXIL)
	{
		HRESULT hr = m_pDevice->CreateVertexShader(
			(*ppOutDXIL)->GetBufferPointer(),
			(*ppOutDXIL)->GetBufferSize(), NULL, &pVS);
		if (FAILED(hr))
		{
			SAFE_RELEASE(*ppOutDXIL);
			return NULL;
		}
		return pVS;
	}
	return NULL;
}
ID3D11PixelShader* CDXManager::CompilePixelShader(
	wchar_t* pszFileName, char*pszEntryPoint)
{
	ID3D11PixelShader* pPS = NULL;
	ID3D10Blob* pDXIL = CompileShader(pszFileName,
		"ps_5_0", pszEntryPoint);
	if (pDXIL)
	{
		HRESULT hr = m_pDevice->CreatePixelShader(
			pDXIL->GetBufferPointer(),
			pDXIL->GetBufferSize(), NULL, &pPS);
		SAFE_RELEASE(pDXIL);
		if (FAILED(hr))
			return NULL;
		return pPS;
	}
	return NULL;
}

