#include "FrameMgr.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"

IMPLEMENT_SINGLETON(CFrameMgr)


CFrameMgr::CFrameMgr()
	:m_fTimeCount(1.f), m_iFrame(NULL), m_szFPS(L""), m_fTimeCount2(1.f)
{
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FixedTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CPU_Tick, sizeof(LARGE_INTEGER));
}


CFrameMgr::~CFrameMgr()
{
}

bool CFrameMgr::Lock_Frame(_float _LockValue, _float _DeltaTime)
{
	m_fTimeCount += _DeltaTime;

	if (1.f / _LockValue < m_fTimeCount)
	{
		++m_iFrame;
		m_fTimeCount = 0.f;
		return true;
	}

	return false;
}

void CFrameMgr::Render_Frame(_float _DeltaTime)
{
	CTimer_Manager::Get_Instance()->Set_DeltaTime(L"FrameTime2");
	m_fTimeCount2 += CTimer_Manager::Get_Instance()->Get_DeltaTime(L"FrameTime2");

	if (1.f <= m_fTimeCount2)
	{
		swprintf_s(m_szFPS, L"FPS: %d", m_iFrame);
		m_iFrame = 0;
		m_fTimeCount2 = 0.f;
	}

	_mat matTrans;
	D3DXMatrixTranslation(&matTrans, 640.f, 100, 10.f);

	//프레임 나타내는 건 추후에 고침

	CGraphic_Device::Get_Instance()->Get_Sprite()->SetTransform(&matTrans);

	CGraphic_Device::Get_Instance()->Get_Font()->DrawText(
		CGraphic_Device::Get_Instance()->Get_Sprite(),
		m_szFPS, lstrlen(m_szFPS), nullptr, 0,
		D3DCOLOR_ARGB(255, 255, 0, 0));
}

void CFrameMgr::Free()
{
}
