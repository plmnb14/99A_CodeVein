#include "stdafx.h"
#include "..\Headers\OKMessageUI.h"


COKMessageUI::COKMessageUI(_Device pDevice)
	: CUI(pDevice)
{
}

COKMessageUI::COKMessageUI(const COKMessageUI & rhs)
	: CUI(rhs)
{
}

HRESULT COKMessageUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT COKMessageUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	return NOERROR;
}

_int COKMessageUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	if (m_bIsActive &&
		1.f > m_fAlpha)
		m_fAlpha += _float(TimeDelta) * 2.f;

	if (!m_bIsActive &&
		0.f < m_fAlpha)
		m_fAlpha -= _float(TimeDelta) * 2.f;

	m_pCollider->Update(m_pTransformCom->Get_Pos());

	return NO_EVENT;
}

_int COKMessageUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT COKMessageUI::Render_GameObject()
{
	if (!m_bIsActive && 0.f >= m_fAlpha)
		return NOERROR;

	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(3);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT COKMessageUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_OKMessageUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
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
	m_pCollider->Set_Radius(_v3{ 0.1f, 0.05f, 0.01f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return NOERROR;
}

HRESULT COKMessageUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, _uint(iIndex))))
		return E_FAIL;

	return NOERROR;
}

COKMessageUI * COKMessageUI::Create(_Device pGraphic_Device)
{
	COKMessageUI* pInstance = new COKMessageUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CGameObject * COKMessageUI::Clone_GameObject(void * pArg)
{
	COKMessageUI* pInstance = new COKMessageUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);
	return pInstance;
}

void COKMessageUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CUI::Free();
}
