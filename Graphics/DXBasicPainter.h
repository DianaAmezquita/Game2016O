#pragma once
#include "DXManager.h"
#include "Matrix4D.h"

class CDXBasicPainter
{
protected:
	CDXManager* m_pManager;
	ID3D11InputLayout* m_pIL;
	ID3D11VertexShader* m_pVS;
	ID3D11PixelShader* m_pPS;
	ID3D11Buffer*      m_pCB;
	// Al basic painter le voy a dar un miembro que me diga donde dibujar
	ID3D11RenderTargetView* m_pRTV;
	ID3D11RasterizerState* m_pDrawLH;
	ID3D11RasterizerState* m_pDrawRH;
	ID3D11DepthStencilState* m_pDSSMask;
	ID3D11DepthStencilState* m_pDSSDrawOnMask;
	ID3D11DepthStencilState* m_pDSSDraw;
	
	// Soporta Sombras
	ID3D11ShaderResourceView* m_pSRVShadowMap; //Algunos autores les llaman mapas de luz
	ID3D11DepthStencilView* m_DSVShadowMap; // PAra poder construir el bufffer de profundidad
	ID3D11RenderTargetView* m_pRTVShadowMap; // Esto es para poder alicar el paso 1 order construir

	ID3D11VertexShader* m_pVSShadow;
	ID3D11PixelShader* m_pPSShadow;

#define PAINTER_DRAW_MARK		0x01
#define PAINTER_DRAW_ON_MARK	0x02
#define PAINTER_DRAW			0x04
public:
	void ClearShadow();
	void SetRenderTarget(ID3D11RenderTargetView* pRTV){ m_pRTV = pRTV; }
	ID3D11RasterizerState* GetDrawRHRState(){ return m_pDrawRH; }
	ID3D11RasterizerState* GetDrawLHRState(){ return m_pDrawLH; }
	struct MATERIAL // Definen como la luz es reflejada 
	{
		VECTOR4D Ambient; // Refleccion ambiental
		VECTOR4D Diffuse; // Como reflej la parte difusa
		VECTOR4D Specular; // COmo se reflejan los brillos
		VECTOR4D Emissive; // No esta en funcion de las fuentes de lu, en pocas palabras podemos hacer que un objeto brille de noche
		VECTOR4D Power; // Me da la manera de pulimento del objeto entre mas sea su valor es mas pulido
	};

	struct LIGHT // Las luces, se pueden definir de varios tipos: pointline (velas, lamaparas), direccional (sol), spotlights (enfoque, reflectores)
	{
		struct
		{
			unsigned long Flags, Type, notused0, notused1; // not used es para z y w que no se usan. las banderas es para indicar si las luces estan encendidas o apagadas y el type es para direccional, spothlight, etc
			#define LIGHT_ON 0x01


			
						//Types
			#define LIGHT_DIRECTIONAL 0
			#define LIGHT_POINT 1
			#define LIGHT_SPOT 2
		};
		VECTOR4D Ambient;
		VECTOR4D Diffuse; // EL color difuso
		VECTOR4D Specular;
		VECTOR4D Attenuation; // 1 / (x+(y*d))+(z(*d^2))) vigila el comportamiento energetico
		VECTOR4D Position; //Spotghlight or DIrectional
		VECTOR4D Direction; //Spotghlight or DIrectional
		VECTOR4D Factors; // La componente x nos ayuda a representar la potencia de spothlight (e)
	};
	struct PARAMS
	{
		struct
		{
			unsigned int Flags1, notused0, notused1, notused2;

		};
#define LIGHTING_AMBIENT			0x001
#define LIGHTING_DIFFUSE			0x002
#define LIGHTING_SPECULAR			0x004
#define LIGHTING_EMISSIVE			0x008
#define MAPPING_DIFFUSE				0x010
#define MAPPING_NORMAL				0x020
#define MAPPING_ENVIRONMENTAL_FAST	0x040
#define MAPPING_NORMAL_TRUE			0x080
#define MAPPING_EMISSIVE			0x100
#define MAPPING_SHADOW				0x200

#define SHADOW_MAP_RESOLUTION 1024//512 // resolucion de textura
		MATRIX4D World;
		MATRIX4D View;
		MATRIX4D Projection;
		MATRIX4D LightView; // con respecto a la luz
		MATRIX4D LightProjection; // con respecto a la luz. A lo mejor es isometrica o en perspectiva tu eliges el tipo de sombra
		VECTOR4D Brightness;
		MATERIAL Material;
		LIGHT lights[8];
	}m_Params;
	struct VERTEX
	{
		VECTOR4D Position;
		VECTOR4D Normal;
		VECTOR4D Tangent;
		VECTOR4D Binormal;
		VECTOR4D Color;
		VECTOR4D TexCoord;
		static D3D11_INPUT_ELEMENT_DESC InputLayout[];
	};
	CDXBasicPainter(CDXManager* pOwner);
	bool Initialize();
	void Uninitialize();

	void DrawIndexed(VERTEX* pVertices,
		unsigned long nVertices,
		unsigned long *pIndices,
		unsigned long nIndices,
		unsigned long flags,
		bool bShadow=false);

	~CDXBasicPainter();
};

