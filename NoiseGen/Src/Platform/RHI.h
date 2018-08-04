#ifndef _RHI_H_
#define _RHI_H_

// This hardware interface allows to agnostically represent API types and functions

#include "Core\coretypes.h"

// This includes all the native types for D3D11
#ifdef WIN32
#include "Win/DX11_RHI.h"
#else 
#pragma ERROR
#endif

// Enums which are shared across many API calls
enum RHI_Comp
{
	RHIDC_Unknown,
	RHIDC_Never,
	RHIDC_Less,
	RHIDC_LessEqual,
	RHIDC_Greater,
	RHIDC_GreaterEqual,
	RHIDC_Always,
	RHIDC_MAX
};

/*
 * TEXTURE
 */
// Helper Function for texture create
enum eTEXTUREFORMAT
{
	TEXFMT_UNKNOWN,
	TEXFMT_R8G8B8A8,
	TEXFMT_R8G8B8A8_SRGB,
	TEXFMT_R8_SNORM,
	TEXFMT_R16_FLOAT,
	TEXFMT_D32,
	TEXFMT_D24S8,
	TEXFMT_MAX,
};

struct TEXTURE_DESC
{
	Uint32			Width;
	Uint32			Height;
	Uint32			MipLevels;
	Uint32			ArraySize;
	eTEXTUREFORMAT	Format;

	TEXTURE_DESC(Uint32 W, Uint32 H);
	TEXTURE_DESC(Uint32 W, Uint32 H, eTEXTUREFORMAT FMT);
};


/*
 * RENDER/DEPTH TARGET
 */
struct RHI_VIEWPORT
{
	float X,Y, W,H, ZMin,ZMax;
};

struct RHI_RECT
{
	float L,B, R,T;
};

TEXTURE2D		RHI_CreateTexture2D(DEVICE device, TEXTURE_DESC &Desc);
TEXTURE2D		RHI_CreateTexture2DFromFile(DEVICE device, WCHAR *filename);
DEPTHSTENCIL	RHI_CreateDepthStencil(DEVICE device, TEXTURE_DESC &desc);
RENDERTARGET	RHI_CreateRenderTarget(DEVICE device, TEXTURE_DESC &desc);

void			RHI_SetViewport(DEVICE device,RHI_VIEWPORT *viewport);
void			RHI_SetScissorRect(DEVICE device,RHI_RECT *rect);

//void			RHI_SetDepthStencil(DEVICE device, DEPTHSTENCIL *depthStencil);
//void			RHI_SetRenderTarget(DEVICE device, DEPTHSTENCIL *RENDERTARGET);

void			RHI_SetRenderTargets(DEVICE device, Uint32 NumRTs, RENDERTARGET *renderTarget, DEPTHSTENCIL *depthStencil);


/*
 * VERTEX BUFFER 
 */
VERTEXBUFFER	RHI_CreateVertexBuffer(Uint32 NumVerts, Uint32 VertexStride );
VERTEXBUFFER	RHI_CreateAndFillVertexBuffer(DEVICE device, Uint32 NumVerts,	Uint32 VertSize, void *FillData);
void			RHI_SetVertexBuffer(DEVICE &device, VERTEXBUFFER &VB);

/*
 * INDEX BUFFER
 */
INDEXBUFFER		RHI_CreateIndexBuffer(Uint32 NumVerts );
INDEXBUFFER		RHI_CreateAndFillIndexBuffer( DEVICE device, Uint32 IdxSz, Uint32 NumIndices, void *FillData);
void			RHI_SetIndexBuffer(DEVICE &device, INDEXBUFFER &IB);

/*
 * INPUT LAYOUT
 */
// Vertex InputLayout TYPES
enum eINPUTLAYOUT_TYPES
{
	ILT_UNKNOWN, 
	ILT_FLOAT_2,
	ILT_FLOAT_3,
	ILT_FLOAT_4,
	ILT_HALF_2,
	ILT_HALF_4,
	ILT_SHORT2,
	ILT_SHORT4,
	ILT_UBYTE_4,
	ILT_MAX,
};

enum eINPUTLAYOUT_USAGE
{
	ILU_UNKNOWN,
	ILU_POSITION,
	ILU_TEXCOORD,
	ILU_MAX,
};

struct InputLayoutElement
{
	eINPUTLAYOUT_TYPES type;
	eINPUTLAYOUT_USAGE usage;
};

INPUTLAYOUT RHI_CreateInputLayout(DEVICE device, Uint32 NumElements, InputLayoutElement *pElements);

/*
 *   DEPTH STENCIL STATE
 */
enum eDEPTHSTENCILSTATE_FLAGS
{
	eDEPTHSTENCILSTATE_UNKNOWN,
	eDEPTHSTENCILSTATE_DepthTest,
	eDEPTHSTENCILSTATE_DepthWrite,
};

