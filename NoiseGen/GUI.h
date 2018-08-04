#include"Core/Coretypes.h"
extern void ProcessGUI();

struct tGUI
{
	float		LerpVal;
	Sint32		NoiseOffset;
	Sint32		Iterations;

	tGUI::tGUI() 
	{
		LerpVal			= 1.0f;
		NoiseOffset		= 0;
		Iterations		= 0;
	}
};

extern tGUI gGUI;