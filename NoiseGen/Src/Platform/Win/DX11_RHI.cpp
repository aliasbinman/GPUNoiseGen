#include "../RHI.h"

#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

// Global device state
DEVICE g_Device;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

DXGI_FORMAT DX11_ILT_REMAP[ILT_MAX];
DXGI_FORMAT DX11_TexFormat[TEXFMT_MAX];

// Called once at application start time
void RHIInit()
{
	// Create the remap table for the DXGI Format for input layout
	for(int i=0; i< ILT_MAX; i++)
	{
		DX11_ILT_REMAP[i] = DXGI_FORMAT_UNKNOWN;
	}

	DX11_TexFormat[TEXFMT_UNKNOWN]			= DXGI_FORMAT_UNKNOWN;
	DX11_TexFormat[TEXFMT_R8G8B8A8]			= DXGI_FORMAT_R8G8B8A8_UNORM;
	DX11_TexFormat[TEXFMT_R8G8B8A8_SRGB]	= DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	DX11_TexFormat[TEXFMT_R8_SNORM]			= DXGI_FORMAT_R8_UNORM;
	DX11_TexFormat[TEXFMT_R16_FLOAT]		= DXGI_FORMAT_R16_FLOAT;
	DX11_TexFormat[TEXFMT_D32]				= DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	DX11_TexFormat[TEXFMT_D24S8]			= DXGI_FORMAT_D24_UNORM_S8_UINT;
}

DXGI_FORMAT ConvertFmt(eTEXTUREFORMAT fmt)
{
	return DX11_TexFormat[fmt];
}


void RHIClose()
{
}


TEXTURE2D RHI_CreateTexture2D(DEVICE device, TEXTURE_DESC &Desc)
{	
	D3D11_TEXTURE2D_DESC d3dDesc;
	ZeroMemory(&d3dDesc, sizeof(d3dDesc));
	d3dDesc.Width					= Desc.Width;
	d3dDesc.Height					= Desc.Height;
	d3dDesc.MipLevels				= 1;
	d3dDesc.ArraySize				= 1;
	d3dDesc.Format					= ConvertFmt(Desc.Format);
	d3dDesc.SampleDesc.Count		= 1;
	d3dDesc.SampleDesc.Quality		= 0;
	d3dDesc.Usage					= D3D11_USAGE_DEFAULT;
	d3dDesc.BindFlags				= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	d3dDesc.CPUAccessFlags			= 0;
	d3dDesc.MiscFlags				= 0;
	
	TEXTURE2D tex2D;
	g_Device.pD3DDevice->CreateTexture2D(&d3dDesc, 0, &tex2D.pD3DTexture2D);


	return tex2D;
}


TEXTURE2D RHI_CreateTexture2DFromFile(DEVICE device, WCHAR *filename)
{
	TEXTURE2D tex2D;
	D3DX11CreateShaderResourceViewFromFile(g_Device.pD3DDevice, L"floor2.dds", NULL, NULL, &tex2D.pD3DSRV, NULL);
	return tex2D;
}

