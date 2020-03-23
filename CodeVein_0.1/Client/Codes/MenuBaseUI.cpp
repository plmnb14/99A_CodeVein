#include "stdafx.h"
#include "..\Headers\MenuBaseUI.h"



CMenuBaseUI::CMenuBaseUI(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CMenuBaseUI::CMenuBaseUI(const CMenuBaseUI & rhs)
	: CUI(rhs)
{
}

_uint CMenuBaseUI::Get_SlotSize(_uint iSlotIndex)
{
	return m_vecItemSlot[iSlotIndex]->Get_SlotSize();
}

CItem::ITEM_TYPE CMenuBaseUI::Get_SlotItemType(_uint iSlotIndex)
{
	
	return m_vecItemSlot[iSlotIndex]->Get_SlotItemType();
}



HRESULT CMenuBaseUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMenuBaseUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	//m_vecMenuIcon.reserve(9);
	//m_vecItemSlot.reserve(8);

	m_fPosX = WINCX - 300.f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 648;
	m_fSizeY = 648;

	Add_Slot(5);
	Add_CntUI(5);
	Add_MenuIcon(3);

	return NOERROR;
}

_int CMenuBaseUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	

	SetUp_WindowPosition();
	
	for(_uint i = 0; i < m_iCountUICnt; ++i)
	{
		m_vecCntUI[i]->Set_Count(m_vecItemSlot[i]->Get_SlotSize());
	}



	

	return NO_EVENT;
}

_int CMenuBaseUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMenuBaseUI::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);


	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();


	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CMenuBaseUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STAGE, L"Texture_Window", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMenuBaseUI::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}


void CMenuBaseUI::SetUp_WindowPosition()
{
	if (true == m_bIsOpenWindow)
	{
		m_fPosX = WINCX - 300.f;
		m_fPosY = WINCY * 0.5f;	
	}
	else
	{
		m_fPosX = -WINCX * 0.5f;
		m_fPosY = -WINCY * 0.5f;	
	}

	for(_uint i = 0; i < m_iSlotCnt; ++i)
	{
		m_vecItemSlot[i]->Set_UI_Pos(m_fPosX - 126.f + /*m_vecItemSlot[i]->Get_UI_Size().x**/ i * 40.f, m_fPosY + 205.f);
	}

	for (_uint i = 0; i < m_iCountUICnt; ++i)
	{
		m_vecCntUI[i]->Set_UI_Pos(m_vecItemSlot[i]->Get_UI_Pos().x, m_vecItemSlot[i]->Get_UI_Pos().y + 30.f);
	}
	
	for (_uint i = 0; i < m_iMenuIconCnt; ++i)
	{
		m_vecMenuIcon[i]->Set_UI_Pos(m_fPosX - 200.f + (m_vecMenuIcon[i]->Get_UI_Size().x + 10.f) * i, m_fPosY - 230.f);
	}

	
}



void CMenuBaseUI::Add_Slot(_uint iSlotCnt)
{
	m_iSlotCnt = iSlotCnt;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	for(_uint i = 0; i < iSlotCnt; ++i)
	{
		CUI::UI_DESC* pSlotDesc = new CUI::UI_DESC;
		
		pSlotDesc->fSizeX = 46.f;
		pSlotDesc->fSizeY = 46.f;
		pManagement->Add_GameObject_ToLayer(L"GameObject_ItemSlot", SCENE_STAGE, L"Layer_ItemSlot", pSlotDesc);

		CItemSlot* pItemSlot = static_cast<CItemSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemSlot", SCENE_STAGE));

		m_vecItemSlot.push_back(pItemSlot);
	}
	
	Safe_Release(pManagement);
}

void CMenuBaseUI::Add_CntUI(_uint iCountUICnt)
{
	m_iCountUICnt = iCountUICnt;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	for (_uint i = 0; i < iCountUICnt; ++i)
	{
		CUI::UI_DESC* pCntDesc = new CUI::UI_DESC;
		pCntDesc->fPosX = m_fPosX;
		pCntDesc->fPosY = m_fPosY;
		pCntDesc->fSizeX = 20.f;
		pCntDesc->fSizeY = 20.f;

		pManagement->Add_GameObject_ToLayer(L"GameObject_CntSlotUI", SCENE_STAGE, L"Layer_CntUI", pCntDesc);

		CSlotCnt_UI* pCntUI = static_cast<CSlotCnt_UI*>(pManagement->Get_GameObjectBack(L"Layer_CntUI", SCENE_STAGE));

		m_vecCntUI.push_back(pCntUI);
	}

	Safe_Release(pManagement);
}

void CMenuBaseUI::Add_MenuIcon(_uint iMenuIconCnt)
{
	m_iMenuIconCnt = iMenuIconCnt;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	for (_uint i = 0; i < iMenuIconCnt; ++i)
	{
		CUI::UI_DESC* pDesc = new CUI::UI_DESC;
		pDesc->fPosX = 500.f;
		pDesc->fPosY = 600.f;
		pDesc->fSizeX = 50.f;
		pDesc->fSizeY = 50.f;

		pManagement->Add_GameObject_ToLayer(L"GameObject_MenuIcon", SCENE_STAGE, L"Layer_MenuIcon", pDesc);

		CMenuIcon* pIcon = static_cast<CMenuIcon*>(pManagement->Get_GameObjectBack(L"Layer_MenuIcon", SCENE_STAGE));
		pIcon->Set_MenuType(CMenuIcon::MENU_TYPE(i));
		m_vecMenuIcon.push_back(pIcon);
	}

	Safe_Release(pManagement);
}

void CMenuBaseUI::Add_Item(CItem::ITEM_TYPE eType, _uint iSlotIndex)
{
	if (CItem::ITEM_NONE == eType)
		return;

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	if (iSlotIndex > m_iSlotCnt - 1)
	{
		m_bIsFull = true;
		Safe_Release(pManagement);
		return;
	}
	else
		m_bIsFull = false;

	if (((0 < m_vecItemSlot[iSlotIndex]->Get_SlotSize()) &&
		(eType != m_vecItemSlot[iSlotIndex]->Get_SlotItemType())) ||
		(m_vecItemSlot[iSlotIndex]->Get_SlotSize() >= 3))
	{
		Add_Item(eType, iSlotIndex + 1);
	}
	else
		m_vecItemSlot[iSlotIndex]->Add_Item(eType);

	Safe_Release(pManagement);
}

void CMenuBaseUI::Delete_Item(_uint iSlotIndex)
{
	if (0 < m_vecItemSlot[iSlotIndex]->Get_SlotSize())
		m_vecItemSlot[iSlotIndex]->Pop_Item();
	else
		MSG_BOX("해당 슬롯에 등록된 아이템이 없습니다.");
}

void CMenuBaseUI::Add_Manu_Slot(CItemSlot * pSlot)
{
	m_iSlotCnt++;
	m_vecItemSlot.push_back(pSlot);
}

CMenuBaseUI * CMenuBaseUI::Create(_Device pGraphic_Device)
{
	CMenuBaseUI* pInstance = new CMenuBaseUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CMenuBaseUI Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuBaseUI::Clone_GameObject(void * pArg)
{
	CMenuBaseUI* pInstance = new CMenuBaseUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMenuBaseUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenuBaseUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
