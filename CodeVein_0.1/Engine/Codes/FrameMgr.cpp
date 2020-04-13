#include "FrameMgr.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Management.h"

IMPLEMENT_SINGLETON(CFrameMgr)


CFrameMgr::CFrameMgr()
	:m_fTimeCount(1.f), m_iFrame(NULL), m_szFPS(L""), m_fTimeCount2(1.f)
{
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FixedTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CPU_Tick, sizeof(LARGE_INTEGER));

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matProj);
}


CFrameMgr::~CFrameMgr()
{
}

_bool CFrameMgr::Lock_Frame(_float _LockValue, _float _DeltaTime)
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

HRESULT CFrameMgr::Render_Frame(_float _DeltaTime)
{
	if (nullptr == m_pRenderShader)
		return E_FAIL;

	m_fTimeCount2 += _DeltaTime;

	if (1.f <= m_fTimeCount2)
	{
		m_vColor = (m_iFrame >= 50.f ? _v4(255.f, 0.f, 255.f, 0.f) :
			m_iFrame >= 40.f ? _v4(255.f, 0.f, 0.f, 255.f) :
			m_iFrame >= 30.f ? _v4(255.f, 255.f, 127.f, 0.f) : _v4(255.f, 255.f, 0.f, 0.f));

		swprintf_s(m_szFPS, L"[ FPS : %d ]", m_iFrame);
		m_iFrame = 0;
		m_fTimeCount2 = 0.f;
	}

	if (FAILED(Init_ShaderConstant()))
		return E_FAIL;

	m_pRenderShader->Begin_Shader();
	m_pRenderShader->Begin_Pass(0);

	CGraphic_Device::Get_Instance()->Get_Font()->DrawText(
		CGraphic_Device::Get_Instance()->Get_Sprite(),
		m_szFPS, lstrlen(m_szFPS), nullptr, 0,
		D3DCOLOR_ARGB(255, (_uint)m_vColor.y, (_uint)m_vColor.z, (_uint)m_vColor.w));

	m_pRenderShader->End_Pass();
	m_pRenderShader->End_Shader();

	return S_OK;
}

HRESULT CFrameMgr::Init_ShaderConstant()
{
	if (nullptr == m_pRenderShader)
		return E_FAIL;

	if (FAILED(m_pRenderShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	m_matView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);

	if (FAILED(m_pRenderShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pRenderShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFrameMgr::Ready_FrameShader()
{
	// 월드
	m_matWorld._11 = 1.f;
	m_matWorld._22 = 1.f;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = 30.f;
	m_matWorld._42 = 20.f;

	// 직교투영
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRenderShader = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_UI", nullptr));

	return S_OK;
}

void CFrameMgr::Free()
{
	Safe_Release(m_pRenderShader);
}
