#include "stdafx.h"
#include "..\Headers\GeneralStoreOption.h"

CGeneralStoreOption::CGeneralStoreOption(_Device pDevice)
	: CUI(pDevice)
{
}

CGeneralStoreOption::CGeneralStoreOption(const CGeneralStoreOption & rhs)
	: CUI(rhs)
{
}

HRESULT CGeneralStoreOption::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CGeneralStoreOption::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_bIsActive = false;

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.07f, 0.1f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return NOERROR;
}

_int CGeneralStoreOption::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	if (!m_bIsActive)
		return NOERROR;

	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	switch (m_eOptionType)
	{
	case OPTION_PURCHASE:
		m_iIndex = 2;
		break;
	case OPTION_BYE:
		m_iIndex = 3;
		break;
	}

	return NO_EVENT;
}

_int CGeneralStoreOption::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CGeneralStoreOption::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	_uint iIndex = 0;
	if (!m_bIsSelect)
	{
		iIndex = m_iIndex;
		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);

		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	else
	{
		LOOP(2)
		{
			(i == 0) ? (iIndex = 1) : (iIndex = m_iIndex);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(1);

			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}
	

	return NOERROR;
}

HRESULT CGeneralStoreOption::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_GeneralStoreNPCUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGeneralStoreOption::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	return NOERROR;
}

CGeneralStoreOption * CGeneralStoreOption::Create(_Device pGraphic_Device)
{
	CGeneralStoreOption* pInstance = new CGeneralStoreOption(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGeneralStoreOption::Clone_GameObject(void * pArg)
{
	CGeneralStoreOption* pInstance = new CGeneralStoreOption(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGeneralStoreOption::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CUI::Free();
}
