#pragma once

#include "Monster.h"

BEGIN(Client)

class Engine::CEffect;
class CDeerKingRightHandCol final : public CMonster
{
protected:
	explicit CDeerKingRightHandCol(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDeerKingRightHandCol(const CDeerKingRightHandCol& rhs);
	virtual ~CDeerKingRightHandCol() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

private:
	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

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
	static CDeerKingRightHandCol* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END