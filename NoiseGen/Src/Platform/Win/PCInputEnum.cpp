#include <memory.h>
//#include "DXUT.h"
#include "PCInputEnum.h"


#pragma comment (lib, "dxguid.lib") 
#pragma comment (lib, "dinput8.lib") 

CPCInputEnum *g_pInput;

// Joypad Stuff


bool CPCInputEnum::Initialise(HWND hWnd, HINSTANCE hInstance)
{
HRESULT hRes;

	m_hWnd		= hWnd;
	m_hInstance	= hInstance;

	hRes = DirectInput8Create(m_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, 
		    				  (void**)&m_DirectInput, NULL);
	//DrASSERT_ERROR(result==DI_OK,"Couldn't create DirectInput");

	// Mouse
	InitialiseMouse();

	// Keyboard
	InitialiseKeyboard();

	return true;
}


bool CPCInputEnum::ShutDown()
{
	if (m_KeyboardDevice)
	{
		m_KeyboardDevice->Unacquire();
		m_KeyboardDevice->Release();
	}

	if (m_MouseDevice)
	{
		m_MouseDevice->Unacquire();
		m_MouseDevice->Release();
	}
	
	m_DirectInput->Release();				// Release main DINPUT object
	return true;
}


bool CPCInputEnum::Update()
{
	// Keyboard
	RefreshKeyboardState();

	// Mouse
	RefreshMouseState();

	// Joypad

	return true;
}




//
// MOUSE
//
bool CPCInputEnum::InitialiseMouse()
{
HRESULT hRes;

    hRes = m_DirectInput->CreateDevice(GUID_SysMouse,&m_MouseDevice,0);
				//DrASSERT_ERROR(result==DI_OK,"Couldn't create mouse device");
	
	memset(&m_MouseDeviceInfo, 0, sizeof(m_MouseDeviceInfo));
	m_MouseDeviceInfo.dwSize=sizeof(m_MouseDeviceInfo);
	m_MouseDevice->GetDeviceInfo(&m_MouseDeviceInfo);

	memset(&m_MouseDeviceCapabilities, 0, sizeof(m_MouseDeviceCapabilities));
	m_MouseDeviceCapabilities.dwSize=sizeof(m_MouseDeviceCapabilities);
	m_MouseDevice->GetCapabilities(&m_MouseDeviceCapabilities);


	///////////////
	m_MouseDevice->SetDataFormat( &c_dfDIMouse2 );
	m_MouseDevice->SetCooperativeLevel(m_hWnd, DISCL_FOREGROUND );//| DISCL_EXCLUSIVE); 
	

	HRESULT	result = m_MouseDevice->Acquire();
//	DrASSERT_WARNING(result==DI_OK,"Couldn't acquire device");

	DIPROPRANGE range;
	range.diph.dwSize=sizeof(DIPROPRANGE);
	range.diph.dwHeaderSize=sizeof(DIPROPHEADER);	
	range.diph.dwHow=DIPH_BYOFFSET;

	range.diph.dwObj=DIMOFS_X;
	result=m_MouseDevice->GetProperty(DIPROP_RANGE,&range.diph);
	m_xMin=range.lMin;
	m_xMax=range.lMax;

	range.diph.dwObj=DIMOFS_Y;
	result=m_MouseDevice->GetProperty(DIPROP_RANGE,&range.diph);
	m_yMin=range.lMin;
	m_yMax=range.lMax;

	range.diph.dwObj=DIMOFS_Z;
	result=m_MouseDevice->GetProperty(DIPROP_RANGE,&range.diph);
	m_zMin=range.lMin;
	m_zMax=range.lMax;

  /*
	m_axisStateBuffer1=new DrReal[m_MouseDeviceCapabilities.dwAxes];
	memset(m_axisStateBuffer1,0,m_MouseDeviceCapabilities.dwAxes*sizeof(DrReal));
	m_axisStateBuffer2=new DrReal[m_MouseDeviceCapabilities.dwAxes];
	memset(m_axisStateBuffer2,0,m_MouseDeviceCapabilities.dwAxes*sizeof(DrReal));
	m_currentAxisState=m_axisStateBuffer1;
	m_previousAxisState=m_axisStateBuffer2;

	m_buttonStateBuffer1=new DrUInt8[m_MouseDeviceCapabilities.dwButtons];
	memset(m_buttonStateBuffer1,0,m_MouseDeviceCapabilities.dwButtons*sizeof(DrUInt8));
	m_buttonStateBuffer2=new DrUInt8[m_MouseDeviceCapabilities.dwButtons];
	memset(m_buttonStateBuffer2,0,m_MouseDeviceCapabilities.dwButtons*sizeof(DrUInt8));
	m_currentButtonState=m_buttonStateBuffer1;
	m_previousButtonState=m_buttonStateBuffer2;
	
	pInputEnumerator->AttachDevice(this);
	*/


	return true;

}

