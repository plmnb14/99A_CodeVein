#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_WB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

private:
	explicit CInput_Device(void);
	virtual ~CInput_Device(void);

public:
	const	POINT	Get_ScreenCenter();
	const	POINT	Get_MouseGap();
	const	POINT	Get_MousePos();
	const	RAY		Get_Ray(const RAY_CALC _eCalc = RAY_TO_WORLD);
	const	_bool	MousePt_InRect(_float fRectPosX, _float fRectPosY, _float fRectSizeX, _float fRectSizeY, HWND hWnd);

public:
	const	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	const	_bool	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	const	_bool	Get_DIMouseState_Up(MOUSEKEYSTATE eMouse);
	const	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState) { return *(((_long*)&m_tMouseState) + eMouseState); }

public:
	void  Set_DPI(const _float _fDPI) { m_fDPI = _fDPI; }
	void  Set_MouseLock(const _bool _bOnCenter = true);

public:
	void  Calc_MouseLockPos();

public:
	const	_bool	Key_Pressing(_ubyte byKeyID);
	const	_bool	Key_Down(_ubyte byKeyID);
	const	_bool	Key_Up(_ubyte byKeyID);


public:
	HRESULT Ready_Input_Dev(HINSTANCE hInst, HWND hWnd);
	HRESULT	Set_InputDev(void);

private:
	_bool	m_bOnCenter = false;

private:
	_float	m_fDPI = 0.5f;

private:
	POINT m_CurMousePos = { 0,0 };
	POINT m_OldMousePos = { 0,0 };
	POINT m_MouseGap = { 0,0 };
	POINT m_MouseLockPos = { 0,0 };

private:
	HWND	m_hWnd = nullptr;

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	DIMOUSESTATE			m_tMouseState = { 0, };
	_bool					m_bMouseKeyDown[4] = {false, };
	_byte					m_byKeyState[MAX_STR];

	_byte					m_byKeyDown[MAX_STR];
	_byte					m_byKeyUp[MAX_STR];

public:
	virtual void	Free(void);

};

END
