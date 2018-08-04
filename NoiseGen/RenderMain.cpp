#include "GUI.h"
#include "Core\Core.h"
#include "Platform\RHI.h"
#include "ImGui\ImGui.h"

__forceinline Mat4 Mat4Identity() { return Mat4(1.0f);}

struct Vertex_Pos32fx3_UVf
{
	float   x, y, z;
	float	U, V;
};

//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------
struct SimpleVertex
{
	glm::vec3 Pos;
	glm::vec2 Tex;
};

struct CBView
{
	glm::mat4x4		mView;
	glm::mat4x4		mProjection;
	glm::mat4x4		mViewProjection;
	float			LerpVal;
	float			Pad[2];
	Sint32			NoiseOffset;
};

struct CBDraw
{
	glm::mat4x4		mWorld;
};

//extern OrbitCamera				g_OrbitCamera;
//extern UVCamera					g_UVCamera;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
extern HINSTANCE                    g_hInst;
extern HWND                         g_hWnd;
extern IDXGISwapChain*              g_pSwapChain;

ID3D11RenderTargetView*             g_pBackBufferRTV;



ID3D11InputLayout*                  g_pVertexLayout_PosUV = NULL;

DEPTHSTENCIL	                    g_DepthStencil;
RENDERTARGET						g_MainRT;

RASTERIZERSTATE						g_Rasterizer_Solid;
BLENDSTATE							g_DefaultBlendState;

CONSTANTBUFFER                      g_pCBView ;
DEPTHSTENCILSTATE					g_DepthStencilState_Main;
DEPTHSTENCILSTATE					g_DepthStencilState_Always;


VERTEXBUFFER						g_QuadVB;
INDEXBUFFER							g_QuadIB;
	
// Noise Generation
const int NoiseSZ = 256;
//TEXTURE2D							g_NoiseTex;
RENDERTARGET						g_Blur1Tex;
RENDERTARGET						g_Blur2Tex;
RENDERTARGET						g_NoiseRT;

PIXELSHADER							g_pNoiseGenPS;
PIXELSHADER							g_Blur1PS;
PIXELSHADER							g_Blur2PS;
PIXELSHADER							g_FinalOutputPS;
PIXELSHADER							g_CopyPS;



SAMPLERSTATE						g_pSamplerLinear;
SAMPLERSTATE						g_pSamplerPoint;

// Histogram 
COMPUTESHADER						g_HistogramCS;
COMPUTESHADER						g_ClearHistogramCS;
COMPUTESHADER						g_HistogramAnalyzeCS;

ID3D11Buffer						*g_pHISTOGRAM;
ID3D11ShaderResourceView* const     g_pNullSRV = nullptr;       // Helper to Clear SRVs 
ID3D11UnorderedAccessView* const    g_pNullUAV = nullptr; 
ID3D11UnorderedAccessView			*g_pHISTOGRAM_UAV;
ID3D11ShaderResourceView            *g_pHISTOGRAM_SRV;

#if 0
ID3D11InputLayout*                  g_pVertexLayout_Std = NULL;
ID3D11InputLayout*                  g_pVertexLayout_DebugView = NULL;
ID3D11InputLayout*                  g_pVertexLayout_PosOnly = NULL;
ID3D11InputLayout*                  g_pVertexSimpleLayout = NULL;

glm::mat4x4                         g_WorldMat;
glm::mat4x4                         g_ViewMat;
glm::mat4x4                         g_ProjectionMat;

// Agnostic Stuff

DEPTHSTENCILSTATE					g_DepthStencilState_Shadow;

RASTERIZERSTATE						g_Rasterizer_Wireframe;
RASTERIZERSTATE						g_Rasterizer_Shadow;


CONSTANTBUFFER                      g_pCBDraw ;

SAMPLERSTATE						g_pSamplerPCF;
TEXTURE2D							g_pFloorTex;
#endif

VERTEXSHADER						g_pQuadVS;

