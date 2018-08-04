

Texture2D    txTex        : register( t0 );
Texture2D    txNoise      : register( t1 );

SamplerState samLinear  : register( s0 );
SamplerState samPoint   : register( s1 );

#define UVSZ  256.0f





Texture2D<float4>			ColorBuf ;
groupshared uint			g_TileHistogram[256];
RWBuffer<uint>				Histogram;
Buffer<uint>				HistogramIn;



cbuffer cbView: register( b0 )
{
    matrix View;
    matrix Projection;
    matrix WorldViewProjection;
	float  LerpVal;
	float  Pad1,Pad2;
	uint   NoiseOffset;
};


struct VS_INPUT_POSUV
{
	float3 Pos		: POSITION;
	float2 UV		: TEXCOORD0;
};

struct PS_INPUT_POSUV
{
    float4 Pos		: SV_POSITION;
    float2 UV		: TEXCOORD0;
};

float hash1( uint n ) 
{
    // integer hash copied from Hugo Elias
	n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U + NoiseOffset;
    return float( n & int(0x7fffffffU))/float(0x7fffffff);
}
float hash_IQ3( uint2 x )
{
    uint2 q = 1103515245U * ( (x>>1U) ^ (x.yx   ) );
    uint  n = 1103515245U * ( (q.x  ) ^ (q.y>>3U) );
    return float(n) * (1.0/float(0xffffffffU));
}
float hash(uint2 c)
{
	int x = 0x3504f333 * c.x*c.x + c.y;
	int y = 0xf1bbcdcb * c.y*c.y + c.x;

	return float(x*y)*(2.0 / 8589934592.0) + 0.5;
}




PS_INPUT_POSUV VS_Main(VS_INPUT_POSUV input)
{
	PS_INPUT_POSUV output = (PS_INPUT_POSUV)0;
	float4 inputPos = float4(input.Pos.xyz,1);

	output.Pos = mul(WorldViewProjection, inputPos);
	output.UV = output.UV;
	return output;
}


float4 PS_NoiseGen( PS_INPUT_POSUV input) : SV_Target
{
	float2 UV =input.UV;

	uint   n = (UV.x * 256) + (UV.y * 256 * 256);
	uint2 n2 = uint2((UV.x*256), (UV.y*256));
//	float4 Col = hash1(n);
	//float4 Col = hash( n2 ) ;
	float4 Col = hash_IQ3( n2 ) ;
	
	//Col = UV.x;
	return Col;
}




float4 DoBlur3(float2 UV, float2 Dir)
{
	float c =0;

    c += txTex.Sample( samPoint, UV  + (Dir * -1.0f) ).r * 1;
    c += txTex.Sample( samPoint, UV  + (Dir *  0.0f) ).r * 2;
    c += txTex.Sample( samPoint, UV  + (Dir *  1.0f) ).r * 1;
	c/= 4;
	return c;
}

float4 DoBlur(float2 UV, float2 Dir)
{
	float c =0;

    c += txTex.Sample( samPoint, UV  + (Dir * -2.0f) ).r * 1;
    c += txTex.Sample( samPoint, UV  + (Dir * -1.0f) ).r * 4;
    c += txTex.Sample( samPoint, UV  + (Dir *  0.0f) ).r * 6;
    c += txTex.Sample( samPoint, UV  + (Dir *  1.0f) ).r * 4;
    c += txTex.Sample( samPoint, UV  + (Dir *  2.0f) ).r * 1;
	c/= 6 + ((4+1)*2);

	return c;
}


float4 DoBlur7(float2 UV, float2 Dir)
{
	float c =0;

    c += txTex.Sample( samPoint, UV  + (Dir * -3.0f) ).r * 1;
    c += txTex.Sample( samPoint, UV  + (Dir * -2.0f) ).r * 6;
    c += txTex.Sample( samPoint, UV  + (Dir * -1.0f) ).r * 15;
    c += txTex.Sample( samPoint, UV  + (Dir *  0.0f) ).r * 20;
    c += txTex.Sample( samPoint, UV  + (Dir *  1.0f) ).r * 15;
    c += txTex.Sample( samPoint, UV  + (Dir *  2.0f) ).r * 6;
    c += txTex.Sample( samPoint, UV  + (Dir *  3.0f) ).r * 1;
	c/= 20 + ((15+6+1)*2);
	return c;
}

float4 DoBlur9(float2 UV, float2 Dir)
{
	float c =0;

    c += txTex.Sample( samPoint, UV  + (Dir * -4.0f) ).r * 10;
    c += txTex.Sample( samPoint, UV  + (Dir * -3.0f) ).r * 45;
    c += txTex.Sample( samPoint, UV  + (Dir * -2.0f) ).r * 120;
    c += txTex.Sample( samPoint, UV  + (Dir * -1.0f) ).r * 210;
    c += txTex.Sample( samPoint, UV  + (Dir *  0.0f) ).r * 252;
    c += txTex.Sample( samPoint, UV  + (Dir *  1.0f) ).r * 210;
    c += txTex.Sample( samPoint, UV  + (Dir *  2.0f) ).r * 120;
    c += txTex.Sample( samPoint, UV  + (Dir *  3.0f) ).r * 45;
    c += txTex.Sample( samPoint, UV  + (Dir *  4.0f) ).r * 10;
	c/= 252 + ((210 + 120 + 45 + 10)*2);
	return c;
}




