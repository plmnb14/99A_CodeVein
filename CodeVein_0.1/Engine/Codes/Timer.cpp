#include "Timer.h"

USING(Engine)

CTimer::CTimer()
	:m_fDeltaTime(0)
{
	ZeroMemory(&m_CurTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_OldTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_FixedTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CPU_Tick, sizeof(LARGE_INTEGER));
}


CTimer::~CTimer()
{
}

void CTimer::Set_DeltaTime()
{
	QueryPerformanceCounter(&m_FixedTime);

	if (m_FixedTime.QuadPart - m_CurTime.QuadPart > m_CPU_Tick.QuadPart)
	{
		QueryPerformanceCounter(&m_CurTime);
		QueryPerformanceFrequency(&m_CPU_Tick);
	}

	m_fDeltaTime = (_float(m_FixedTime.QuadPart - m_OldTime.QuadPart) / m_CPU_Tick.QuadPart) * m_fTimeMul;
	m_fOriginDeltaTime = (_float(m_FixedTime.QuadPart - m_OldTime.QuadPart) / m_CPU_Tick.QuadPart);

	m_OldTime = m_FixedTime;

	if (m_bResetMultiplyTime)
	{
		Reset_DeltaMutiply();
	}
}

_float CTimer::Get_DeltaTime()
{
	return m_fDeltaTime;
}

void CTimer::Set_TimeMultiply(_float _time)
{
	m_fTimeMul = _time;
}

void CTimer::Set_ResetMutiplyAccTime(_float _fDelayTime)
{
	// _fDelayTime 가 지난 후에 Multiply 가 초기화 됩니다.

	if (_fDelayTime <= 0.f)
	{
		m_bResetMultiplyTime = false;
		m_fResetMultiplyAccTime = 0.f;
	}

	else
	{
		m_fResetMultiplyDelayTime = _fDelayTime;
		m_fResetMultiplyAccTime = 0.f;
		m_bResetMultiplyTime = true;
	}
}

void CTimer::Reset_DeltaMutiply()
{
	if (m_fResetMultiplyAccTime < m_fResetMultiplyDelayTime)
	{
		m_fResetMultiplyAccTime += m_fOriginDeltaTime;

		if (m_fResetMultiplyAccTime >= m_fResetMultiplyDelayTime)
		{
			m_bResetMultiplyTime = false;
			m_fResetMultiplyAccTime = 0.f;
			m_fResetMultiplyDelayTime = 0.f;
			m_fTimeMul = 1.f;
		}
	}
}

HRESULT CTimer::Initialize()
{
	QueryPerformanceCounter(&m_CurTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_FixedTime);

	QueryPerformanceFrequency(&m_CPU_Tick);

	return S_OK;
}

CTimer * CTimer::Create()
{
	CTimer* pInstance = new CTimer;

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}
