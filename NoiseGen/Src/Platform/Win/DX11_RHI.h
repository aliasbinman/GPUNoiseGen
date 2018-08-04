#include "../RHI.h"

#include "d3d11.h"

class DEVICE
{
public:
	
	ID3D11Device			*pD3DDevice; 
	ID3D11DeviceContext		*pD3DContext;
	HWND					hWnd;
	HINSTANCE				hInstance;

	void					Release();
};
extern DEVICE				g_Device;

class TEXTURE2D 
{
public:
	
	ID3D11Texture2D				*pD3DTexture2D;
	ID3D11ShaderResourceView	*pD3DSRV;

	TEXTURE2D() {pD3DTexture2D = nullptr; pD3DSRV = nullptr;}
	void					Release();
};


class DEPTHSTENCIL 
{
public:
	ID3D11Texture2D			*pD3DTexture2D;
	ID3D11DepthStencilView	*pD3DDSV;
	ID3D11ShaderResourceView	*pD3DSRV;
	void					Release();
};

class RENDERTARGET 
{
public:
	ID3D11Texture2D			*pD3DTexture2D;
	ID3D11RenderTargetView	*pD3DRTV;
	ID3D11ShaderResourceView	*pD3DSRV;
	void					Release();
};


class VERTEXBUFFER
{
public:
	
	ID3D11Buffer			*pD3DVB;
	Uint32					Stride;
	Uint32					Offset;

							VERTEXBUFFER();
	void					Release();
};


class INDEXBUFFER
{
public:
	
	ID3D11Buffer			*pD3DIB;
	Uint32					IdxSz;
	DXGI_FORMAT				Fmt;

							INDEXBUFFER();
	void					Release();
};
	

class VERTEXDECL
{
public:
	void					Release();
	
};

class VERTEXSHADER
{
public:
	ID3D11VertexShader		*pD3DVS;
    ID3DBlob				*pBlob;
	void					Release();
	void					ReleaseBlob();
};


class PIXELSHADER
{
public:
	
	ID3D11PixelShader		*pD3DPS;
    ID3DBlob				*pBlob;
	void					Release();
	void					ReleaseBlob();
};


class COMPUTESHADER
{
public:

	ID3D11ComputeShader		*pD3DCS;
	ID3DBlob				*pBlob;
	void					Release();
	void					ReleaseBlob();
};


class INPUTLAYOUT
{
public:

	ID3D11InputLayout		*pD3DIL;

	INPUTLAYOUT() { pD3DIL = nullptr;}
	void					Release();
};


/*
 * STATE - DEPTH/Stencil, Rasterizer, Blend
 */

class DEPTHSTENCILSTATE
{
public:

	ID3D11DepthStencilState *pD3DDSS;
	void					Release();
};

class RASTERIZERSTATE
{
public:

	ID3D11RasterizerState 	*pD3DRS;
	void					Release();
};

class BLENDSTATE
{
public:

	ID3D11BlendState		*pD3DBS;
	void					Release();
};

class CONSTANTBUFFER
{
public:

	ID3D11Buffer			*pD3DBuf;
	void					Release();
};


/*
 * Sampler
 */
class SAMPLERSTATE
{
public:

	ID3D11SamplerState		*pD3DSampler;
	void					Release();
};
