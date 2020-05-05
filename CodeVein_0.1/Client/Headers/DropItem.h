#pragma once

#include "Item.h"

BEGIN(Client)

class CDropItem :public CItem
{
protected:
	explicit CDropItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDropItem(const CDropItem& rhs);
	virtual ~CDropItem() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

private:
	virtual void Check_Dist();

protected:
	virtual HRESULT Add_Component(void* _pArg) override;
	virtual HRESULT SetUp_ConstantTable() override;
	virtual HRESULT Ready_Status(void * _pArg) override;
	virtual HRESULT Ready_Collider() override;

public:
	static CDropItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void * pArg) override;
	virtual void Free();
 
private:
	_float m_fEffectLimitTime = 0.f;
};

END