eTEXTUREFORMAT						NoiseFMT = TEXFMT_R16_FLOAT;


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
bool CreateResources()
{
	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_Device.pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pBackBufferRTV);


	// Texture for the noise  and the 2 blur targets
	TEXTURE_DESC desc(NoiseSZ, NoiseSZ, NoiseFMT);
	g_NoiseRT  =  RHI_CreateRenderTarget(g_Device,  desc);
	g_Blur1Tex =  RHI_CreateRenderTarget(g_Device,  desc);
	g_Blur2Tex =  RHI_CreateRenderTarget(g_Device,  desc);


	RHI_SamplerDesc samplerDesc;

	g_pSamplerLinear		= RHI_CreateSamplerState(g_Device, samplerDesc);
	samplerDesc.Filter		= RHISamplerFilter_Point;
	g_pSamplerPoint			= RHI_CreateSamplerState(g_Device, samplerDesc);
	


	//g_NoiseTex = RHI_CreateTexture2D
//
//   g_Device.pD3DDevice->CreateTexture2D(CreateRenderTargetView(pBackBuffer, NULL, &g_pMainRT[0]);



//	D3D11_RENDER_TARGET_VIEW_DESC desc;
//	desc.Format				=  DXGI_FORMAT_R8_SNORM;
//	desc.ViewDimension		=  D3D11_RTV_DIMENSION_TEXTURE2D;
//	desc.Texture2D.MipSlice = 0;
//	desc.Texture2D.MipSlice = 0;
//	g_Device.pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_NoiseRTV);


	// Create two textures for the render targets. Current and history
//	D3D11_TEXTURE2D_DESC texDesc;
//	texDesc.Width = 
//	g_Device.pD3DDevice->CreateTexture2D(CreateRenderTargetView(pBackBuffer, NULL, &g_pMainRT[0]);


