#include "stdafx.h"
#include "..\Headers\Material.h"


CMaterial::CMaterial(_Device pDevice)
	: CGameObject(pDevice)
{
}

CMaterial::CMaterial(const CMaterial & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMaterial::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMaterial::Ready_GameObject(void * pArg)
{
	CGameObject::Ready_GameObject(pArg);

	return NOERROR;
}

_int CMaterial::Update_GameObject(_double TimeDelta)
{
	return _int();
}

CMaterial * CMaterial::Create(_Device pGraphic_Device)
{
	CMaterial* pInstance = new CMaterial(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject * CMaterial::Clone_GameObject(void * pArg)
{
	CMaterial* pInstance = new CMaterial(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

void CMaterial::Free()
{
	CGameObject::Free();
}
