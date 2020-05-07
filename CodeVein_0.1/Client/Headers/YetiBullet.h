#pragma once

#include "Monster.h"

BEGIN(Client)

class CYetiBullet final : public CMonster
{
protected:
	explicit CYetiBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CYetiBullet(const CYetiBullet& rhs);
	virtual ~CYetiBullet() = default;

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
	static CYetiBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTrail_VFX*			m_pTrailEffect = nullptr;
	CEffect*			m_pBulletBody = nullptr;

	_tchar				m_pEffect_Tag0[STR_128] = { 0, };
	_tchar				m_pEffect_Tag1[STR_128] = { 0, };
	_tchar				m_pEffect_Tag2[STR_128] = { 0, };
	_tchar				m_pEffect_Tag3[STR_128] = { 0, };
	_tchar				m_pEffect_Tag4[STR_128] = { 0, };
	_tchar				m_pEffect_Tag5[STR_128] = { 0, };

	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;
	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_bool				m_bEffect = true;
	_bool				m_bPlayerFriendly = false;

};

END
