#pragma once

#include "Monster.h"

BEGIN(Client)

class Engine::CEffect;
class CDeerKingColdBeam final : public CMonster
{
protected:
	explicit CDeerKingColdBeam(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDeerKingColdBeam(const CDeerKingColdBeam& rhs);
	virtual ~CDeerKingColdBeam() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CEffect*			m_pEffect = nullptr;
	_v3					m_vDir = _v3(0.f, 0.f, 0.f);

	_float				m_fEffectOffset = 0.f;
	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

private:
	_bool				m_bPlayerFriendly = false;		// �÷��̾� ����

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
	static CDeerKingColdBeam* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END