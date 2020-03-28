#pragma once

#include "Client_Defines.h"
#include "Item.h"

BEGIN(Client)

class CConsume_Item final : public CItem
{
private:
	explicit CConsume_Item(_Device pDevice);
	explicit CConsume_Item(const CConsume_Item& rhs);
	virtual ~CConsume_Item() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

public:
	static CConsume_Item*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END