DEPTHSTENCIL RHI_CreateDepthStencil(DEVICE device, TEXTURE_DESC &desc)
{
	DEPTHSTENCIL tex2D;

	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width					= desc.Width;
	descDepth.Height				= desc.Height;
	descDepth.MipLevels				= 1;
	descDepth.ArraySize				= 1;
	descDepth.Format				= DXGI_FORMAT_R24G8_TYPELESS;
	descDepth.SampleDesc.Count		= 1;
	descDepth.SampleDesc.Quality	= 0;
	descDepth.Usage					= D3D11_USAGE_DEFAULT;
	descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags		= 0;
	descDepth.MiscFlags				= 0;
	HRESULT hres = g_Device.pD3DDevice->CreateTexture2D(&descDepth, NULL, &tex2D.pD3DTexture2D);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format					= DXGI_FORMAT_D24_UNORM_S8_UINT ;
	descDSV.ViewDimension			= D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice		= 0;
	hres = g_Device.pD3DDevice->CreateDepthStencilView(tex2D.pD3DTexture2D, &descDSV, &tex2D.pD3DDSV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format					= DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip	= 0;
	srvDesc.Texture2D.MipLevels		= -1;
	hres = g_Device.pD3DDevice->CreateShaderResourceView(tex2D.pD3DTexture2D, &srvDesc, &tex2D.pD3DSRV);
	return tex2D;
}



RENDERTARGET RHI_CreateRenderTarget(DEVICE device, TEXTURE_DESC &desc)
{
	RENDERTARGET tex2D;

	D3D11_TEXTURE2D_DESC descRT;
	ZeroMemory(&descRT, sizeof(descRT));
	descRT.Width				= desc.Width;
	descRT.Height				= desc.Height;
	descRT.MipLevels			= 1;
	descRT.ArraySize			= 1;
	descRT.Format				= ConvertFmt(desc.Format);
	descRT.SampleDesc.Count		= 1;
	descRT.SampleDesc.Quality	= 0;
	descRT.Usage				= D3D11_USAGE_DEFAULT;
	descRT.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	descRT.CPUAccessFlags		= 0;
	descRT.MiscFlags			 = 0;
	g_Device.pD3DDevice->CreateTexture2D(&descRT, NULL, &tex2D.pD3DTexture2D);
	g_Device.pD3DDevice->CreateRenderTargetView(tex2D.pD3DTexture2D, NULL, &tex2D.pD3DRTV);

	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format					= descRT.Format;
    srvDesc.ViewDimension			= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip	= 0;
	srvDesc.Texture2D.MipLevels		= 1;
	HRESULT hr =  g_Device.pD3DDevice->CreateShaderResourceView(tex2D.pD3DTexture2D, &srvDesc, &tex2D.pD3DSRV);


	return tex2D;
}

void RHI_SetViewport(DEVICE device, RHI_VIEWPORT *viewport)
{
	D3D11_VIEWPORT vp;
	vp.Width		= viewport->W;
	vp.Height		= viewport->H;
	vp.MinDepth		= viewport->ZMin;
	vp.MaxDepth		= viewport->ZMax;
	vp.TopLeftX		= viewport->X;
	vp.TopLeftY		= viewport->Y;
	g_Device.pD3DContext->RSSetViewports(1, &vp);
}

void RHI_SetScissorRect(DEVICE device, RHI_RECT *rect)
{
	if(rect == NULL)
	{
		g_Device.pD3DContext->RSSetScissorRects(0, NULL);
	} 
	else
	{
		D3D11_RECT d3drect;
		d3drect.left	= (LONG) rect->L;
		d3drect.right	= (LONG) rect->R;
		d3drect.bottom  = (LONG) rect->B;
		d3drect.top 	= (LONG) rect->T;
		g_Device.pD3DContext->RSSetScissorRects(1, &d3drect);
	}
}

/*
 * VERTEX BUFFER
 */
VERTEXBUFFER::VERTEXBUFFER()
{
	pD3DVB	= NULL;
	Stride	= 0;
	Offset  = 0;
}

VERTEXBUFFER RHI_CreateAndFillVertexBuffer(DEVICE device, Uint32 NumVerts, Uint32 VertSize, void *FillData )
{
	VERTEXBUFFER VB;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth        = VertSize * NumVerts;
	bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags   = 0;
	bufferDesc.MiscFlags        = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem			= FillData;
	InitData.SysMemPitch		= 0;
	InitData.SysMemSlicePitch	= 0;

	HRESULT hr = device.pD3DDevice->CreateBuffer( &bufferDesc, &InitData, &VB.pD3DVB );
	VB.Stride					 = VertSize;
	VB.Offset					 = 0;

	return VB;
}

void RHI_SetVertexBuffer(DEVICE &device, VERTEXBUFFER &VB)
{
	device.pD3DContext->IASetVertexBuffers(0, 1, &VB.pD3DVB, &VB.Stride, &VB.Offset);
}


INDEXBUFFER::INDEXBUFFER()
{
	pD3DIB	= NULL;
	IdxSz	= 0;
	Fmt		= DXGI_FORMAT_UNKNOWN;
}

INDEXBUFFER RHI_CreateAndFillIndexBuffer(DEVICE device, Uint32 IdxSz,  Uint32 NumIndices, void *FillData )
{
	INDEXBUFFER IB;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth        = NumIndices * IdxSz;
	bufferDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags   = 0;
	bufferDesc.MiscFlags        = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem			= FillData;
	InitData.SysMemPitch		= 0;
	InitData.SysMemSlicePitch	= 0;

    HRESULT hr =  device.pD3DDevice->CreateBuffer( &bufferDesc, &InitData, &IB.pD3DIB );
	IB.IdxSz = IdxSz;
	if(IdxSz ==2)
		IB.Fmt = DXGI_FORMAT_R16_UINT;
	else
		IB.Fmt = DXGI_FORMAT_R32_UINT;

	return IB;
}

void RHI_SetIndexBuffer(DEVICE &device, INDEXBUFFER &IB)
{
	device.pD3DContext->IASetIndexBuffer(IB.pD3DIB,IB.Fmt , 0);
	
}



VERTEXSHADER RHI_CreateVertexShaderFromFile(DEVICE device, const WCHAR *filename, const char *EntryName, const char *profile)
{
HRESULT hr;

	VERTEXSHADER VS;

	DWORD dwShaderFlags = 0;
    ID3DBlob* pErrorBlob;

	hr = D3DX11CompileFromFile( filename, NULL, NULL, EntryName, profile, dwShaderFlags, 0, NULL, &VS.pBlob, &pErrorBlob, NULL );
	while(hr != S_OK)
	{
		char * Error =  (char *) pErrorBlob->GetBufferPointer();
		MessageBoxA(g_Device.hWnd, Error, "Error", MB_OK);
		hr = D3DX11CompileFromFile( filename, NULL, NULL, EntryName, profile, dwShaderFlags, 0, NULL, &VS.pBlob, &pErrorBlob, NULL );
	}

    hr =  device.pD3DDevice->CreateVertexShader( VS.pBlob->GetBufferPointer(), VS.pBlob->GetBufferSize(), NULL, &VS.pD3DVS );
	return VS;
}


PIXELSHADER RHI_CreatePixelShaderFromFile(DEVICE device, const WCHAR *filename, const char *EntryName, const char *profile)
{
HRESULT hr;

	PIXELSHADER PS;

	DWORD dwShaderFlags = 0;
    ID3DBlob* pErrorBlob;

	hr = D3DX11CompileFromFile( filename, NULL, NULL, EntryName, profile, dwShaderFlags, 0, NULL, &PS.pBlob, &pErrorBlob, NULL );
	while(hr != S_OK)
	{
		char * Error =  (char *) pErrorBlob->GetBufferPointer();
		MessageBoxA(g_Device.hWnd, Error, "Error", MB_OK);
		hr = D3DX11CompileFromFile( filename, NULL, NULL, EntryName, profile, dwShaderFlags, 0, NULL, &PS.pBlob, &pErrorBlob, NULL );
	}

    hr =  device.pD3DDevice->CreatePixelShader( PS.pBlob->GetBufferPointer(), PS.pBlob->GetBufferSize(), NULL, &PS.pD3DPS );
	return PS;
}


COMPUTESHADER RHI_CreateComputeShaderFromFile(DEVICE device, const WCHAR *filename, const char *EntryName, const char *profile)
{
	HRESULT hr;

	COMPUTESHADER CS;

	DWORD dwShaderFlags = 0;
	ID3DBlob* pErrorBlob;

	hr = D3DX11CompileFromFile(filename, NULL, NULL, EntryName, profile, dwShaderFlags, 0, NULL, &CS.pBlob, &pErrorBlob, NULL);
	while (hr != S_OK)
	{
		char * Error = (char *)pErrorBlob->GetBufferPointer();
		MessageBoxA(g_Device.hWnd, Error, "Error", MB_OK);
		hr = D3DX11CompileFromFile(filename, NULL, NULL, EntryName, profile, dwShaderFlags, 0, NULL, &CS.pBlob, &pErrorBlob, NULL);
	}

	hr = device.pD3DDevice->CreateComputeShader(CS.pBlob->GetBufferPointer(), CS.pBlob->GetBufferSize(), NULL, &CS.pD3DCS);
	return CS;
}



INPUTLAYOUT RHI_CreateInputLayout(DEVICE device)
{
HRESULT hr;

	INPUTLAYOUT IL;
    ID3DBlob* pBlob = NULL;

	   const D3D11_INPUT_ELEMENT_DESC SceneLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
     

    hr =  device.pD3DDevice->CreateInputLayout( SceneLayout, ARRAYSIZE( SceneLayout ), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &IL.pD3DIL);
	return IL;
}




INPUTLAYOUT RHI_CreateInputLayout(DEVICE device, Uint32 NumElements, InputLayoutElement *pElements)
{
	D3D11_INPUT_ELEMENT_DESC DX11Layout[16];

	for(Uint32 i=0; i<NumElements; i++)
	{
		InputLayoutElement *pEle = &pElements[i];

		Uint32 NumPos		= 0;
		Uint32 NumTexCoord	= 0;

		if(pEle->usage == ILU_POSITION)
		{
			DX11Layout[i].SemanticName	= "POSITION";
			DX11Layout[i].SemanticIndex	= NumPos;
		}
		else
		{
			DX11Layout[i].SemanticName = "TEXCOORD";
			DX11Layout[i].SemanticIndex	= NumTexCoord;
		}

	
	}

	INPUTLAYOUT layout;

	/*
	    const D3D11_INPUT_ELEMENT_DESC SceneLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        hr = pd3dDevice->CreateInputLayout( SceneLayout, ARRAYSIZE( SceneLayout ), pBlob->GetBufferPointer(),
                                                    pBlob->GetBufferSize(), &g_pSceneVertexLayout );
	*/

	return layout;
}






/*
BLENDSTATE CreateBlendState(eBLENDSTATE_FLAGS flags)
{

	BLENDSTATE BS;
	return BS;
}
*/


/*
 *
 *   DEPTH STENCIL STATE
 *
 */

DEPTHSTENCILSTATE RHI_CreateDepthStencilState(DEVICE device, RHI_DepthStencilDesc &RHIDesc)
{
	D3D11_DEPTH_STENCIL_DESC desc;
	memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
	
	if(RHIDesc.DepthEnable)
		desc.DepthEnable = true;
  
	if(RHIDesc.StencilEnable)
		desc.StencilEnable = true;

//	if(flags & eDEPTHSTENCILSTATE_DepthWrite)
	
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	switch (RHIDesc.DepthComp)
	{
	case RHIDC_Less:
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		break;

	case RHIDC_LessEqual:
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;

	case RHIDC_Greater:
		desc.DepthFunc = D3D11_COMPARISON_GREATER;
		break;

	case RHIDC_GreaterEqual:
		desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		break;

	case RHIDC_Always:
		desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		break;

	default:
		break;
	}
  
/*
BOOL                       StencilEnable;
  UINT8                      StencilReadMask;
  UINT8                      StencilWriteMask;
  D3D11_DEPTH_STENCILOP_DESC FrontFace;
  D3D11_DEPTH_STENCILOP_DESC BackFace;
 D3D11_DEPTH_STENCIL_DESC;
*/
	DEPTHSTENCILSTATE DSS;
	 device.pD3DDevice->CreateDepthStencilState(&desc, &DSS.pD3DDSS);

	return DSS;
}

void RHI_SetRenderTargets(DEVICE device, Uint32 NumRTs, RENDERTARGET *renderTarget, DEPTHSTENCIL *depthStencil)
{
	device.pD3DContext->OMSetRenderTargets(NumRTs, &renderTarget->pD3DRTV, depthStencil->pD3DDSV);
}


void RHI_SetDepthStencilState(DEVICE device, DEPTHSTENCILSTATE &dss)
{
	device.pD3DContext->OMSetDepthStencilState(dss.pD3DDSS,0);
}



/*
 *
 *   RASTERIZER STATE
 *
 */

RASTERIZERSTATE RHI_CreateRasterizerState(DEVICE device, RHI_RasterizerDesc &RHIDesc)
{
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(rastDesc));
	
	if(RHIDesc.WireFrameEnable)
		rastDesc.FillMode			= D3D11_FILL_WIREFRAME;
	else
		rastDesc.FillMode			= D3D11_FILL_SOLID;
	
	rastDesc.DepthClipEnable		= RHIDesc.DepthClipEnable;

	switch(RHIDesc.CullMode)
	{
	case RHIRastCull_BACK:
		rastDesc.CullMode			= D3D11_CULL_BACK;
		break;

	case RHIRastCull_FRONT:
		rastDesc.CullMode			= D3D11_CULL_FRONT;
		break;

	case RHIRastCull_NONE:
		rastDesc.CullMode			= D3D11_CULL_NONE;
		break;
	}

	RASTERIZERSTATE RS;
	device.pD3DDevice->CreateRasterizerState(&rastDesc, &RS.pD3DRS);

	return RS;
}

