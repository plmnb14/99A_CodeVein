#include "..\Headers\Graphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

Engine::CGraphic_Device::CGraphic_Device(void)
{
	
}

HRESULT Engine::CGraphic_Device::Ready_GraphicDev(HWND hWnd, WINMODE eMode, const _uint& iSizeX, const _uint& iSizeY, LPDIRECT3DDEVICE9* ppGraphicDev)
{
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	
	if(nullptr == m_pSDK)
		return E_FAIL;

	D3DCAPS9			DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	if(FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)))
		return E_FAIL;

	_ulong		uFlag = 0;

	if(DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		uFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	else
		uFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;


	D3DPRESENT_PARAMETERS			Present_Parameter;
	ZeroMemory(&Present_Parameter, sizeof(D3DPRESENT_PARAMETERS));

	Present_Parameter.BackBufferWidth = iSizeX;
	Present_Parameter.BackBufferHeight = iSizeY;
	Present_Parameter.BackBufferFormat = D3DFMT_A8R8G8B8;
	Present_Parameter.BackBufferCount = 1;

	Present_Parameter.MultiSampleType = D3DMULTISAMPLE_NONE;
	Present_Parameter.MultiSampleQuality = 0;

	Present_Parameter.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Present_Parameter.hDeviceWindow = hWnd;

	Present_Parameter.Windowed = eMode;
	Present_Parameter.EnableAutoDepthStencil = TRUE;
	Present_Parameter.AutoDepthStencilFormat = D3DFMT_D24S8;

	Present_Parameter.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	Present_Parameter.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, uFlag, &Present_Parameter, &m_pGraphicDev)))
		return E_FAIL;

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
		return E_FAIL;

	D3DXFONT_DESC tFontInfo;
	ZeroMemory(&tFontInfo, sizeof(D3DXFONT_DESC));

	tFontInfo.Width = 10;
	tFontInfo.Height = 20;
	tFontInfo.Weight = FW_HEAVY;
	tFontInfo.CharSet = HANGEUL_CHARSET;
	lstrcpy(tFontInfo.FaceName, L"±Ã¼­");

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &tFontInfo, &m_pFont)))
		return E_FAIL;

	if(nullptr != ppGraphicDev)
	{
		*ppGraphicDev = m_pGraphicDev;
		Safe_AddRef(m_pGraphicDev);
	}

	return S_OK;
}

LPD3DXSPRITE CGraphic_Device::Get_Sprite()
{
	return m_pSprite;
}

LPD3DXFONT CGraphic_Device::Get_Font()
{
	return m_pFont;
}

void CGraphic_Device::Render_Begin()
{
	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(0, 0, 0, 0), 1.f, 0);

	m_pGraphicDev->BeginScene();
}

void CGraphic_Device::Render_End(HWND _hWnd)
{
	m_pGraphicDev->EndScene();

	m_pGraphicDev->Present(nullptr, nullptr, _hWnd, nullptr);
}

void CGraphic_Device::Render_Sprite_Begin()
{
	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void CGraphic_Device::Render_Sprite_End()
{
	m_pSprite->End();
}

void Engine::CGraphic_Device::Free(void)
{
	_ulong dwRefCnt = 0;

	if (dwRefCnt = Engine::Safe_Release(m_pFont))
		MSG_BOX("m_pGraphicDev Release Failed");

	if (dwRefCnt = Engine::Safe_Release(m_pSprite))
		MSG_BOX("m_pGraphicDev Release Failed");

	if(dwRefCnt = Engine::Safe_Release(m_pGraphicDev))
		MSG_BOX("m_pGraphicDev Release Failed");

	if(dwRefCnt = Engine::Safe_Release(m_pSDK))
		MSG_BOX("m_pSDK Release Failed");
}

