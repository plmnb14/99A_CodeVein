#include "stdafx.h"
#include "..\Headers\ExpendSellCollectionUI.h"
#include "UI_Manager.h"

CExpendSellCollectionUI::CExpendSellCollectionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExpendSellCollectionUI::CExpendSellCollectionUI(const CExpendSellCollectionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExpendSellCollectionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CExpendSellCollectionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	SetUp_Default();

	SetUp_SlotPos();
	return NOERROR;
}

_int CExpendSellCollectionUI::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		
		SetUp_SlotPos();
		SetUp_SubUI_Active(true);

		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
	
	}
	if (!m_bIsActive)
		return NO_EVENT;

	CUI::Update_GameObject(TimeDelta);

	m_vecSlot = *CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_VecExpendSlot();
	Click_SubUI();
	SetUp_SlotPos();

	return NO_EVENT;
}

_int CExpendSellCollectionUI::Late_Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return NO_EVENT;
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CExpendSellCollectionUI::Render_GameObject()
{
	
	return NOERROR;
}

HRESULT CExpendSellCollectionUI::Add_Component()
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

HRESULT CExpendSellCollectionUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CExpendSellCollectionUI::SetUp_SubUI_Active(_bool bIsActive)
{
	for (auto& iter : m_vecSlot)
	{
		iter->Set_Active(bIsActive);
	}
}

void CExpendSellCollectionUI::SetUp_SlotPos()
{
	m_vecSlot = *CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_VecExpendSlot();
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

void CExpendSellCollectionUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;
	
	for (auto& iter : m_vecSlot)
	{
		if (iter->Pt_InRect() && !iter->Get_Select())
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);

				m_pExpendSellUI->Set_SellSlot(iter);
				m_pExpendSellUI->Set_Active(true);
			}
		}
	}
	
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_UI_Size(50.f, 50.f);
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

void CExpendSellCollectionUI::SetUp_Default()
{
	m_pExpendSellUI = static_cast<CExpendSellUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendSellUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpendSellUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}
//
//void CExpendSellCollectionUI::Sell_ExpendItem(CExpendables_Slot * pSlot)
//{
//	if (nullptr == pSlot)
//		return;
//
//	
//
//	if (pSlot->Get_Size() == 0)
//	{		
//		
//
//		_uint iIdx = 0;
//		for (auto& iter : m_vecSlot)
//		{
//			if (iter == pSlot)
//			{
//				pSlot->Set_Dead();
//				m_vecSlot.erase(m_vecSlot.begin() + iIdx);
//				m_vecSlot.shrink_to_fit();
//			}
//			
//			iIdx++;
//		}
//	}
//	else
//		pSlot->Delete_Item();
//		
//
//	SetUp_SlotPos();
//}

CExpendSellCollectionUI * CExpendSellCollectionUI::Create(_Device pGraphic_Device)
{
	CExpendSellCollectionUI* pInstance = new CExpendSellCollectionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("GeneralSellUI Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CExpendSellCollectionUI::Clone_GameObject(void * pArg)
{
	CExpendSellCollectionUI* pInstance = new CExpendSellCollectionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("GeneralSellUI Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpendSellCollectionUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
