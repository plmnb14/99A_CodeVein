#include "stdafx.h"
#include "..\Headers\Armor.h"


CArmor::CArmor(_Device pDevice)
	: CGameObject(pDevice)
{
}

CArmor::CArmor(const CArmor & rhs)
	: CGameObject(rhs)
{
}

HRESULT CArmor::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CArmor::Ready_GameObject(void * pArg)
{
	CGameObject::Ready_GameObject(pArg);

	return NOERROR;
}

_int CArmor::Update_GameObject(_double TimeDelta)
{
	return _int();
}

CArmor * CArmor::Create(_Device pGraphic_Device)
{
	CArmor* pInstance = new CArmor(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CArmor::Clone_GameObject(void * pArg)
{
	CArmor* pInstance = new CArmor(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CArmor::Free()
{
	CGameObject::Free();
}