void RHI_SetRasterizerState(DEVICE device, RASTERIZERSTATE &rs)
{
	device.pD3DContext->RSSetState(rs.pD3DRS);
}

/*
 *
 *   BLEND STATE
 *
 */

BLENDSTATE RHI_CreateBlendState(DEVICE device, RHI_BlendDesc &RHIDesc)
{
	BLENDSTATE BS;

	D3D11_BLEND_DESC blendDesc;

	device.pD3DDevice->CreateBlendState(&blendDesc, &BS.pD3DBS);
	return BS;
}


//	RHI_BlendStateDesc blendDesc;
//	ZeroMemory(&blendDesc, sizeof(blendDesc));
//	blendDesc.AlphaToCoverageEnable		= FALSE;
//	blendDesc.IndependentBlendEnable	= FALSE;
//	blendDesc.RenderTarget[0].BlendEnable = FALSE;
//	g_Device.pD3DDevice->CreateBlendState(&blendDesc, &g_DefaultBlendState);


/*
 *
 *   VERTEX SHADER
 *
 */

void RHI_SetVertexShader(DEVICE device, VERTEXSHADER &vs)
{
	device.pD3DContext->VSSetShader(vs.pD3DVS, NULL, 0);
}



void RHI_SetPixelShader(DEVICE device, PIXELSHADER &ps)
{
	device.pD3DContext->PSSetShader(ps.pD3DPS, NULL, 0);
}