//	g_Device.pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pMainRT[0]);


	pBackBuffer->Release();
	{
		// Create depth stencil texture
		TEXTURE_DESC		texDepth(g_Window_W, g_Window_H);
		g_DepthStencil = RHI_CreateDepthStencil(g_Device, texDepth);

		TEXTURE_DESC		texRT(g_Window_W, g_Window_H);
		g_MainRT = RHI_CreateRenderTarget(g_Device, texRT);

		g_pQuadVS = RHI_CreateVertexShaderFromFile(g_Device, L"Main.fx", "VS_Quad", "vs_5_0");
		g_pNoiseGenPS = RHI_CreatePixelShaderFromFile(g_Device, L"Main.fx", "PS_NoiseGen", "ps_5_0");
		g_Blur1PS = RHI_CreatePixelShaderFromFile(g_Device, L"Main.fx", "PS_Blur1", "ps_5_0");
		g_Blur2PS = RHI_CreatePixelShaderFromFile(g_Device, L"Main.fx", "PS_Blur2", "ps_5_0");
		g_FinalOutputPS = RHI_CreatePixelShaderFromFile(g_Device, L"Main.fx", "PS_FinalOutput", "ps_5_0");
		g_CopyPS = RHI_CreatePixelShaderFromFile(g_Device, L"Main.fx", "PS_Copy", "ps_5_0");

		g_HistogramCS			= RHI_CreateComputeShaderFromFile(g_Device, L"Main.fx", "CS_HistogramGen", "cs_5_0");
		g_ClearHistogramCS		= RHI_CreateComputeShaderFromFile(g_Device, L"Main.fx", "CS_ClearHistogram", "cs_5_0");
		g_HistogramAnalyzeCS	= RHI_CreateComputeShaderFromFile(g_Device, L"Main.fx", "CS_HistogramAnalyze", "cs_5_0");
	}

	{
		D3D11_BUFFER_DESC desc;

		const int HIST_SZ = 256+4;
		desc.ByteWidth = 4 * (HIST_SZ);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 4;
		HRESULT hres = g_Device.pD3DDevice->CreateBuffer(&desc, 0, &g_pHISTOGRAM);

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		UAVDesc.ViewDimension		= D3D11_UAV_DIMENSION_BUFFER;
		UAVDesc.Format				= DXGI_FORMAT_R32_UINT;
		UAVDesc.Buffer.FirstElement	= 0;
		UAVDesc.Buffer.NumElements = HIST_SZ;
		UAVDesc.Buffer.Flags	   = 0;
		hres = g_Device.pD3DDevice->CreateUnorderedAccessView(g_pHISTOGRAM, &UAVDesc, &g_pHISTOGRAM_UAV);


		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format	= DXGI_FORMAT_R32_UINT;
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.ElementWidth	 = 4;
		srvDesc.Buffer.FirstElement	 = 0;
		srvDesc.Buffer.NumElements	= HIST_SZ;
		hres = g_Device.pD3DDevice->CreateShaderResourceView(g_pHISTOGRAM, &srvDesc, &g_pHISTOGRAM_SRV);

		int g=0;


	}


	{
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = ARRAYSIZE(layout);

		// Create the input layout
		g_Device.pD3DDevice->CreateInputLayout(layout, numElements, g_pQuadVS.pBlob->GetBufferPointer(), g_pQuadVS.pBlob->GetBufferSize(), &g_pVertexLayout_PosUV);
	}
	g_pQuadVS.ReleaseBlob();

	
	RHI_RasterizerDesc			RastDesc;
	RastDesc.CullMode			= RHIRastCull_BACK;
	g_Rasterizer_Solid			= RHI_CreateRasterizerState(g_Device, RastDesc);

	RHI_BlendDesc				BlendStateDesc;
	g_DefaultBlendState			= RHI_CreateBlendState(g_Device, BlendStateDesc);


	RHI_ConstantBufferDesc cbDesc;
	cbDesc.ByteWidth	= sizeof(CBView);
	g_pCBView = RHI_CreateConstantBuffer(g_Device, cbDesc);


	RHI_DepthStencilDesc		DepthDesc;
	DepthDesc.DepthComp			= RHIDC_GreaterEqual;
	DepthDesc.DepthEnable		= true;
	DepthDesc.StencilEnable		= false;
	g_DepthStencilState_Main	= RHI_CreateDepthStencilState(g_Device, DepthDesc);


	DepthDesc.DepthComp			= RHIDC_Always;
	DepthDesc.DepthEnable		= true;
	DepthDesc.StencilEnable		= false;
	g_DepthStencilState_Always	= RHI_CreateDepthStencilState(g_Device, DepthDesc);

	// Create a quad to draw a Full screen quad
	Vertex_Pos32fx3_UVf Verts[4] = { {-1.0f,-1.0f, 0.0f,   0.0f, 1.0f}, 	
									 {-1.0f, 1.0f, 0.0f,   0.0f, 0.0f},
									 { 1.0f,-1.0f, 0.0f,   1.0f, 1.0f}, 	
									 { 1.0f, 1.0f, 0.0f,   1.0f, 0.0f} };
	g_QuadVB					= RHI_CreateAndFillVertexBuffer(g_Device, 4, sizeof(Vertex_Pos32fx3_UVf), (void *)Verts );

	Uint16	QuadIBData[6] = { 0, 1, 2, 1, 3, 2};
	g_QuadIB					= RHI_CreateAndFillIndexBuffer(g_Device, sizeof(Uint16), 6, QuadIBData);






