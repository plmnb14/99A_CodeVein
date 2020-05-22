#include "stdafx.h"
#include "..\Headers\PickUp_ItemUI.h"
#include "UI_Manager.h"

#include "DropItem.h"

CPickUp_ItemUI::CPickUp_ItemUI(_Device Graphic_Device)
	: CUI(Graphic_Device)
{
}

CPickUp_ItemUI::CPickUp_ItemUI(const CPickUp_ItemUI & rhs)
	: CUI(rhs)
{
}

HRESULT CPickUp_ItemUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPickUp_ItemUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fSizeX = 298.f;
	m_fSizeY = 38.f;

	m_fPosX = 1100.f;
	m_fPosY = 200.f;

	return S_OK;
}

_int CPickUp_ItemUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CUI::Update_GameObject(TimeDelta);

	CItem_Manager* pItem_Mgr = CItem_Manager::Get_Instance();

	SetUp_State(TimeDelta);

	//m_iIndex = pItem_Mgr->Get_PickUp_Number();

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	return S_OK;
}

_int CPickUp_ItemUI::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return S_OK;
}

HRESULT CPickUp_ItemUI::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CPickUp_ItemUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pBufferCom)
		return E_FAIL;

	if (false == m_bIsActive)
		return NOERROR;

	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	CItem_Manager* pItemManager = CItem_Manager::Get_Instance();

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(3);

	if (FAILED(SetUp_ConstantTable(m_uiRenderNumber)))
		return E_FAIL;

	m_pShaderCom->Commit_Changes();
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();


	return S_OK;
}

HRESULT CPickUp_ItemUI::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_PickUp_Item", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPickUp_ItemUI::SetUp_ConstantTable(_uint TextureIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_fSparkle", &m_fSparkleBox, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, TextureIndex)))
		return E_FAIL;

	return S_OK;
}

void CPickUp_ItemUI::SetUp_State(_double TimeDelta)
{
	CUI_Manager* pUIManager = CUI_Manager::Get_Instance();
	CDropItem* pDropItem = static_cast<CDropItem*>(g_pManagement->Get_GameObjectBack(L"Layer_Item", SCENE_STAGE));

	if (false == m_bOne_PickupUIEnd)
	{
		m_fPercentage = m_fPickup_Itembar / m_fSizeX;
		m_fPickup_Itembar += 5.f;
	}
	
	if (m_fPickup_Itembar >= m_fSizeX)
	{
		m_fPickup_Itembar = m_fSizeX;
		m_bOne_PickupUIEnd = true;
	}

	if(true == m_bOne_PickupUIEnd)
		m_fTimer += (_float)TimeDelta;

	if (1.45f <= m_fTimer)
	{
		m_bOne_PickupUIEnd = false;
		m_fPickup_Itembar = 0.f;
		if(true == pDropItem->Get_Dead())
			pUIManager->Get_PickUP_ItemUIMgr()->Get_vecPickUp_Item().pop_back(); 
		m_bIsActive = false;
	}
}

void CPickUp_ItemUI::SetUp_Rendering_ItemTextrue()
{
}

CPickUp_ItemUI * CPickUp_ItemUI::Create(_Device pGraphic_Device)
{
	CPickUp_ItemUI* pInstance = new CPickUp_ItemUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CPickUp_ItemUI::Clone_GameObject(void * pArg)
{
	CPickUp_ItemUI* pInstance = new CPickUp_ItemUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CPickUp_ItemUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