void RHI_SetComputeShader(DEVICE device, COMPUTESHADER &cs)
{
	device.pD3DContext->CSSetShader(cs.pD3DCS, NULL, 0);
}

/*
 *
 *   CONSTANT BUFFER
 *
 */


CONSTANTBUFFER	RHI_CreateConstantBuffer(DEVICE device, RHI_ConstantBufferDesc &desc)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= desc.ByteWidth;
	bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags	= 0;

	CONSTANTBUFFER cb;
	HRESULT hr = device.pD3DDevice->CreateBuffer(&bd, NULL, &cb.pD3DBuf);

	return cb;
}

void RHI_UpdateConstantBuffer(DEVICE device, CONSTANTBUFFER &cb, void *Data)
{
	device.pD3DContext->UpdateSubresource(cb.pD3DBuf, 0, NULL, Data, 0, 0);
}


void RHI_SetConstantBuffer_PS(DEVICE device, Uint32 StartSlot, Uint32 NumSlots, CONSTANTBUFFER &cb)
{
	device.pD3DContext->PSSetConstantBuffers(StartSlot, NumSlots, &cb.pD3DBuf);

}

void RHI_SetConstantBuffer_VS(DEVICE device, Uint32 StartSlot, Uint32 NumSlots, CONSTANTBUFFER &cb)
{
	device.pD3DContext->VSSetConstantBuffers(StartSlot, NumSlots, &cb.pD3DBuf);
}


