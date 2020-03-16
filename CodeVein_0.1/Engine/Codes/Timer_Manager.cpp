#include "Timer_Manager.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}


CTimer_Manager::~CTimer_Manager()
{
	Free();
}

void CTimer_Manager::Set_DeltaTime(const _tchar * _TimerTag)
{
	CTimer* pInstance = Find_Timer(_TimerTag);

	if (nullptr == pInstance)
		return;

	pInstance->Set_DeltaTime();
}

_float CTimer_Manager::Get_DeltaTime(const _tchar * _TimerTag)
{
	CTimer* pInstance = Find_Timer(_TimerTag);

	if (nullptr == pInstance)
		return F_NULL;

	return pInstance->Get_DeltaTime();
}

CTimer * CTimer_Manager::Find_Timer(const _tchar * _TimerTag)
{
	auto iter = find_if(m_mTimer.begin(), m_mTimer.end(), CTag_Finder(_TimerTag));

	if (iter == m_mTimer.end())
		return nullptr;

	return iter->second;
}

HRESULT CTimer_Manager::initialize_Timer(const _tchar * _TimerTag)
{
	CTimer* pInstance = Find_Timer(_TimerTag);

	if (nullptr != pInstance)
	{
		//delete pInstance;
		pInstance = nullptr;
		return E_FAIL;
	}

	pInstance = CTimer::Create();

	m_mTimer.emplace(_TimerTag, pInstance);

	return S_OK;
}

void CTimer_Manager::Free()
{
	for_each(m_mTimer.begin(), m_mTimer.end(), CDeleteMap());
	m_mTimer.clear();
}
