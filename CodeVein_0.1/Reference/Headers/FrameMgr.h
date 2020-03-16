#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFrameMgr : public CBase
{

DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	virtual ~CFrameMgr();

public:
	bool Lock_Frame(_float _LockValue , _float _DeltaTime);
	void Render_Frame(_float _DeltaTime);

private:
	_int		m_iFrame;
	_float		m_fTimeCount;
	_float		m_fTimeCount2;
	TCHAR		m_szFPS[MAX_STR];

	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_FixedTime;
	LARGE_INTEGER	m_CPU_Tick;

	_float			m_fDeltaTime;

public:
	virtual void Free();
};

END
