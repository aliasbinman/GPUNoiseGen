#include "RHI.h"


TEXTURE_DESC::TEXTURE_DESC(Uint32 W, Uint32 H)
{
	Width		= W;
	Height		= H;
	MipLevels	= 1;
	ArraySize	= 1;
	Format		= TEXFMT_R8G8B8A8;
	Flags		= TEXCREATE_DEFAULT;
}


TEXTURE_DESC::TEXTURE_DESC(Uint32 W, Uint32 H, eTEXTUREFORMAT FMT)
{
	Width		= W;
	Height		= H;
	MipLevels	= 1;
	ArraySize	= 1;
	Format		= FMT;
	Flags		= TEXCREATE_DEFAULT;
};



RHI_DepthStencilDesc::RHI_DepthStencilDesc()
{
	ZeroMemory(this, sizeof(RHI_DepthStencilDesc));
	// Sensible Defaults
	DepthEnable		= true;
	StencilEnable	= true;
	DepthComp		= RHIDC_LessEqual;
}


RHI_RasterizerDesc::RHI_RasterizerDesc()
{
	ZeroMemory(this, sizeof(RHI_RasterizerDesc));
	// Sensible Defaults
	WireFrameEnable		= false;
	DepthClipEnable		= true;
	CullMode			= RHIRastCull_BACK;
}


RHI_BlendDesc::RHI_BlendDesc()
{
	ZeroMemory(this, sizeof(RHI_BlendDesc));
	// Sensible Defaults
	AlphaToCoverageEnable		= false;
	IndependentBlendEnable		= false;
}

RHI_ConstantBufferDesc::RHI_ConstantBufferDesc()
{
	ZeroMemory(this, sizeof(RHI_ConstantBufferDesc));
}


RHI_SamplerDesc::RHI_SamplerDesc()
{
	ZeroMemory(this, sizeof(RHI_SamplerDesc));

	// Sensible Defaults
	Filter		= RHISamplerFilter_Linear;
	AddressU	= RHISamplerTA_Wrap;
	AddressV	= RHISamplerTA_Wrap;
	AddressW	= RHISamplerTA_Wrap;
	Comp		= RHIDC_Never;

	MinLOD		= 0.0f;
	MaxLOD		= 99999.0f;
	MipLODBias	= 0.0f;
}

