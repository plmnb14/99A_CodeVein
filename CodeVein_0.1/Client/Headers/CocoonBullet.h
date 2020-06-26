#pragma once

#include "Monster_Bullet.h"

BEGIN(Client)

class CCocoonBullet final : public CMonster_Bullet
{
protected:
	explicit CCocoonBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCocoonBullet(const CCocoonBullet& rhs);
	virtual ~CCocoonBullet() = default;

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
	static CCocoonBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

};

END


