#include "stdafx.h"
#include "..\Headers\ExpendCollectionUI.h"


CExpendCollectionUI::CExpendCollectionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExpendCollectionUI::CExpendCollectionUI(const CExpendCollectionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExpendCollectionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExpendCollectionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	return NOERROR;
}

_int CExpendCollectionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	
	Update_SubUI();
	Click_SubUI();

	return NO_EVENT;
}

_int CExpendCollectionUI::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CExpendCollectionUI::Render_GameObject()
{
	
	return NOERROR;
}

HRESULT CExpendCollectionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	//// For.Com_Renderer
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	//// For.Com_Texture
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MaterialShopUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	//// For.Com_Shader
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	//// for.Com_VIBuffer
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CExpendCollectionUI::SetUp_ConstantTable(_uint iIndex)
{
	return NOERROR;
}

void CExpendCollectionUI::SetUp_Default()
{
}

void CExpendCollectionUI::Update_SubUI()
{
}

void CExpendCollectionUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;
}

CExpendCollectionUI * CExpendCollectionUI::Create(_Device pGraphic_Device)
{
	CExpendCollectionUI* pInstance = new CExpendCollectionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExpendCollectionUI::Clone_GameObject(void * pArg)
{
	CExpendCollectionUI* pInstance = new CExpendCollectionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExpendCollectionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
