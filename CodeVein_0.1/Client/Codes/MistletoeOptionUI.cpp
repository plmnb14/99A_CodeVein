#include "stdafx.h"
#include "..\Headers\MistletoeOptionUI.h"


CMistletoeOptionUI::CMistletoeOptionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CMistletoeOptionUI::CMistletoeOptionUI(const CMistletoeOptionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CMistletoeOptionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CMistletoeOptionUI::Ready_GameObject(void * pArg)
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

_int CMistletoeOptionUI::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return S_OK;

	CUI::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	m_pRendererCom->Add_RenderList(RENDER_3DUI, this);
	
	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	switch (m_eOption)
	{
	case OPTION_STAGE:
		m_iIndex = 1;
		break;
	case OPTION_INHERIT:
		m_iIndex = 2;
		break;
	case OPTION_LEVELUP:
		m_iIndex = 3;
		break;
	}
	return NO_EVENT;
}

_int CMistletoeOptionUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CMistletoeOptionUI::Render_GameObject()
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
			(i == 0) ? (iIndex = 4) : (iIndex = m_iIndex);

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

HRESULT CMistletoeOptionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MistletoeUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
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

HRESULT CMistletoeOptionUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	/*if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iIndex)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	//m_pShaderCom->Set_Texture("g_DepthTexture", g_pManagement->Get_Target_Texture(L"Target_DepthUI"));
	return NOERROR;
}

CMistletoeOptionUI * CMistletoeOptionUI::Create(_Device pGraphic_Device)
{
	CMistletoeOptionUI* pInstance = new CMistletoeOptionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMistletoeOptionUI::Clone_GameObject(void * pArg)
{
	CMistletoeOptionUI* pInstance = new CMistletoeOptionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMistletoeOptionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CUI::Free();
}