#if 0
	RastDesc.WireFrameEnable	= true;
	RastDesc.CullMode			= RHIRastCull_NONE;
	g_Rasterizer_Wireframe		= RHI_CreateRasterizerState(g_Device, RastDesc);




	DepthDesc.DepthComp			= RHIDC_LessEqual;
	DepthDesc.DepthEnable		= true;
	DepthDesc.StencilEnable		= false;
	g_DepthStencilState_Shadow	= RHI_CreateDepthStencilState(g_Device, DepthDesc);


	DepthDesc.DepthComp			= RHIDC_Always;
	g_DepthStencilState_Always  = RHI_CreateDepthStencilState(g_Device, DepthDesc);


	cbDesc.ByteWidth	= sizeof(CBDraw);
	g_pCBDraw = RHI_CreateConstantBuffer(g_Device, cbDesc);

	// Load the Texture
	g_pFloorTex = RHI_CreateTexture2DFromFile(g_Device, L"floor2.dds");


	// Create the sampler
	RHI_SamplerDesc samplerDesc;
	g_pSamplerLinear		= RHI_CreateSamplerState(g_Device, samplerDesc);

	samplerDesc.Filter		= RHISamplerFilter_Point;
	g_pSamplerPoint			= RHI_CreateSamplerState(g_Device, samplerDesc);

	samplerDesc.AddressU	= RHISamplerTA_Border;
	samplerDesc.AddressV	= RHISamplerTA_Border;
	samplerDesc.AddressW	= RHISamplerTA_Border;
	samplerDesc.Comp		= RHIDC_Less;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = 10.8f;
	samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 10.8f;
	g_pSamplerPCF			= RHI_CreateSamplerState(g_Device, samplerDesc);


	// Initialize the world matrices
	g_WorldMat = Mat4Identity();

#endif

	// Create a light source
//	g_MainDirLight = new DirectionalLight( glm::vec3(0.5,-0.5,0.5), 512);

	// Initialize the projection matrix - Infinite far variety.
	return true;
}

bool DestroyResources()
{

	g_Rasterizer_Solid.Release();
	g_DefaultBlendState.Release();
	g_pCBView.Release() ;

	if (g_pVertexLayout_PosUV)		g_pVertexLayout_PosUV->Release();
//	if (&g_pMainRT[0])				g_pMainRT[0]->Release();
	if (g_pSwapChain)				g_pSwapChain->Release();

	g_pNoiseGenPS.Release();
	g_pQuadVS.Release();
//	pBackBuffer->Release();

	g_DepthStencilState_Main.Release();
	g_DepthStencilState_Always.Release();

	g_QuadVB.Release();
	g_QuadIB.Release();

	g_pSamplerLinear.Release();
	g_pSamplerPoint.Release();


#if 0
	if (g_pVertexLayout_Std)		g_pVertexLayout_Std->Release();
	if (g_pVertexLayout_DebugView)	g_pVertexLayout_DebugView->Release();
	
	if (g_pVertexLayout_PosOnly)	g_pVertexLayout_PosOnly->Release();
	if (g_pVertexSimpleLayout)		g_pVertexSimpleLayout->Release();
	if (g_pSwapChain)				g_pSwapChain->Release();

	g_DepthStencilState_Always.Release();
	g_Rasterizer_Wireframe.Release();
	g_Standard_VS.Release();
	g_UVDraw_VS.Release();
	g_PosOnly_VS.Release();
	g_pPostFX_TAA_VS.Release();
	g_pFloorVS.Release();
	g_pFloorVS.ReleaseBlob();
	g_pFloorPS.Release() ;
	g_pFloorPS.ReleaseBlob() ;
	g_pPostFX_TAA_PS.Release() ;
	g_Standard_PS.Release();
	g_FixedCol_PS.Release();
	g_DepthOnly_PS.Release();
	g_pCBDraw.Release() ;
	g_pSamplerLinear.Release();
	g_pSamplerPoint.Release();
	g_pSamplerPCF.Release();
	g_DepthStencil.Release();
	g_MainRT.Release();
	g_pFloorTex.Release();
#endif

//	delete g_MainDirLight;

	return true;
}



