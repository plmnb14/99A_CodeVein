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
	explicit CLoading(_Device pGraphicDev);
	virtual ~CLoading(void) = default;

public:
	SCENEID Get_LoadingID(void) const { return m_eLoadingID; }
	CRITICAL_SECTION* Get_Crt(void) { return &m_Crt; }
	_bool Get_Finish(void) const { return m_bFinish; }
	_ulong Get_LoadingCnt() { return m_dwLoadingCnt; }

public:
	virtual void Set_LoadStaticMesh(_bool _bLoadStatic) { m_bLoadStaticMesh = _bLoadStatic; }
	virtual void Set_LoadDynamicMesh(_bool _bLoadDynamic) { m_bLoadDynamicMesh = _bLoadDynamic; }

public:
	HRESULT Ready_Loading(SCENEID eLoadingID);

public:
	static unsigned int CALLBACK Thread_Main(void* pArg);

private:
	HRESULT Ready_Effect(void);
	Engine::EFFECT_INFO* Read_EffectData(const _tchar* szPath);
	HRESULT Add_EffectPrototype(const _tchar* szName, _bool bIsMesh = false);

private:
	_uint Loading_Title();
	_uint Loading_Stage();
	_uint Loading_MainStages();

private:
	HRESULT Ready_Intro_MonsterPrototype();

private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	SCENEID				m_eLoadingID = SCENE_LOGO;
	_Device				m_pGraphicDev = nullptr;
	_bool				m_bFinish = false;

private:
	_ulong				m_dwLoadingCnt = 0;

private:
	_bool				m_bLoadStaticMesh = false;
	_bool				m_bLoadDynamicMesh = false;

public:
	static CLoading* Create(_Device pGraphicDev, SCENEID eLoadingID);
	virtual	void Free(void);
};

END

#endif // Loading_h__
