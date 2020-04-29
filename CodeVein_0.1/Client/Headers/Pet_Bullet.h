#pragma once

#include "Info_Monster.h"

BEGIN(Client)

class CPet_Bullet final : public CGameObject
{
protected:
	explicit CPet_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPet_Bullet(const CPet_Bullet& rhs);
	virtual ~CPet_Bullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Update_Collider();
	void Render_Collider();
	void Update_Trails(_double TimeDelta);
	virtual void Enter_Collision();
	virtual void Check_CollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();
	HRESULT Ready_Effect(void* pArg);

public:
	static CPet_Bullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CEffect*			m_pBulletBody = nullptr;

	PET_BULLET_TYPE		m_eBulletType = PET_BULLET_TYPE::PET_BULLET_NONE;

	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_float				m_fEffectOffset = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;

	_bool				m_bDead = false;
	_bool				m_bEffect = true;

};

END
