#include "stdafx.h"
#include "..\Headers\Expendables.h"



CExpendables::CExpendables(_Device pDevice)
	: CGameObject(pDevice)
{
}

CExpendables::CExpendables(const CExpendables & rhs)
	: CGameObject(rhs)
{
}

HRESULT CExpendables::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExpendables::Ready_GameObject(void * pArg)
{
	CGameObject::Ready_GameObject(pArg);

	return NOERROR;
}

_int CExpendables::Update_GameObject(_double TimeDelta)
{
	return _int();
}

CExpendables * CExpendables::Create(_Device pGraphic_Device)
{
	CExpendables* pInstance = new CExpendables(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExpendables::Clone_GameObject(void * pArg)
{
	CExpendables* pInstance = new CExpendables(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExpendables::Free()
{
	CGameObject::Free();
}
