#pragma once

#include "Monster.h"

BEGIN(Client)

class CCocoonBigBullet final : public CMonster
{
protected:
	explicit CCocoonBigBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCocoonBigBullet(const CCocoonBigBullet& rhs);
	virtual ~CCocoonBigBullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

private:
	void Update_Collider();
	void Render_Collider();
	virtual void Check_CollisionEvent();
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();

public:
	static CCocoonBigBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CEffect*			m_pBulletBody = nullptr;

	_v3					m_vDir = V3_NULL;
	_float				m_fSpeed = 0.f;
	_float				m_fEffectOffset = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;

	_bool				m_bDead = false;
	_bool				m_bEffect = true;

};

END




