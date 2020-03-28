#include "stdafx.h"
#include "..\Headers\Material_Item.h"

CMaterial_Item::CMaterial_Item(_Device pDevice)
	: CItem(pDevice)
{
}

CMaterial_Item::CMaterial_Item(const CMaterial_Item & rhs)
	: CItem(rhs)
{
}

HRESULT CMaterial_Item::Ready_GameObject_Prototype()
{
	CItem::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CMaterial_Item::Ready_GameObject(void * pArg)
{
	CItem::Ready_GameObject(pArg);

	m_eType = CItem::MATERIAL;
	
	return NOERROR;
}

_int CMaterial_Item::Update_GameObject(_double TimeDelta)
{
	CItem::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	return NO_EVENT;
}

CMaterial_Item * CMaterial_Item::Create(_Device pGraphic_Device)
{
	CMaterial_Item* pInstance = new CMaterial_Item(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CMaterial_Item Item Created Filed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMaterial_Item::Clone_GameObject(void * pArg)
{
	CMaterial_Item* pInstance = new CMaterial_Item(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CMaterial_Item Item Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterial_Item::Free()
{
	CItem::Free();
}
