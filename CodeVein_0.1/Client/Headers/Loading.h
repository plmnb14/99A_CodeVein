#ifndef Loading_h__
#define Loading_h__

// 렌더링 X
// 다음 씬을 위한 원형객체들(오브젝트, 컴포넌트)을 준비시키낟.

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoading final : public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading(void) = default;
public:
	SCENEID Get_LoadingID(void) const { return m_eLoadingID; }
	CRITICAL_SECTION* Get_Crt(void) { return &m_Crt; }
	_bool Get_Finish(void) const { return m_bFinish; }
	const _tchar* Get_String(void) const { return m_szString; }
public:
	HRESULT Ready_Loading(SCENEID eLoadingID);
	_uint Loading_ForStage(void);
public:
	static unsigned int CALLBACK Thread_Main(void* pArg);
private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	SCENEID				m_eLoadingID;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bFinish;
	_tchar				m_szString[128];

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENEID eLoadingID);
	virtual	void Free(void);
};

END

#endif // Loading_h__