bool CPCInputEnum::HasMouse()
{
	return true;
}


bool CPCInputEnum::RefreshMouseState()
{
HRESULT hRes;

	m_OldMouseState = m_MouseState;

	hRes = m_MouseDevice->Poll(); 
	if(hRes!=DI_OK) 		
	{
		hRes = m_MouseDevice->Acquire();

		while(hRes == DIERR_INPUTLOST) 
		{
			hRes = m_MouseDevice->Acquire();
		}
	}

	hRes = m_MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState);
	//	DrASSERT_WARNING(result==DI_OK,"Couldn't get device state");

	// Get Coords within the window
	POINT CurPos;	
	GetCursorPos(&CurPos);
	POINT WxPos;
	WxPos.x = CurPos.x;
	WxPos.y = CurPos.y;

	// Need to use directinput deltas here!

	ScreenToClient(m_hWnd, &CurPos);
	m_CursorXPos	= CurPos.x;
	m_CursorYPos	= CurPos.y;
	return true;
}


bool CPCInputEnum::GetMouseState(SMouseState *pState)
{
	pState->Axes1 = m_MouseState.lX;
	pState->Axes2 = m_MouseState.lY;
	pState->Axes3 = m_MouseState.lZ;

	pState->Button1 = m_MouseState.rgbButtons[0] != 0;
	pState->Button2 = m_MouseState.rgbButtons[1] != 0;
	pState->Button3 = m_MouseState.rgbButtons[2] != 0;

	if(m_MouseState.rgbButtons[0]==0x80 && m_OldMouseState.rgbButtons[0]==0x0)
		pState->DebouncedButton1 = true;
	else
		pState->DebouncedButton1 = false;

	if(m_MouseState.rgbButtons[1]==0x80 && m_OldMouseState.rgbButtons[1]==0x00)
		pState->DebouncedButton2 = true;
	else
		pState->DebouncedButton2 = false;

	if(m_MouseState.rgbButtons[2]==0x80 && m_OldMouseState.rgbButtons[2]==0x00)
		pState->DebouncedButton3 = true;
	else
		pState->DebouncedButton3 = false;

	POINT ptCursor;
    GetCursorPos(  &ptCursor );
    ScreenToClient( m_hWnd, &ptCursor );

	pState->CursorXPos = ptCursor.x;
	pState->CursorYPos = ptCursor.y;

	   	/*
	// Swap buffers
	DrReal* tempAxes=m_currentAxisState;
	m_currentAxisState=m_previousAxisState;
	m_previousAxisState=tempAxes;
	DrUInt8* tempButtons=m_currentButtonState;
	m_currentButtonState=m_previousButtonState;
	m_previousButtonState=tempButtons;

	// Fill buffers
	for (DrInt32 button=0; button<(DrInt32)m_deviceCapabilities.dwButtons; button++)
	{
		m_currentButtonState[button]=m_state.rgbButtons[button];
	}
	if (m_deviceCapabilities.dwAxes>0)
		m_currentAxisState[0]=(m_state.lX/64.f);
	if (m_deviceCapabilities.dwAxes>1)
		m_currentAxisState[1]=(m_state.lY/64.f);
	if (m_deviceCapabilities.dwAxes>2)
		m_currentAxisState[2]=(m_state.lZ/64.f);

	*/



	return true;

}