struct RHI_DepthStencilDesc
{
						RHI_DepthStencilDesc();

	bool				DepthEnable;
	bool				StencilEnable;
	RHI_Comp			DepthComp;
};

DEPTHSTENCILSTATE RHI_CreateDepthStencilState(DEVICE device, RHI_DepthStencilDesc &desc);
void			  RHI_SetDepthStencilState(DEVICE device, DEPTHSTENCILSTATE &dss);

/*
 *   RASTERIZER STATE
 */
enum RHI_Rasterizer_Cull
{
	RHIRastCull_Unknown=0,
	RHIRastCull_BACK,
	RHIRastCull_FRONT,
	RHIRastCull_NONE,
	RHIRastCull_MAX
};

struct RHI_RasterizerDesc
{
							RHI_RasterizerDesc();

	bool					WireFrameEnable;
	bool					DepthClipEnable;
	RHI_Rasterizer_Cull		CullMode;
};

RASTERIZERSTATE		RHI_CreateRasterizerState(DEVICE device, RHI_RasterizerDesc &desc);
void				RHI_SetRasterizerState(DEVICE device, RASTERIZERSTATE &rs);


/*
 *   BLEND STATE STATE
 */
struct RHI_BlendDesc
{
							RHI_BlendDesc();

	bool					AlphaToCoverageEnable;
	bool					IndependentBlendEnable;
};

BLENDSTATE		RHI_CreateBlendState(DEVICE device, RHI_BlendDesc &desc);
void			RHI_SetBlendState(DEVICE device, BLENDSTATE &rs);

/*
 * SHADER 
 */
VERTEXSHADER	RHI_CreateVertexShaderFromFile(DEVICE device, const WCHAR *filename, const char *EntryName, const char *profile);
void			RHI_SetVertexShader(DEVICE device, VERTEXSHADER &vs);

PIXELSHADER		RHI_CreatePixelShaderFromFile( DEVICE device, const WCHAR *filename, const char *EntryName, const char *profile);
void			RHI_SetPixelShader(DEVICE device, PIXELSHADER &ps);

COMPUTESHADER	RHI_CreateComputeShaderFromFile(DEVICE device, const WCHAR *filename, const char *EntryName, const char *profile);
void			RHI_SetComputeShader(DEVICE device, COMPUTESHADER &cs);

/*
 * SAMPLER 
 */
enum	RHI_SAMPLER_TA
{	
		RHISamplerTA_Unknown=0,
		RHISamplerTA_Wrap,
		RHISamplerTA_Clamp,
		RHISamplerTA_Border,
		RHISamplerTA_MAX
};

enum	RHI_SAMPLER_FILTER
{	
		RHISamplerFilter_Unknown=0,
		RHISamplerFilter_Linear,
		RHISamplerFilter_Point,
		RHISamplerFilter_Compare,
		RHISamplerFilter_MAX
};

struct RHI_SamplerDesc
{
						RHI_SamplerDesc();
	
	Uint8				MaxAniso;
	float				MinLOD;
	float				MaxLOD;
	float				MipLODBias;
	

	RHI_SAMPLER_TA		AddressU;
	RHI_SAMPLER_TA		AddressV;
	RHI_SAMPLER_TA		AddressW;
	RHI_SAMPLER_FILTER  Filter;

	RHI_Comp			Comp;
	float				BorderColor[4];
};

SAMPLERSTATE		RHI_CreateSamplerState(DEVICE device, RHI_SamplerDesc &desc);
void				RHI_SetSamplerState_PS(DEVICE device, Uint32 StartSlot, Uint32 NumSamplers, SAMPLERSTATE &sampler);

/*
 * CONSTANT BUFFER 
 */
struct RHI_ConstantBufferDesc
{
						RHI_ConstantBufferDesc();
	Uint32				ByteWidth;	
};

CONSTANTBUFFER		RHI_CreateConstantBuffer(DEVICE device, RHI_ConstantBufferDesc &desc);

void				RHI_UpdateConstantBuffer(DEVICE device, CONSTANTBUFFER &cb, void *Data);

void				RHI_SetConstantBuffer_PS(DEVICE device, Uint32 StartSlot, Uint32 NumSlots, CONSTANTBUFFER &cb);
void				RHI_SetConstantBuffer_VS(DEVICE device, Uint32 StartSlot, Uint32 NumSlots, CONSTANTBUFFER &cb);
void				RHI_SetConstantBuffer_VS_PS(DEVICE device, Uint32 StartSlot, Uint32 NumSlots, CONSTANTBUFFER &cb);


extern				void RHIInit();
#endif


