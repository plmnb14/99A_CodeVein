#include "stdafx.h"
#include "..\Headers\Consume_Item.h"


CConsume_Item::CConsume_Item(_Device pDevice)
	: CItem(pDevice)
{
}

CConsume_Item::CConsume_Item(const CConsume_Item & rhs)
	: CItem(rhs)
{
}

HRESULT CConsume_Item::Ready_GameObject_Prototype()
{
	CItem::Ready_GameObject_Prototype();

	m_eType = CItem::CONSUME;

	return NOERROR;
}

HRESULT CConsume_Item::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CItem::Ready_GameObject(pArg);

	m_eType = CItem::CONSUME;

	return NOERROR;
}

_int CConsume_Item::Update_GameObject(_double TimeDelta)
{
	CItem::Update_GameObject(TimeDelta);

	return _int();
}

_int CConsume_Item::Late_Update_GameObject(_double TimeDelta)
{
	
	return _int();
}

HRESULT CConsume_Item::Render_GameObject()
{
	return NOERROR;
}

HRESULT CConsume_Item::Add_Component()
{
	return NOERROR;
}

HRESULT CConsume_Item::SetUp_ConstantTable()
{
	return NOERROR;
}

CConsume_Item * CConsume_Item::Create(_Device pGraphic_Device)
{
	CConsume_Item* pInstance = new CConsume_Item(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Consume Item Created Filed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CConsume_Item::Clone_GameObject(void * pArg)
{
	CConsume_Item* pInstance = new CConsume_Item(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Consume Item Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CConsume_Item::Free()
{
	CGameObject::Free();
}
