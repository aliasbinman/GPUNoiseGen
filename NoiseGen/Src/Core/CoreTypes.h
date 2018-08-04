#ifndef _CORETYPES_H_
#define _CORETYPES_H_

//#include "Math.h"
#include "glm\vec4.hpp"
#include "glm\vec2.hpp"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"

typedef long long		Uint64;
typedef unsigned int	Uint32;
typedef   signed int	Sint32;
typedef unsigned short  Uint16;
typedef   signed short  Sint16;
typedef unsigned char	Uint8;
typedef float FLOAT;

typedef glm::vec4		Vec4;
typedef glm::vec3		Vec3;
typedef glm::vec2		Vec2;
typedef glm::mat4		Mat4;

const float PI			= 3.1415926535f;
const float TWOPI		= PI*2.0f;
const float HALFPI		= PI*0.5f;
const float QUARTERFPI	= PI*0.25f;
const Uint32 UNREF		= 0xffffffff;

__inline Uint32 Max(Uint32 A, Uint32 B) { return A > B ? A : B;}

#endif


