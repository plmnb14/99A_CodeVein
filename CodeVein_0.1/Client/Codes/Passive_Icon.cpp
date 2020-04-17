#include "stdafx.h"
#include "..\Headers\Passive_Icon.h"


CPassive_Icon::CPassive_Icon(_Device pDevice)
	: CUI(pDevice)
{
}

CPassive_Icon::CPassive_Icon(const CPassive_Icon & rhs)
	: CUI(rhs)
{
}

HRESULT CPassive_Icon::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CPassive_Icon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	return NOERROR;
}

_int CPassive_Icon::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	return _int();
}

_int CPassive_Icon::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CPassive_Icon::Render_GameObject()
{
	return E_NOTIMPL;
}

HRESULT CPassive_Icon::Add_Component()
{
	return E_NOTIMPL;
}

HRESULT CPassive_Icon::SetUp_ConstantTable()
{
	return E_NOTIMPL;
}

CPassive_Icon * CPassive_Icon::Create(_Device pGraphic_Device)
{
	CPassive_Icon* pInstance = new CPassive_Icon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CPassive_Icon::Clone_GameObject(void * pArg)
{
	CPassive_Icon* pInstance = new CPassive_Icon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CPassive_Icon::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