//
// KEYBOARD
//
bool CPCInputEnum::InitialiseKeyboard()
{
HRESULT hRes;
	

	hRes = m_DirectInput->CreateDevice(GUID_SysKeyboard,&m_KeyboardDevice,0);
	//DrASSERT_ERROR(result==DI_OK,"Couldn't create keyboard device");
	m_KeyboardDeviceInfo.dwSize=sizeof(m_KeyboardDeviceInfo);
	m_KeyboardDevice->GetDeviceInfo(&m_KeyboardDeviceInfo);
	m_KeyboardDeviceCapabilities.dwSize=sizeof(m_KeyboardDeviceCapabilities);
	m_KeyboardDevice->GetCapabilities(&m_KeyboardDeviceCapabilities);

	//	DrASSERT_ERROR(!m_started,"Device already started");

	m_KeyboardDevice->SetDataFormat( &c_dfDIKeyboard );
	m_KeyboardDevice->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE); 
	hRes = m_KeyboardDevice->Acquire();
//	DrASSERT_WARNING(result==DI_OK,"Couldn't acquire device");
//	m_currentButtonState=m_buttonStateBuffer1;
//	m_previousButtonState=m_buttonStateBuffer2;
//	pInputEnumerator->AttachDevice(this);

	memset(m_KeyState, 0 , sizeof(m_KeyState));

	m_KeyCyclicPos = 0;
	m_KeyCyclicEnd = 0;

	return true;
}

bool CPCInputEnum::RefreshKeyboardState()
{
HRESULT	hRes;

//	DrASSERT_ERROR(m_started,"Device not started");
	memcpy(m_OldKeyState, m_KeyState, sizeof(m_KeyState));

	hRes = m_KeyboardDevice->Poll(); 
	if(hRes!=DI_OK) 		
	{
		hRes = m_KeyboardDevice->Acquire();

		while(hRes == DIERR_INPUTLOST) 
		{
			hRes = m_KeyboardDevice->Acquire();
		}
	}

	// Swap buffers
//	DrUInt8* tempButtons=m_currentButtonState;
//	m_currentButtonState=m_previousButtonState;
//	m_previousButtonState=tempButtons;

	hRes = m_KeyboardDevice->GetDeviceState(sizeof(Uint8)*256, m_KeyState);
//	DrASSERT_WARNING(result==DI_OK,"Couldn't get device state");
	return true;
}

Uint8 CPCInputEnum::IsDebouncedKeyPressed(Uint8 Key) 
{ 
	return m_KeyState[Key] && !(m_OldKeyState[Key]);
}

bool CPCInputEnum::HasKeyboard()
{
	return true;
}


void  CPCInputEnum::SetTextInput(Uint8 Key)
{
 	m_KeyCyclicBuffer[m_KeyCyclicEnd] = Key;
 	m_KeyCyclicEnd++;
	m_KeyCyclicEnd &= (MAX_KEYCYCLICBUFFER-1);

	// Determine if buffer is full
	if(m_KeyCyclicEnd == m_KeyCyclicPos)
	{
		m_KeyCyclicPos++;
		m_KeyCyclicPos &= (MAX_KEYCYCLICBUFFER-1);
	}

}

Uint8 CPCInputEnum::GetTextInput(bool Clear)
{
	if(Clear)
	{
		m_KeyCyclicEnd = m_KeyCyclicPos = 0;
		return 0;
	}

	if(m_KeyCyclicEnd == m_KeyCyclicPos)
		return 0;

	Uint8 Key = m_KeyCyclicBuffer[m_KeyCyclicPos];
	m_KeyCyclicPos++;
	m_KeyCyclicPos &= (MAX_KEYCYCLICBUFFER-1);
	return Key;
}

/*


IInputEnum *GetInputEnum()
{
	pInputEnum = new CPCInputEnum;
	return pInputEnum;
}




*/