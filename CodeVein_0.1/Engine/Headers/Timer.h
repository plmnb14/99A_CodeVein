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
	void	Set_DeltaTime();
	_float	Get_DeltaTime();
	void	Set_TimeMul(_float _time);

private:
	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_FixedTime;
	LARGE_INTEGER	m_CPU_Tick;

	_float			m_fDeltaTime	= 0.f;;
	_float			m_fTimeMul		= 1.f;

public:
	static CTimer* Create();
	virtual void Free();
};

END