float4 PS_Blur1( PS_INPUT_POSUV input) : SV_Target
{
	float2 UV =input.UV;
	float ooSZ = 1.0f / UVSZ;

//	UV += ooSZ*0.5f;

	float4 Col = DoBlur(UV, float2( ooSZ, 0.0f));
	return Col;
}


float4 PS_Blur2( PS_INPUT_POSUV input) : SV_Target
{
	float2 UV =input.UV;
	float ooSZ = 1.0f / UVSZ;

//	UV += ooSZ*0.5f;

	float4 Blur = DoBlur(UV, float2( 0.0f, ooSZ));

	// Renorm the blur
//	Blur -= 0.5;
//	Blur *= lerp(1,1+LerpVal *2  ,LerpVal);
//	Blur += 0.5;

	float4 Noise = txNoise.Sample( samPoint, UV );

	// Subtract off the blur off the noise
	float4 NoiseMinusBlur = 0.5+(Noise-Blur);
	//float4 NoiseMinusBlur = Blur ;//= 0.5+(Noise-Blur);
	float4 Col = lerp(Noise,NoiseMinusBlur, LerpVal);


	//float4 Col = lerp(Noise,Blur, LerpVal);


//	Col = ((Col-0.5)*1.1) + 0.5;

//	float4 Col = lerp(Noise,Blur, LerpVal);
	return Col;
}

float4 PS_Copy( PS_INPUT_POSUV input) : SV_Target
{
	float2 UV =input.UV;
	float4 Col = txTex.Sample( samPoint, UV );
	return Col;
}


float4 PS_Normalize( PS_INPUT_POSUV input) : SV_Target
{
	// Get the Rescaling from the histogram

	// Get the input pixel

	// Renormalize
	float2 UV =input.UV;
	float4 Col = txTex.Sample( samPoint, UV );
	return Col;
}



float4 PS_FinalOutput( PS_INPUT_POSUV input) : SV_Target
{
	float2 UV =input.UV;
	UV *= float2(1600/256,900/256)/2;
	float ooSZ = 1.0f / UVSZ;

	float4 Col = txTex.Sample( samLinear, UV ).r;




// Histogram
	{
		UV =input.UV * float2(1600,900) -float2(1600-256, 900-100) ;

		// Get Histogram Info
		uint MaxVal		=  HistogramIn[256+1];
		uint MinIndex	=  HistogramIn[256+2];
		uint MaxIndex	=  HistogramIn[256+3];
		float MaxValScale =  float(MaxVal) / 100.0f;

		if((UV.x>0) && (UV.y>0))
		{
			UV.y = 100 - UV.y;
			uint HistPos = (uint) UV.x;
			uint HistVal = HistogramIn[HistPos];

			// Get Background
			Col = 0;
			Col.r = frac(UV.y * (MaxValScale*0.001) );
			Col.b = (UV.x < MinIndex) ||  (UV.x > MaxIndex) ? 1 : 0;
			if ((uint)UV.y*MaxValScale < HistVal)
			{
				Col =1;
			}
		}
	}

//	Col = sqrt(Col);
	Col = Col * Col;

	return Col;
}

PS_INPUT_POSUV VS_Quad(VS_INPUT_POSUV input)
{
	PS_INPUT_POSUV output = (PS_INPUT_POSUV)0;
	output.Pos = float4(input.Pos.xyz, 1);
	output.UV = input.UV;
	return output;
}


[numthreads(16, 16, 1)]
void CS_ClearHistogram(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	Histogram[GI] =0;
}



[numthreads(16, 16, 1)]
void CS_HistogramGen(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	g_TileHistogram[GI] = 0;
	GroupMemoryBarrierWithGroupSync();
	
	// Get the Value at this pixel
	uint Col =ColorBuf[DTid.xy].r*256;
	InterlockedAdd(g_TileHistogram[Col], 1);

	GroupMemoryBarrierWithGroupSync();

	InterlockedAdd(Histogram[GI], g_TileHistogram[GI]);


//	Histogram.InterlockedAdd(GI * 4, g_TileHistogram[GI]);
}

groupshared uint	HistMinCount ;
groupshared uint	HistMaxCount ;
groupshared uint	HistMinIndex ;
groupshared uint	HistMaxIndex ;

[numthreads(16, 16, 1)]
void CS_HistogramAnalyze(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	if (GI == 0)
	{
		HistMinCount = 256*256*256;
		HistMaxCount = 0;
		HistMinIndex = 255;
		HistMaxIndex = 0;

	}
	GroupMemoryBarrierWithGroupSync();

	// We have a histogram of values we need to now find the min and max counts and also the min and max values which have some entry
	uint HistPos = GI;
	uint HistCount = Histogram[HistPos];

	// Get the min and max values
	InterlockedMin(HistMinCount, HistCount);
	InterlockedMax(HistMaxCount, HistCount);

	// Get the min and max indexes that contain a non zero count
	if (HistCount > 0)
	{
		InterlockedMin(HistMinIndex, HistPos);
		InterlockedMax(HistMaxIndex, HistPos);
	}

	GroupMemoryBarrierWithGroupSync();


	// Write these values into the last 4 elements
	if (GI == 0)
	{
		Histogram[256 + 0] = HistMinCount;
		Histogram[256 + 1] = HistMaxCount;
		Histogram[256 + 2] = HistMinIndex;
		Histogram[256 + 3] = HistMaxIndex;
	}
}


[numthreads(16, 16, 1)]
void CS_ProcessImage(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{



}




