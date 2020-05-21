#pragma once

#include "Client_Defines.h"
#include "Scene.h"
#include "Loading.h"
#include "OrthoEffect.h"

#include "Management.h"

BEGIN(Client)
class CScriptManager;
class CScene_Stage_02 final : public CScene
{
private:
	explicit CScene_Stage_02(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene_Stage_02() = default;

public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();

public:
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Environment(const _tchar* pLayerTag);

private:
	void Create_Fog(_double TimeDelta);
	void Create_Dust(_double TimeDelta);
	void Create_Snow(_double TimeDelta);
	void Check_Effect_Fade();

private:
	CNavMesh*		m_pNavMesh = nullptr;

private:
	_float m_fMapFogDelay = 10.f;
	_float m_fMapWindDustDelay = 1.f;
	_float m_fMapSnowDelay = 1.f;

	_float m_fFogDestiny = 0.04f;
	_float m_fFogMinDestiny = 0.01f;

	_bool m_bFadeInStart = false;
	_bool m_bFadeOutStart = false;
	_float m_fFade = 1.f;

	COrthoEffect*		m_pSnowEffect_0 = nullptr;
	COrthoEffect*		m_pSmokeEffect_0 = nullptr;
	COrthoEffect*		m_pSmokeEffect_1 = nullptr;

	CRenderer*			m_pRenderer = nullptr;
	CScriptManager*		m_pScriptManager = nullptr;
private:
	HRESULT Ready_LightDesc();

public:
	static CScene_Stage_02* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic);
	virtual void Free();
};

END