#ifndef PCINPUTENUM_H_
#define PCINPUTENUM_H_

#define DIRECTINPUT_VERSION 0x800
#include <dinput.h>

#include "Core\CoreTypes.h"
//#include "Interface\IInputEnum.h"
#include "KeyboardCodes.h"

struct SMouseState
{
	Sint32	Axes1;
	Sint32	Axes2;
	Sint32	Axes3;

	Sint32	CursorXPos;
	Sint32	CursorYPos;

	bool	Button1;
	bool	Button2;
	bool	Button3;
	bool	Button4;

	bool	DebouncedButton1;
	bool	DebouncedButton2;
	bool	DebouncedButton3;
	bool	DebouncedButton4;

};

class CPCInputEnum //: public IInputEnum
{
private: 

	IDirectInput8*			m_DirectInput;
	HWND					m_hWnd;
	HINSTANCE				m_hInstance;

	// Mouse Internals
	bool					InitialiseMouse();
	IDirectInputDevice8*	m_MouseDevice;
	DIDEVICEINSTANCE		m_MouseDeviceInfo;
	DIMOUSESTATE2			m_MouseState;
	DIMOUSESTATE2			m_OldMouseState;
	DIDEVCAPS				m_MouseDeviceCapabilities;
	Sint32					m_xMin;
	Sint32					m_xMax;
	Sint32					m_yMin;
	Sint32					m_yMax;
	Sint32					m_zMin;
	Sint32					m_zMax;
	Sint32					m_CursorXPos;
	Sint32					m_CursorYPos;


	// Keyboard Internals
	bool					InitialiseKeyboard();
	IDirectInputDevice8*	m_KeyboardDevice;
	DIDEVICEINSTANCE		m_KeyboardDeviceInfo;
	DIDEVCAPS				m_KeyboardDeviceCapabilities;
	Uint8					m_KeyState[256];
	Uint8					m_OldKeyState[256];

	// Text Entry
	#define MAX_KEYCYCLICBUFFER 64	// MUST BE A POW 2
	Uint32					m_KeyCyclicPos;
	Uint32					m_KeyCyclicEnd;
	Uint8					m_KeyCyclicBuffer[MAX_KEYCYCLICBUFFER];


public:

	// Initialise / Shutdown
	bool					Initialise(HWND hWnd, HINSTANCE hInstance);
	bool					ShutDown();

	// Mouse
	bool					HasMouse();
	bool					RefreshMouseState();
	bool					GetMouseState(SMouseState *pMouseState);

	// Keyboard
	bool					HasKeyboard();
	bool					RefreshKeyboardState();
	Uint8					IsKeyPressed(Uint8 Key) { return m_KeyState[Key];}
	Uint8					IsDebouncedKeyPressed(Uint8 Key);

	// Text Entry Stuff
	void					SetTextInput(Uint8 Key);
	Uint8					GetTextInput(bool Clear);

	// Tick
	bool					Update();
};


extern CPCInputEnum *g_pInput;


#endif // PCINPUTENUM_H_
