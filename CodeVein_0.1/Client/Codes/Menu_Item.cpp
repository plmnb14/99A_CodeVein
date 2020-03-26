#include "stdafx.h"
#include "..\Headers\Menu_Item.h"

CMenu_Item::CMenu_Item(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CMenu_Item::CMenu_Item(const CMenu_Item & rhs)
	: CUI(rhs)
{
}

HRESULT CMenu_Item::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMenu_Item::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = WINCX * 0.5f;
	m_fSizeY = WINCY * 0.7f;

	Add_Slot(5);
	
	return NOERROR;
}

_int CMenu_Item::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	SetUp_WindowPos();

	
	Click_ItemSlot();
	

	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	

	Safe_Release(pManagement);
	
	return NO_EVENT;
}

_int CMenu_Item::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMenu_Item::Render_GameObject()
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

	// 버퍼를 렌더링한다.
	// (인덱스버퍼(012023)에 보관하고있는 인덱스를 가진 정점을 그리낟.)
	// 삼각형 두개를 그리낟.각각의 삼각형마다 정점세개, 각각의 정점을 버텍스 셰이더의 인자로 던진다.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();


	pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CMenu_Item::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMenu_Item::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 2)))
		return E_FAIL;

	return NOERROR;
}

void CMenu_Item::Click_ItemSlot()
{
	if (CInput_Device::Get_Instance()->Get_DIMouseState(CInput_Device::DIM_RB))
	{
		LOOP(_int(m_iSlotCnt))
		{
			if (CInput_Device::Get_Instance()->MousePt_InRect(m_vecItemSlot[i]->Get_UI_Pos().x, m_vecItemSlot[i]->Get_UI_Pos().y,
				m_vecItemSlot[i]->Get_UI_Size().x, m_vecItemSlot[i]->Get_UI_Size().y, g_hWnd))
			{
				CManagement* pManagement = CManagement::Get_Instance();
				if (nullptr == pManagement)
					return;
				Safe_AddRef(pManagement);

				CMenuBaseUI* pBase = static_cast<CMenuBaseUI*>(pManagement->Get_GameObjectBack(L"Layer_MenuBase", SCENE_STAGE));

				if (m_vecItemSlot[i]->Get_SlotSize() > 0)
				{
					pBase->Add_Item(m_vecItemSlot[i]->Get_SlotItemType(), 0);
					if (false == pBase->Get_Full())
						m_vecItemSlot[i]->Pop_Item();
					else
						MSG_BOX("더이상 넣을 수 없습니다.");
				}
				

				Safe_Release(pManagement);
			}
		}
	}
	
}

void CMenu_Item::SetUp_WindowPos()
{
	if (true == m_bIsOpenWindow)
	{
		m_fPosX = WINCX * 0.5f;
		m_fPosY = WINCY * 0.5f;
	}
	else
	{
		m_fPosX = -WINCX * 0.5f;
		m_fPosY = WINCY * 0.5f;
	}

	LOOP(_int(m_iSlotCnt))
	{
		m_vecItemSlot[i]->Set_UI_Pos(m_fPosX - 50.f, m_fPosY - 60.f + i * 50.f);
		m_vecCntUI[i]->Set_UI_Pos(m_vecItemSlot[i]->Get_UI_Pos().x - 10.f, m_vecItemSlot[i]->Get_UI_Pos().y + 10.f);
		m_vecCntUI[i]->Set_Count(m_vecItemSlot[i]->Get_SlotSize());
	}

	
}

void CMenu_Item::Add_Slot(_uint iSlotCnt)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;

	Safe_AddRef(pManagement);

	m_iSlotCnt += iSlotCnt;

	for (_uint i = 0; i < iSlotCnt; ++i)
	{
		pManagement->Add_GameObject_ToLayer(L"GameObject_ItemSlot", SCENE_STAGE, L"Layer_ItemSlot");
		CItemSlot* pSlot = static_cast<CItemSlot*>(pManagement->Get_GameObjectBack(L"Layer_ItemSlot", SCENE_STAGE));
		
		pSlot->Set_UI_Size(50.f, 50.f);
		m_vecItemSlot.push_back(pSlot);

		pManagement->Add_GameObject_ToLayer(L"GameObject_CntSlotUI", SCENE_STAGE, L"Layer_MenuItemCnt");
		CSlotCnt_UI* pCntUI = static_cast<CSlotCnt_UI*>(pManagement->Get_GameObjectBack(L"Layer_MenuItemCnt", SCENE_STAGE));
		pCntUI->Set_UI_Size(20.f, 20.f);
		m_vecCntUI.push_back(pCntUI);
	}
	

	Safe_Release(pManagement);
}

void CMenu_Item::Add_Item(CItem::ITEM_TYPE eType, _uint iSlotIndex)
{
	
	
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return;
	Safe_AddRef(pManagement);

	if (iSlotIndex > m_iSlotCnt - 1)
	{
		Safe_Release(pManagement);
		return;
	}

	if (m_vecItemSlot.size() == 0)
		Add_Slot(1);

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

CMenu_Item * CMenu_Item::Create(_Device pGraphic_Device)
{
	CMenu_Item* pInstance = new CMenu_Item(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CMenu_Item Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenu_Item::Clone_GameObject(void * pArg)
{
	CMenu_Item* pInstance = new CMenu_Item(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMenu_Item");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu_Item::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
