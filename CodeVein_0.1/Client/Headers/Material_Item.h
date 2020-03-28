#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CMaterial_Item final : public CItem
{
private:
	explicit CMaterial_Item(_Device pDevice);
	explicit CMaterial_Item(const CMaterial_Item& rhs);
	virtual ~CMaterial_Item() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	

public:
	static CMaterial_Item*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END