void RHI_SetConstantBuffer_VS_PS(DEVICE device, Uint32 StartSlot, Uint32 NumSlots, CONSTANTBUFFER &cb)
{
	device.pD3DContext->PSSetConstantBuffers(StartSlot, NumSlots, &cb.pD3DBuf);
	device.pD3DContext->VSSetConstantBuffers(StartSlot, NumSlots, &cb.pD3DBuf);
}


/*
 *
 *   SAMPLER STATE
 *
 */

SAMPLERSTATE RHI_CreateSamplerState(DEVICE device, RHI_SamplerDesc &desc)
{
	D3D11_SAMPLER_DESC sampDesc;

	ZeroMemory(&sampDesc, sizeof(sampDesc));
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	if(desc.Comp == RHIDC_Never)
	{
		if(desc.Filter == RHISamplerFilter_Linear)
			sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		else
			sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else
		sampDesc.Filter			= D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	
	
	//ANISOTROPIC;
//	sampDesc.Filter			= D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy	= desc.MaxAniso;

	switch(desc.AddressU)
	{
	case RHISamplerTA_Wrap:
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case RHISamplerTA_Clamp:
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case RHISamplerTA_Border:
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	}

	switch(desc.AddressV)
	{
	case RHISamplerTA_Wrap:
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case RHISamplerTA_Clamp:
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case RHISamplerTA_Border:
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	}

	switch(desc.AddressW)
	{
	case RHISamplerTA_Wrap:
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case RHISamplerTA_Clamp:
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case RHISamplerTA_Border:
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		break;
	}

	switch (desc.Comp)
	{
	case RHIDC_Never:
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		break;

	case RHIDC_Less:
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		break;

	case RHIDC_LessEqual:
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		break;

	case RHIDC_Greater:
		sampDesc.ComparisonFunc = D3D11_COMPARISON_GREATER;
		break;

	case RHIDC_GreaterEqual:
		sampDesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
		break;

	default:
		break;
	}

	sampDesc.BorderColor[0] = desc.BorderColor[0];
	sampDesc.BorderColor[1] = desc.BorderColor[1];
	sampDesc.BorderColor[2] = desc.BorderColor[2];
	sampDesc.BorderColor[3] = desc.BorderColor[3];
	
	// LOD
	sampDesc.MinLOD			= desc.MinLOD;
	sampDesc.MaxLOD			= desc.MaxLOD;
	sampDesc.MipLODBias		= desc.MipLODBias;

	SAMPLERSTATE sampler;
	HRESULT hres = g_Device.pD3DDevice->CreateSamplerState(&sampDesc, &sampler.pD3DSampler);
	return sampler;
}

void RHI_SetSamplerState_PS(DEVICE device, Uint32 StartSlot, Uint32 NumSamplers,  SAMPLERSTATE &sampler)
{
	g_Device.pD3DContext->PSSetSamplers(StartSlot, NumSamplers,  &sampler.pD3DSampler);
}




void DEVICE::Release()
{
	pD3DContext->ClearState();
	SAFE_RELEASE(pD3DContext);
	SAFE_RELEASE(pD3DDevice);
}

void TEXTURE2D::Release()
{
}
void DEPTHSTENCIL::Release()
{
	SAFE_RELEASE(pD3DTexture2D);
	SAFE_RELEASE(pD3DDSV);
}
void RENDERTARGET::Release()
{
	SAFE_RELEASE(pD3DTexture2D);
	SAFE_RELEASE(pD3DRTV);
}
void VERTEXBUFFER::Release()
{
	SAFE_RELEASE(pD3DVB);
}
void INDEXBUFFER::Release()
{
	SAFE_RELEASE(pD3DIB);
}
void VERTEXDECL::Release()
{
}
void VERTEXSHADER::Release()
{
	SAFE_RELEASE(pD3DVS);
}
void VERTEXSHADER::ReleaseBlob()
{
	SAFE_RELEASE(pBlob);
}

void PIXELSHADER::Release()
{
	SAFE_RELEASE(pD3DPS);
}
void PIXELSHADER::ReleaseBlob()
{
    SAFE_RELEASE(pBlob);
}

void INPUTLAYOUT::Release()
{
	SAFE_RELEASE(pD3DIL);
}
void DEPTHSTENCILSTATE::Release()
{
	SAFE_RELEASE(pD3DDSS);
}

void RASTERIZERSTATE::Release()
{
	SAFE_RELEASE(pD3DRS);
}
void BLENDSTATE::Release()
{
	SAFE_RELEASE(pD3DBS);
}
void CONSTANTBUFFER::Release()
{
	SAFE_RELEASE(pD3DBuf);
}

void SAMPLERSTATE::Release()
{
	SAFE_RELEASE(pD3DSampler);
}











#if 0
ID3D11Buffer*      g_pVertexBuffer;

// Define the data-type that
// describes a vertex.
struct SimpleVertexCombined
{
    D3DXVECTOR3 Pos;  
    D3DXVECTOR3 Col;  
};

// Supply the actual vertex data.
SimpleVertexCombined verticesCombo[] =
{
    D3DXVECTOR3( 0.0f, 0.5f, 0.5f ),
    D3DXVECTOR3( 0.0f, 0.0f, 0.5f ),
    D3DXVECTOR3( 0.5f, -0.5f, 0.5f ),
    D3DXVECTOR3( 0.5f, 0.0f, 0.0f ),
    D3DXVECTOR3( -0.5f, -0.5f, 0.5f ),
    D3DXVECTOR3( 0.0f, 0.5f, 0.0f ),
};

// Fill in a buffer description.
D3D11_BUFFER_DESC bufferDesc;
bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
bufferDesc.ByteWidth        = sizeof( SimpleVertexCombined ) * 3;
bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
bufferDesc.CPUAccessFlags   = 0;
bufferDesc.MiscFlags        = 0;

// Fill in the subresource data.
D3D11_SUBRESOURCE_DATA InitData;
InitData.pSysMem = verticesCombo;
InitData.SysMemPitch = 0;
InitData.SysMemSlicePitch = 0;

// Create the vertex buffer.
hr = g_pd3dDevice->CreateBuffer( &bufferDesc, &InitData, &g_pVertexBuffer );

#endif
