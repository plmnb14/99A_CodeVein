#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CEffect_BloodChunk final : public CGameObject
{
public:
	typedef struct tagEffInfo
	{
		tagEffInfo(_v3 _vPos, _v3 _vDir)
			: vCreatePos(_vPos), vDirection(_vDir)
		{}
		_v3 vCreatePos;
		_v3 vDirection;
	}EFF_INFO;

protected:
	explicit CEffect_BloodChunk(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect_BloodChunk(const CEffect_BloodChunk& rhs);
	virtual ~CEffect_BloodChunk() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CEffect*			m_pBulletBody = nullptr;

	_v3					m_vDir = V3_NULL;
	_float				m_fSpeed = 0.f;
	_float				m_fAccel = 0.f;
	_float				m_fJumpPower = 0.f;
	_float				m_fCreatePosY = 0.f;

	_float				m_fDelay = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_bool				m_bEffect = true;
	_float				m_fEffectOffset = 0.f;

private:
	void Check_Move(_double TimeDelta);
	void Check_Delay(_double TimeDelta);

private:

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CEffect_BloodChunk* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END