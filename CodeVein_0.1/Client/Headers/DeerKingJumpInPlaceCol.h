#pragma once

#include "Monster.h"

BEGIN(Client)

class Engine::CEffect;
class CDeerKingJumpInPlaceCol final : public CMonster
{
protected:
	explicit CDeerKingJumpInPlaceCol(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDeerKingJumpInPlaceCol(const CDeerKingJumpInPlaceCol& rhs);
	virtual ~CDeerKingJumpInPlaceCol() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

private:
	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;
	_bool				m_bEffect = false;

private:
	_bool				m_bPlayerFriendly = false;		// �÷��̾� ����

private:
	HRESULT Update_Collider();

private:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

	HRESULT Calc_Dir();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Collider();

public:
	static CDeerKingJumpInPlaceCol* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END