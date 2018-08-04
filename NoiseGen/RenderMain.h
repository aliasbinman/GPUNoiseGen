#include "Core\Core.h"
#include "Platform\RHI.h"
#pragma once


enum ETriFillMode
{
	TriFillMode_SOLID		= 0,
	TriFillMode_WIRE		= 1,
	TriFillMode_SOLIDWIRE	= 2
};

enum ETriViewMode
{
	TriViewMode_COLOR		= 0,
	TriViewMode_NORMAL		= 1,
};