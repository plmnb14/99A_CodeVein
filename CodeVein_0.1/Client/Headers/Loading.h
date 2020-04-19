#ifndef Loading_h__
#define Loading_h__

// ������ X
// ���� ���� ���� ������ü��(������Ʈ, ������Ʈ)�� �غ��Ű��.

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
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	SCENEID				m_eLoadingID;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bFinish;
	_tchar				m_szString[128];

private:
	_bool				m_bLoadStaticMesh = false;
	_bool				m_bLoadDynamicMesh = false;

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENEID eLoadingID);
	virtual	void Free(void);
};

END

#endif // Loading_h__
