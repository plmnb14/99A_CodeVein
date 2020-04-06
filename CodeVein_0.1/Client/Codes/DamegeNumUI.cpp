#include "stdafx.h"
#include "..\Headers\DamegeNumUI.h"

CDamegeNumUI::CDamegeNumUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CDamegeNumUI::CDamegeNumUI(const CDamegeNumUI & rhs)
	: CUI(rhs)
{
}

HRESULT CDamegeNumUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CDamegeNumUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);


	return S_OK;
}

_int CDamegeNumUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	SetUp_State(TimeDelta);

	_mat matBill, matWorld, matView;
	matWorld = m_pTransformCom->Get_WorldMat();

	matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, NULL, &matBill);

	Compute_ViewZ(&m_pTransformCom->Get_Pos());

	m_pTransformCom->Set_WorldMat((matBill * matWorld));


	m_fPosX = m_pTransformCom->Get_Pos().x;
	m_fPosY = m_pTransformCom->Get_Pos().y;

	matWorld._41 = m_fPosX - WINCX * 0.5f;
	matWorld._42 = -m_fPosY + WINCY * 0.5f;

	// 데미지가 들어왔을 때만 랜더해야 함
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	return S_OK;
}

_int CDamegeNumUI::Late_Update_GameObject(_double TimeDelta)
{
	return S_OK;
}

HRESULT CDamegeNumUI::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CDamegeNumUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(0);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CDamegeNumUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_DamegeNum", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamegeNumUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &g_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &g_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_mat))))
		return E_FAIL;

	return S_OK;
}

void CDamegeNumUI::SetUp_State(_double TimeDelta)
{
}

CDamegeNumUI* CDamegeNumUI::Create(_Device pGraphic_Device, CGameObject * MonsterTarget)
{
	CDamegeNumUI* pInstance = new CDamegeNumUI(pGraphic_Device);

	pInstance->m_pTarget = MonsterTarget;

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CDamegeNumUI::Clone_GameObject(void * pArg)
{
	CDamegeNumUI* pInstance = new CDamegeNumUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDamegeNumUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
