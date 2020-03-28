#include "stdafx.h"
#include "..\Headers\Tab_Consume.h"

#include "Item_Manager.h"
#include "Item_Slot.h"
#include "Button_UI.h"
#include "Inventory.h"
#include "QuickSlot.h"

CTab_Consume::CTab_Consume(_Device pDevice)
	: CUI(pDevice)
{
}

CTab_Consume::CTab_Consume(const CTab_Consume & rhs)
	: CUI(rhs)
{
}

CItem_Slot * CTab_Consume::Get_Slot(_uint iInx)
{
	return m_vecRegist[iInx];
}

HRESULT CTab_Consume::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CTab_Consume::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	/*m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 500.f;
	m_fSizeY = 600.f;*/

	// 슬롯 생성
	CUI::UI_DESC* pDesc = nullptr;
	for (_uint i = 0; i < 5; ++i)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX - 130.f;
		pDesc->fPosY = m_fPosY + 100.f * i - 150.f;
		pDesc->fSizeX = 100.f;
		pDesc->fSizeY = 100.f;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_ItemSlot", SCENE_STAGE, L"Layer_ItemSlot", pDesc);
		

		CItem_Slot* pSlot = static_cast<CItem_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_ItemSlot", SCENE_STAGE));
		m_vecSlot.push_back(pSlot);
	}

	// 퀵슬롯 등록 슬롯 생성
	
	for (_uint i = 0; i < 3; ++i)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX + 130.f;
		pDesc->fPosY = m_fPosY + 100.f * i - 100.f;
		pDesc->fSizeX = 100.f;
		pDesc->fSizeY = 100.f;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_ItemSlot", SCENE_STAGE, L"Layer_QuickSlotRegist", pDesc);


		CItem_Slot* pSlot = static_cast<CItem_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlotRegist", SCENE_STAGE));
		m_vecRegist.push_back(pSlot);
	}

	// 버튼 생성
	LOOP(2)
	{
		pDesc = new CUI::UI_DESC;
		pDesc->fPosX = m_fPosX;
		pDesc->fPosY = m_fPosY -50.f + 150.f * i;
		pDesc->fSizeX = 50.f;
		pDesc->fSizeY = 50.f;
		pDesc->iIndex = 2 - i;
		g_pManagement->Add_GameObject_ToLayer(L"GameObject_ButtonUI", SCENE_STAGE, L"Layer_ButtonUI", pDesc);

		CButton_UI* pBtn = static_cast<CButton_UI*>(g_pManagement->Get_GameObjectBack(L"Layer_ButtonUI", SCENE_STAGE));
		m_vecButton.push_back(pBtn);
	}
	
	return NOERROR;
}

_int CTab_Consume::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	/*if (g_pInput_Device->Get_DIKeyState(DIK_SUBTRACT))
		Add_Item(CItem::CONS_HP, 0);
		
	else if (g_pInput_Device->Get_DIKeyState(DIK_H))
		Add_Item(CItem::CONS_SP, 0);*/
		

	//cout << m_vecSlot[0]->Get_SlotSize() << ", " << m_vecSlot[1]->Get_SlotSize() << endl;

	/*if(g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		Regist_Item(0);
	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
		Delete_Item();
	cout << m_vecRegist[0]->Get_SlotSize() << endl;*/
	//cout << m_vecRegist[1]->Get_SlotSize() << endl;

	
	
	return NO_EVENT;
}

_int CTab_Consume::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	
	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 0.1f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;
	
	Compute_ViewZ_UI(0.91f);

	return NO_EVENT;
}

HRESULT CTab_Consume::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	
	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	m_matOldView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matOldProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(0);

	// 버퍼를 렌더링한다.
	// (인덱스버퍼(012023)에 보관하고있는 인덱스를 가진 정점을 그리낟.)
	// 삼각형 두개를 그리낟.각각의 삼각형마다 정점세개, 각각의 정점을 버텍스 셰이더의 인자로 던진다.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matOldView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matOldProj);

	

	return NOERROR;
}

HRESULT CTab_Consume::Add_Component()
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

HRESULT CTab_Consume::SetUp_ConstantTable()
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

void CTab_Consume::Add_Item(CItem::ITEM_ID eID, _uint iIndex)
{
	if (m_vecSlot[iIndex]->Get_ItemID() == CItem::ID_NONE ||
		m_vecSlot[iIndex]->Get_SlotSize() == 0 ||
		m_vecSlot[iIndex]->Get_ItemID() == eID)
	{
		CItem::ITEM_INFO* pInfo = new CItem::ITEM_INFO;
		pInfo->eID = eID;
		pInfo->eType = CItem::CONSUME;

		g_pManagement->Add_GameObject_ToLayer(L"GameObject_Item", SCENE_STAGE, L"Layer_Item", pInfo);


		CItem* pItem = static_cast<CItem*>(g_pManagement->Get_GameObjectBack(L"Layer_Item", SCENE_STAGE));
		m_vecSlot[iIndex]->Add_Item_ToSlot(pItem);
	}
	else
	{
		Add_Item(eID, iIndex + 1);
	}

}

void CTab_Consume::Delete_Item()
{
	//CItem::ITEM_ID eID = CItem::ID_NONE;

	if (m_vecRegist.size() == 0)
		return;

	for (auto& pSlot : m_vecRegist)
	{
		if (pSlot->Coll_Mouse() && pSlot->Get_SlotSize() > 0 && pSlot->Get_ItemID() != CItem::ID_NONE)
		{
			//eID = pSlot->Get_ItemID();
			for(;pSlot->Get_SlotSize() > 0 ;)
				pSlot->Delete_Item_ToSlot();
		}
	}

	//CItem_Manager::Get_Instance()->Delete_Item(eID);
}

void CTab_Consume::Regist_Item(_uint iIndex)
{
	for (auto& pSlot : m_vecSlot)
	{
		if (pSlot->Coll_Mouse())
		{
			if (pSlot->Get_ItemID() != CItem::ID_NONE ||
				pSlot->Get_SlotSize() > 0)
			{
				if (pSlot->Get_ItemID() == m_vecRegist[iIndex]->Get_ItemID() ||
					m_vecRegist[iIndex]->Get_SlotSize() == 0)
				{
					while (pSlot->Get_SlotSize() != m_vecRegist[iIndex]->Get_SlotSize())
					{
						m_vecRegist[iIndex]->Add_Item_ToSlot(pSlot->Get_SlotBack());

						
						static_cast<CQuickSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_QuickSlot", SCENE_STAGE))->Add_Item(m_vecRegist[iIndex]->Get_SlotBack());
					}
				}
				else
					Regist_Item(iIndex + 1);
			}
		}

	}
	
}



CTab_Consume * CTab_Consume::Create(_Device pGraphic_Device)
{
	CTab_Consume* pInstance = new CTab_Consume(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Tab_Consume Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTab_Consume::Clone_GameObject(void * pArg)
{
	CTab_Consume* pInstance = new CTab_Consume(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Tab_Consume Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTab_Consume::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	for_each(m_vecSlot.begin(), m_vecSlot.end(), Safe_Release<CItem_Slot*>);
	//for_each(m_vecRegist.begin(), m_vecRegist.end(), Safe_Release<CItem_Slot*>);
	for_each(m_vecButton.begin(), m_vecButton.end(), Safe_Release<CButton_UI*>);

	CUI::Free();
}
