#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CShader;
class ENGINE_DLL CFrameMgr : public CBase
{

DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr();
	virtual ~CFrameMgr();

public:
	_bool Lock_Frame(_float _LockValue , _float _DeltaTime);
	HRESULT Render_Frame(_float _DeltaTime);

public:
	HRESULT Init_ShaderConstant();
	HRESULT Ready_FrameShader();

private:
	_int		m_iFrame;
	_float		m_fTimeCount;
	_float		m_fTimeCount2;
	TCHAR		m_szFPS[STR_128];

	LARGE_INTEGER	m_CurTime;
	LARGE_INTEGER	m_OldTime;
	LARGE_INTEGER	m_FixedTime;
	LARGE_INTEGER	m_CPU_Tick;

	_float			m_fDeltaTime;

	_v4				m_vColor;

private:
	CShader*		m_pRenderShader = nullptr;

private:
	_mat			m_matWorld;
	_mat			m_matProj;
	_mat			m_matView;

public:
	virtual void Free();
};

END
