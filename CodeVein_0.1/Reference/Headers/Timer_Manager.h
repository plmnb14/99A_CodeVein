#pragma once

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class ENGINE_DLL CTimer_Manager : public CBase
{
DECLARE_SINGLETON(CTimer_Manager)

private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager();

public:
	_float	Get_DeltaTime(const _tchar* _TimerTag);
	void	Set_MutiplyTime(const _tchar* _TimerTag, const _float _fMutiplyTime);
	void	Set_MutiplyResetTime(const _tchar* _TimerTag, const _float _fMutiplyResetTime);

public:
	void	Set_DeltaTime(const _tchar* _TimerTag);
	HRESULT initialize_Timer(const _tchar* _TimerTag);

private:
	CTimer* Find_Timer(const _tchar* _TimerTag);

private:
	map<const _tchar*, CTimer*>		m_mTimer;

private:
	_float							m_fTimeMutiply = 1.f;

public:
	virtual void Free();

};

END
