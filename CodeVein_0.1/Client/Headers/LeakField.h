#pragma once

#include "Monster.h"

BEGIN(Client)

class Engine::CEffect;
class CLeakField final : public CMonster
{
protected:
	explicit CLeakField(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CLeakField(const CLeakField& rhs);
	virtual ~CLeakField() = default;

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

	CTransform*			m_pTarget_Transform = nullptr;
	CAIController*		m_pTarget_AIController = nullptr;

	_float				m_fSpeed = 0.f;
	_float				m_fEffectOffset = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_double				m_dTimeDelta = 0;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지


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
	static CLeakField* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END