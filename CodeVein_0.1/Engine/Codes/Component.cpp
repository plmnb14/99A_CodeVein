#include "..\Headers\Component.h"

CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Dev(pGraphic_Device)
	, m_bisClone(false)
{
	Safe_AddRef(m_pGraphic_Dev);
}


CComponent::CComponent(const CComponent & rhs)
	: m_pGraphic_Dev(rhs.m_pGraphic_Dev)
	, m_bisClone(true)
{
	Safe_AddRef(m_pGraphic_Dev);
}

_int CComponent::Update_Component()
{
	return _int();
}

HRESULT CComponent::Ready_Component_Prototype()
{
	return NOERROR;
}

HRESULT CComponent::Ready_Component(void * pArg)
{
	return NOERROR;
}

void CComponent::Free()
{
	Safe_Release(m_pGraphic_Dev);
}
