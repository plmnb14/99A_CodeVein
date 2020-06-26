#pragma once

#include "Monster_Bullet.h"

BEGIN(Client)

class CHunterBullet final : public CMonster_Bullet
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
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

private:
	HRESULT Add_Component();
	HRESULT Ready_Collider();

public:
	static CHunterBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CEffect*			m_pEffect1 = nullptr;
	CEffect*			m_pEffect2 = nullptr;
	CEffect*			m_pEffect3 = nullptr;

};

END