void GenNoise()
{
	RHI_SetSamplerState_PS(g_Device, 0,1,  g_pSamplerLinear);
	RHI_SetSamplerState_PS(g_Device, 1,1,  g_pSamplerPoint);
	g_Device.pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_Device.pD3DContext->IASetInputLayout(g_pVertexLayout_PosUV);
	RHI_SetRasterizerState(g_Device, g_Rasterizer_Solid);
	g_Device.pD3DContext->OMSetBlendState(NULL,NULL,0xffffffff);
	RHI_SetDepthStencilState(g_Device, g_DepthStencilState_Always);

	RHI_VIEWPORT vp = { 0.0f, 0.0f,  (float) 256, (float) 256,   0.0f ,1.0f };
	RHI_SetViewport(g_Device, &vp);
	
	RHI_SetVertexShader(g_Device, g_pQuadVS);

	RHI_SetVertexBuffer(g_Device, g_QuadVB);
	RHI_SetIndexBuffer(g_Device, g_QuadIB);

	// Noise Gen
	g_Device.pD3DContext->OMSetRenderTargets(1, &g_NoiseRT.pD3DRTV  , nullptr);
	RHI_SetPixelShader(g_Device,  g_pNoiseGenPS);
	g_Device.pD3DContext->DrawIndexed(6, 0,0);


	// Blur X
	for(int i=0; i<gGUI.Iterations ; i++)
	{
		g_Device.pD3DContext->OMSetRenderTargets(1, &g_Blur1Tex.pD3DRTV  , nullptr);
		RHI_SetPixelShader(g_Device,  g_Blur1PS);
		g_Device.pD3DContext->PSSetShaderResources(0, 1, &g_NoiseRT.pD3DSRV);
		g_Device.pD3DContext->DrawIndexed(6, 0,0);


		// Blur Y
		g_Device.pD3DContext->OMSetRenderTargets(1, &g_Blur2Tex.pD3DRTV  , nullptr);
		RHI_SetPixelShader(g_Device,  g_Blur2PS);
		g_Device.pD3DContext->PSSetShaderResources(0, 1, &g_Blur1Tex.pD3DSRV);
		g_Device.pD3DContext->PSSetShaderResources(1, 1, &g_NoiseRT.pD3DSRV);
		g_Device.pD3DContext->DrawIndexed(6, 0,0);

		// Copy
		g_Device.pD3DContext->OMSetRenderTargets(1, &g_NoiseRT.pD3DRTV  , nullptr);
		RHI_SetPixelShader(g_Device,  g_CopyPS);
		g_Device.pD3DContext->PSSetShaderResources(0, 1, &g_Blur2Tex.pD3DSRV);
		g_Device.pD3DContext->DrawIndexed(6, 0,0);

	}

	// Histogram
	{
		g_Device.pD3DContext->OMSetRenderTargets(0, nullptr, nullptr);
		RHI_SetComputeShader(g_Device, g_ClearHistogramCS);
		g_Device.pD3DContext->CSSetUnorderedAccessViews(0, 1,&g_pHISTOGRAM_UAV,0);
		g_Device.pD3DContext->Dispatch(1, 1, 1);

		RHI_SetComputeShader(g_Device, g_HistogramCS);
		g_Device.pD3DContext->CSSetShaderResources(2, 1, &g_NoiseRT.pD3DSRV);
		g_Device.pD3DContext->CSSetUnorderedAccessViews(0, 1,&g_pHISTOGRAM_UAV,0);
		const int NumThreads = 256/16;
		g_Device.pD3DContext->Dispatch(NumThreads, NumThreads, 1);
		g_Device.pD3DContext->CSSetUnorderedAccessViews(0, 1, &g_pNullUAV,0);

		RHI_SetComputeShader(g_Device, g_HistogramAnalyzeCS);
		g_Device.pD3DContext->CSSetUnorderedAccessViews(0, 1,&g_pHISTOGRAM_UAV,0);
		g_Device.pD3DContext->Dispatch(1, 1, 1);
		g_Device.pD3DContext->CSSetUnorderedAccessViews(0, 1, &g_pNullUAV,0);

	}



	// TO THE SCREEN
	g_Device.pD3DContext->OMSetRenderTargets(1, &g_pBackBufferRTV  , g_DepthStencil.pD3DDSV);
	RECT rc;
	GetClientRect(g_Device.hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Setup the viewport
	 vp = { 0.0f, 0.0f,  (float) width, (float) height,   0.0f ,1.0f };
	RHI_SetViewport(g_Device, &vp);

	RHI_SetPixelShader(g_Device,  g_FinalOutputPS);
	g_Device.pD3DContext->PSSetShaderResources(0, 1, &g_NoiseRT.pD3DSRV);
	g_Device.pD3DContext->PSSetShaderResources(2, 1, &g_pHISTOGRAM_SRV);
//	g_Device.pD3DContext->PSSetShaderResources(1, 1, &g_pHISTOGRAM_UAV);

	//g_Device.pD3DContext->CSSetUnorderedAccessViews(0, 1,&g_pHISTOGRAM_UAV,0);
	

	g_Device.pD3DContext->DrawIndexed(6, 0,0);


}

void Render()
{
	// Frame wide Constant buffer
	CBView					cbView;
//	cbView.mView			= g_MainDirLight->mLightMat;
//	cbView.mProjection		= matIdent;
//	cbView.mViewProjection	= g_MainDirLight->mLightMat;
	cbView.LerpVal			= gGUI.LerpVal;
	cbView.NoiseOffset		= gGUI.NoiseOffset;
	RHI_UpdateConstantBuffer(g_Device,  g_pCBView, (void *) &cbView);
	RHI_SetConstantBuffer_VS_PS(g_Device, 0, 1, g_pCBView);


	// Default states
	g_Device.pD3DContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_Device.pD3DContext->IASetInputLayout(g_pVertexLayout_PosUV);
	RHI_SetRasterizerState(g_Device, g_Rasterizer_Solid);
	g_Device.pD3DContext->OMSetBlendState(NULL,NULL,0xffffffff);
	RHI_SetDepthStencilState(g_Device, g_DepthStencilState_Always);


	// Render the noise to an offscreen surface



	// Composite the final buffer


	float ClearColor[4] = { 0.0f, 0.1f, 0.3f, 1.0f }; // red, green, blue, alpha
//	g_Device.pD3DContext->ClearRenderTargetView( g_pMainRT[MainRT] , ClearColor);
//	g_Device.pD3DContext->ClearDepthStencilView(g_DepthStencil.pD3DDSV, D3D11_CLEAR_DEPTH, 0.5f, 0);
//	g_Device.pD3DContext->OMSetRenderTargets(1, &g_pMainRT[MainRT]  , g_DepthStencil.pD3DDSV);

	g_Device.pD3DContext->ClearRenderTargetView(g_pBackBufferRTV , ClearColor);
	g_Device.pD3DContext->ClearDepthStencilView(g_DepthStencil.pD3DDSV, D3D11_CLEAR_DEPTH, 0.5f, 0);
	g_Device.pD3DContext->OMSetRenderTargets(1, &g_pBackBufferRTV  , g_DepthStencil.pD3DDSV);

	
	GenNoise();


		// URG - TODO - FIXME
	RECT rc;
	GetClientRect(g_Device.hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// Setup the viewport
	RHI_VIEWPORT vp = { 0.0f, 0.0f,  (float) width, (float) height,   0.0f ,1.0f };
	RHI_SetViewport(g_Device, &vp);


/*	MainRT		= iCurrFrame&1; 
	HistoryRT	= 1 - MainRT; 

//	g_Device.pD3DContext->OMSetRenderTargets(1, &g_pRenderTargetViews[MainRT], g_DepthStencil.pD3DDSV);
	
	switch (g_GUI_Viewmode)
	{
	case VIEWMODE_3D:
		Render_3D();
		break;
		
	case VIEWMODE_UV:
//		Render_UV();
		break;
	}
	

	D3DPERF_BeginEvent(0xffffffff, L"ImGui");

	ImGui::Render();
	D3DPERF_EndEvent();
*/
	

	ImGui::Render();

	g_pSwapChain->Present(1, 0);
}
