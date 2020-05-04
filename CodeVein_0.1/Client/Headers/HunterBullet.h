#pragma once

#include "Monster.h"

BEGIN(Client)

class CHunterBullet final : public CMonster
{
protected:
	explicit CHunterBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHunterBullet(const CHunterBullet& rhs);
	virtual ~CHunterBullet() = default;

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
	virtual void Check_CollisionEvent();
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();

public:
	static CHunterBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTrail_VFX*			m_pTrailEffect = nullptr;
	CEffect*			m_pBulletBody_0 = nullptr;
	CEffect*			m_pBulletBody_1 = nullptr;
	CEffect*			m_pBulletBody_2 = nullptr;
	CEffect*			m_pBulletBody_3 = nullptr;

	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;
	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_bool				m_bEffect = true;
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

};

END
