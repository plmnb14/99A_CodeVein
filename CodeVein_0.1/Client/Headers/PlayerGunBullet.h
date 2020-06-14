#pragma once

#include "GameObject.h"
#include "Client_Defines.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CPlayerGunBullet final : public CGameObject
{
private:
	explicit CPlayerGunBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPlayerGunBullet(const CPlayerGunBullet& rhs);
	virtual ~CPlayerGunBullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Update_Trails(_double TimeDelta);
	void Update_Collider();
	void Render_Collider();
	virtual void Enter_Collision();
	virtual void Check_CollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();

public:
	static CPlayerGunBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTrail_VFX*			m_pTrailEffect = nullptr;
	CEffect*			m_pBulletBody = nullptr;

private:
	_v3					m_vDir = _v3(0.f, 0.f, 0.f);

	_float				m_fBullet_Speed = 0.f;

	_float				m_fEffectOffset = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;

	_bool				m_bDead = false;
	_bool				m_bEffect = true;
	_bool				m_bPlayerFriendly = true;
};

END