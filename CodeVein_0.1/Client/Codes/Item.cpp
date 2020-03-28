#include "stdafx.h"
#include "..\Headers\Item.h"

CItem::CItem(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CItem::CItem(const CItem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CItem::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CItem::Ready_GameObject(void * pArg)
{
	CGameObject::Ready_GameObject(pArg);

	if (nullptr != pArg)
	{
		m_pInfo = static_cast<ITEM_INFO*>(pArg);

		m_eType = m_pInfo->eType;
		m_eID = m_pInfo->eID;
	}
	else
	{
		m_eType = TYPE_END;
		m_eID = ID_NONE;
	}

	return NOERROR;
}

_int CItem::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	return NO_EVENT;
}

CItem * CItem::Create(_Device pGraphic_Device)
{
	CItem* pInstance = new CItem(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CItem Item Created Filed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItem::Clone_GameObject(void * pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("CItem Item Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	Safe_Delete(m_pInfo);
	CGameObject::Free();
}
