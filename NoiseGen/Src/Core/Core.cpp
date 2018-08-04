#include "Core.h"
#include <memory.h>

Uint32 g_Window_W;
Uint32 g_Window_H;

void ClearMem(void *pmem, Uint32 SizeElement, Uint32 SizeArray)
{
	memset(pmem, 0 , SizeElement* SizeArray);
}

void SetMem(void *pmem, Uint32 set, Uint32 SizeElement, Uint32 SizeArray)
{
	memset(pmem, set , SizeElement* SizeArray);
}

void CopyMem(void *pDst, void *pSrc, Uint32 Size)
{
	memcpy(pDst, pSrc, Size);

}

Sint16 FloatToSint16(float x)
{
	if (x <= 0.0f)
		return 0;

	if (x >= 1.0f)
		return 32767;

	return (Sint16) (x*32767.0f);
}

Uint16 FloatToUint16(float x)
{
	if(x<=0.0f)
		return 0;

	if(x>=1.0f)
		return 65535;

	return (Uint16) (x*65535.0f);
}

Uint32 FloatColToUint32(float r, float g, float b)
{
	return (Uint32(1.0f*255.0f)<<24) + (Uint32(r*255.0f)<<16) + (Uint32(g*255.0f)<<8) + (Uint32(b*255.0f)<<0)  ;
}


FloatCol Uint32ColToFloat(Uint32 InCol)
{
	FloatCol Col;
	Col.b = (float)((InCol >> 0 ) &0xff) / 255.0f;
	Col.g = (float)((InCol >> 8 ) &0xff) / 255.0f;
	Col.r = (float)((InCol >> 16) &0xff) / 255.0f;
	Col.a = (float)((InCol >> 24) &0xff) / 255.0f;
	return Col;
}
