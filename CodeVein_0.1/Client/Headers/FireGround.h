#pragma once

#include "Monster.h"

BEGIN(Client)

class Engine::CEffect;
class CFireGround final : public CMonster
{
protected:
	explicit CFireGround(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFireGround(const CFireGround& rhs);
	virtual ~CFireGround() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;


	_bool				m_bFinishBoom = false;
	_bool				m_bStartBoom = false;
	_double				m_dCurBoomTime = 0;

	_bool				m_bFinishCol = false;
	_bool				m_bFireTickDmg = false;

	_bool				m_bEffectReadyFireOn = false;
	_bool				m_bEffectFloorOn = false;
	_float				m_fEffectOffset = false;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

	_double				m_dTimeDelta = 0;

private:
	HRESULT Update_Collider();

private:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Collider();

public:
	static CFireGround* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END