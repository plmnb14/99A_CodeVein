#pragma once

#include "Monster.h"

BEGIN(Client)

class Engine::CEffect;
class CIceBarrier final : public CMonster
{
protected:
	explicit CIceBarrier(LPDIRECT3DDEVICE9 pGraphic_device);
	explicit CIceBarrier(const CIceBarrier& rhs);
	virtual ~CIceBarrier() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*			m_pTarget_Transform = nullptr;
	CAIController*		m_pTarget_AIController = nullptr;

	CEffect*			m_pBarrierBody = nullptr;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_float				m_fEffectOffset = 0.f;

	// 베러어 변수들
	_bool				m_bBarrierOff = false;
	_double				m_dTime_BarrierOff = 0;

private:
	HRESULT Update_Collider();

private:
	void Check_PhyCollider();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Collider();

public:
	static CIceBarrier* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END