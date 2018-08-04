#ifndef _CORE_H_
#define _CORE_H_

//#pragma warning(push, disable: 4201)

#include <assert.h>
#include "glm\gtc\matrix_inverse.hpp" 

#include "CoreTypes.h"

extern Uint32 g_Window_W;
extern Uint32 g_Window_H;

// Useful functions
void ClearMem(void *pmem, Uint32 SizeElement, Uint32 SizeArray);
void SetMem(void *pmem, Uint32 set, Uint32 SizeElement, Uint32 SizeArray);
void CopyMem(void *pDst, void *pSrc, Uint32 Size);

struct FloatCol
{
	float r,g,b,a;
};

extern FloatCol Uint32ColToFloat(Uint32 InCol);

 template< typename  T> 
 __forceinline void DeleteVectorContents(T &V)
 {
	V.clear();
	V.shrink_to_fit();
 }


#endif


