#include "Input_Device.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device(void)
{
	ZeroMemory(&m_byKeyDown, sizeof(_byte) * MAX_STR);
	ZeroMemory(&m_byKeyUp, sizeof(_byte) * MAX_STR);
}

CInput_Device::~CInput_Device(void)
{
	Free();
}

const POINT CInput_Device::Get_ScreenCenter()
{
	POINT pt;
	pt.x = WINCX / 2;
	pt.y = WINCY / 2;

	ClientToScreen(m_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);

	return pt;
}

const POINT CInput_Device::Get_MouseGap()
{
	GetCursorPos(&m_CurMousePos);
	ScreenToClient(m_hWnd, &m_CurMousePos);

	if (m_bOnCenter)
	{
		m_MouseGap.x = (LONG)((m_CurMousePos.x - (float)WINCX * 0.5f) * m_fDPI);
		m_MouseGap.y = (LONG)((m_CurMousePos.y - (float)WINCY * 0.5f) * m_fDPI);
	}

	else
	{
		m_MouseGap.x = (LONG)((m_CurMousePos.x - m_MouseLockPos.x) * m_fDPI);
		m_MouseGap.y = (LONG)((m_CurMousePos.y - m_MouseLockPos.y) * m_fDPI);
	}

	return m_MouseGap;
}

const POINT CInput_Device::Get_MousePos()
{
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	return pt;
}

const RAY CInput_Device::Get_Ray(const RAY_CALC _eCalc)
{
	RAY	 tRay;
	_v3	 vMouse = {};
	_mat matProj, matView, matWorld, matWorldInv;
	POINT pt = Get_MousePos();


	matProj = CPipeLine::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
	matView = CPipeLine::Get_Instance()->Get_Transform(D3DTS_VIEW);
	//matWorld = _matWorld;


	vMouse.x = (float(pt.x) / (WINCX >> 1) - 1.f) / matProj._11;
	vMouse.y = (float(-pt.y) / (WINCY >> 1) + 1.f) / matProj._22;
	vMouse.z = 1.f;


	tRay.vRayPos = V3_NULL;
	tRay.vRayDir = vMouse;
	V3_NORMAL_SELF(&tRay.vRayDir);


	if (_eCalc == RAY_TO_VIEW)
		return tRay;

	else
	{
		D3DXMatrixInverse(&matView, nullptr, &matView);

		D3DXVec3TransformCoord(&tRay.vRayPos, &tRay.vRayPos, &matView);
		D3DXVec3TransformNormal(&tRay.vRayDir, &tRay.vRayDir, &matView);
		D3DXVec3Normalize(&tRay.vRayDir, &tRay.vRayDir);


		//if (_eCalc == RAY_TO_WORLD)
		//	return tRay;
		//
		//else
		//{
		//	D3DXMatrixInverse(&matWorldInv, nullptr, &matWorld);
		//
		//	D3DXVec3TransformCoord(&tRay.vRayPos, &tRay.vRayPos, &matWorldInv);
		//	D3DXVec3TransformNormal(&tRay.vRayDir, &tRay.vRayDir, &matWorldInv);
		//	D3DXVec3Normalize(&tRay.vRayDir, &tRay.vRayDir);
		//}
	}

	return tRay;
}

void CInput_Device::Calc_MouseLockPos()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);

	m_MouseLockPos = pt;
}

const _bool CInput_Device::Key_Pressing(_ubyte byKeyID)
{
	if (m_byKeyState[byKeyID] & 0x80)
		return true;

	return false;
}

const _bool CInput_Device::Key_Down(_ubyte byKeyID)
{
	if (!m_byKeyDown[byKeyID] && m_byKeyState[byKeyID])
	{
		m_byKeyDown[byKeyID] |= 0x80;
		return true;
	}
	else if (m_byKeyDown[byKeyID] && !m_byKeyState[byKeyID])
	{
		m_byKeyDown[byKeyID] = 0x00;
		return false;
	}

	return false;
}

const _bool CInput_Device::Key_Up(_ubyte byKeyID)
{
	if (m_byKeyUp[byKeyID] && !m_byKeyState[byKeyID])
	{
		m_byKeyUp[byKeyID] = 0x00;
		return true;
	}
	else if (!m_byKeyUp[byKeyID] && m_byKeyState[byKeyID])
	{
		m_byKeyUp[byKeyID] |= 0x80;
		return false;
	}

	return false;
}

HRESULT Engine::CInput_Device::Ready_Input_Dev(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	if (FAILED((DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL))))
		return E_FAIL;

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;


	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

HRESULT Engine::CInput_Device::Set_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(MAX_STR, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	return S_OK;
}

void Engine::CInput_Device::Free(void)
{
	Engine::Safe_Release(m_pKeyBoard);
	Engine::Safe_Release(m_pMouse);
	Engine::Safe_Release(m_pInputSDK);
}

