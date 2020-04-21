#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer();

private:
	HRESULT Initialize();

public:
	_float	Get_DeltaTime();

	void	Set_DeltaTime();
	void	Set_TimeMultiply(_float _time);
	void	Set_ResetMutiplyAccTime(_float _fDelayTime);

private:
	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_FixedTime;
	LARGE_INTEGER	m_CPU_Tick;

	_float			m_fDeltaTime			= 0.f;
	_float			m_fTimeMul				= 1.f;
	_float			m_fOriginDeltaTime		= 0.f;
	_float			m_fResetMultiplyAccTime = 0.f;
	_float			m_fResetMultiplyDelayTime = 0.f;

	_bool			m_bResetMultiplyTime	= false;

private:
	// DeltaTime 의 Multiply 수치를 _fDelayTime 이후 1.f 로 초기화 합니다.
	void	Reset_DeltaMutiply();

public:
	static CTimer* Create();
	virtual void Free();
};

END


