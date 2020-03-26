#pragma once

#include "RenderObject.h"
#include "Management.h"

BEGIN(Engine)

class ENGINE_DLL CEffect abstract : public CGameObject
{
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	EFFECT_INFO* Get_Info() { return m_pInfo; }
	_tchar* Get_ParticleName() { return m_szParticleName; }
	_float	Get_CreateDelay() { return m_fCreateDelay; }

public:
	void Set_ParticleName(_tchar* szBuff) { lstrcpy(m_szParticleName, szBuff); }
	void Reset_Init();

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT LateInit_GameObject();
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	virtual void Setup_Info();

protected:
	EFFECT_INFO*			m_pInfo = nullptr;

	_float					m_fFrame = 0.f;
	_float					m_fAlpha = 1.f;
	_float					m_fLifeTime = 0.f;
	_float					m_fMoveSpeed = 0.f;
	_float					m_fLinearMoveSpeed = 0.f;
	_float					m_fLinearMovePercent = 0.f;
	_float					m_fRotSpeed = 0.f;
	_float					m_fAlphaSpeed = 0.f;
	_float					m_fCreateDelay = 0.f;
	_v3						m_vLerpPos = { 1.f, 1.f, 1.f };
	_v3						m_vLerpScale = { 1.f, 1.f, 1.f };
	_v3						m_vDir = { 1.f, 1.f, 1.f };
	_v3						m_vRot = { 0.f, 0.f, 0.f };
	_v3						m_vFollowPos = { 1.f, 1.f, 1.f };
	_v4						m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_bool					m_bClone = false;
	_bool					m_bFadeOutStart = false;

	_tchar					m_szParticleName[256] = L"";

public:
	virtual void Free();
};

END