#include "stdafx.h"
#include "..\Headers\Expendables_Inven.h"

#include "Item_Manager.h"
#include "UI_Manager.h"

CExpendables_Inven::CExpendables_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CExpendables_Inven::CExpendables_Inven(const CExpendables_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CExpendables_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CExpendables_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	SetUp_Default();

	// Slot Create
	CUI::UI_DESC* pDesc = nullptr;
	CExpendables_Slot* pExSlot = nullptr;
	for (_uint i = 0; i < 6; ++i)
	{
		for (_uint j = 0; j < 5; ++j)
		{
			pDesc = new CUI::UI_DESC;

			pDesc->fPosX = m_fPosX - 103.f + 52.f * j;
			pDesc->fPosY = m_fPosY - 140.f + 52.f * i;
			pDesc->fSizeX = 50.f;
			pDesc->fSizeY = 50.f;
			
			g_pManagement->Add_GameObject_ToLayer(L"GameObject_ExpendSlot", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
			pExSlot = static_cast<CExpendables_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));

			m_vecSlot.push_back(pExSlot);
			m_vecUI_DESC.push_back(pDesc);
		}
	}

	Add_MultiExpendables(CExpendables::Expend_Blood, 5);
	Add_MultiExpendables(CExpendables::Expend_Cheet, 6);
	Add_MultiExpendables(CExpendables::Expend_Hp, 7);
	SetUp_SlotPos();
	return NOERROR;
}

_int CExpendables_Inven::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(false);

		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;
	}

	if (!m_bIsActive)
		return NO_EVENT;
	else
		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(true);

	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);
	
	Click_Inven();

	

	
	return NO_EVENT;
}

_int CExpendables_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExpendables_Inven::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(1);

	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CExpendables_Inven::Add_Component()
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

HRESULT CExpendables_Inven::SetUp_ConstantTable()
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


void CExpendables_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	_uint iIdx = 0;
	for (auto& pExpendSlot : m_vecSlot)
	{
		if (pExpendSlot->Pt_InRect() && pExpendSlot->Get_Type() != CExpendables::EXPEND_END)
		{
			m_pExplainUI->Set_Type(CExpendables::EXPEND_TYPE(pExpendSlot->Get_Type()));
			m_pExplainUI->Set_CurHaveCnt(pExpendSlot->Get_Size());

			if (pExpendSlot->Get_Type() == CExpendables::Expend_Hp)
				m_pExplainUI->Set_MaximumCnt(m_iMaximumCnt);
			else
				m_pExplainUI->Set_MaximumCnt(5);

			if (!pExpendSlot->Get_Select() && pExpendSlot->Get_Size() > 0 && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				if (m_vecQuickSlot.size() == 8)
					return;
				pExpendSlot->Set_Select(true);			
				m_vecQuickSlot.push_back(pExpendSlot);
				
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
			}
			else if(pExpendSlot->Get_Select() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
			{
				pExpendSlot->Set_Select(false);				
				Delete_QuickSlot(pExpendSlot);

				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
			}
		}
		iIdx++;
	}
}

void CExpendables_Inven::SetUp_Default()
{
	m_pExplainUI = static_cast<CExplainExpendUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExplainExpendUI", nullptr));
	m_pExplainUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.5f);
	m_pExplainUI->Set_UI_Size(1024.f * 0.6f, 720.f * 0.6f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExplainUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	
}

void CExpendables_Inven::Add_Slot()
{
	CExpendables_Slot* pSlot = static_cast<CExpendables_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendSlot", nullptr));
	pSlot->Set_UI_Size(50.f, 50.f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_vecSlot.push_back(pSlot);
	SetUp_SlotPos();
}

void CExpendables_Inven::Add_Expendables(CExpendables::EXPEND_TYPE eType)
{
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_Expendables", SCENE_MORTAL, L"Layer_Expendables");
	CExpendables* pExpendables = static_cast<CExpendables*>(g_pManagement->Get_GameObjectBack(L"Layer_Expendables", SCENE_MORTAL));
	if (nullptr == pExpendables)
	{
		MSG_BOX("Expendables is null");
		return;
	}
	pExpendables->Set_Type(eType);
	
	Load_Expendables(pExpendables, 0);
	SetUp_SlotPos();
}

void CExpendables_Inven::Add_MultiExpendables(CExpendables::EXPEND_TYPE eType, _uint iCnt)
{
	for (_uint i = 0; i < iCnt; ++i)
		Add_Expendables(eType);

	SetUp_SlotPos();
}

void CExpendables_Inven::Delete_QuickSlot(CExpendables_Slot* pSlot)
{
	for (_uint i = 0; i < m_vecQuickSlot.size(); ++i)
	{
		if (m_vecQuickSlot[i]->Get_Type() == pSlot->Get_Type() &&
			m_vecQuickSlot[i]->Get_Size() == pSlot->Get_Size())
		{
			m_vecQuickSlot.erase(m_vecQuickSlot.begin() + i);
			break;
		}	
	}
	SetUp_SlotPos();
	return;
}



void CExpendables_Inven::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

void CExpendables_Inven::SetUp_SubUI_Active(_bool bIsActive)
{
	for (auto& pSlot : m_vecSlot)
	{
		pSlot->Set_Active(bIsActive);
	}

	m_pExplainUI->Set_Active(bIsActive);
}

void CExpendables_Inven::Use_Expendableas(CExpendables_Slot * pSlot)
{
	if (nullptr == pSlot)
		return;
	if (pSlot->Get_Size() == 0 ||
		m_vecQuickSlot.size() == 0)
		return;

	pSlot->Delete_Item();

	for (_uint i = 0; i < m_vecQuickSlot.size(); ++i)
	{

		if (m_vecQuickSlot[i] == pSlot &&
			m_vecQuickSlot[i]->Get_Size() == 0)
		{
			m_vecQuickSlot.erase(m_vecQuickSlot.begin() + i);
			m_vecQuickSlot.shrink_to_fit();
			break;
		}
	}

	_ulong idx = 0;
	for (auto& pExSlot : m_vecSlot)
	{
		if (pExSlot->Get_Size() == 0)
		{
			m_vecSlot.erase(m_vecSlot.begin() + idx);
			m_vecSlot.shrink_to_fit();
			Add_Slot();
			break;
		}
		
		++idx;
	}
	SetUp_SlotPos();
}

void CExpendables_Inven::Sell_Item(CExpendables_Slot * pSlot)
{
	if (nullptr == pSlot || pSlot->Get_Size() <= 0)
		return;
	
	pSlot->Delete_Item();

	if (pSlot->Get_Size() <= 0)
	{
		for (_uint i = 0; i < m_vecSlot.size(); ++i)
		{
			if (pSlot == m_vecSlot[i])
			{
				pSlot->Set_Dead();
				m_vecSlot.erase(m_vecSlot.begin() + i);
				m_vecSlot.shrink_to_fit();
			}
		}
	}

	SetUp_SlotPos();
}

void CExpendables_Inven::Sell_Expendables(_uint iDelete)
{
	_uint idx = 0;
	for (auto& pSlot : m_vecSlot)
	{
		if (pSlot->Pt_InRect())
		{
			if (iDelete > pSlot->Get_Size())
				return;
			for (_uint i = 0; i < iDelete; ++i)
			{
				pSlot->Delete_Item();
			}
					
			if (pSlot->Get_Size() == 0)
			{
				m_vecSlot.erase(m_vecSlot.begin() + idx);
				m_vecSlot.shrink_to_fit();
				Add_Slot();
				break;
			}
		}
		
		++idx;
	}

	_ulong Idx = 0;
	for (auto& pSlot : m_vecSlot)
	{

		pSlot->Set_UI_Pos(m_vecUI_DESC[Idx]->fPosX, m_vecUI_DESC[Idx]->fPosY);
		++Idx;
	}
}


void CExpendables_Inven::Load_Expendables(CExpendables* pExpendables, _uint iIndex)
{
	if (nullptr == pExpendables)
		return;
	if (m_vecSlot.size() <= iIndex)
		return;

	if (pExpendables->Get_Type() == CExpendables::Expend_Hp) // 체력물약->최대치 만큼만 슬롯에 담김
	{
		if ((m_vecSlot[iIndex]->Get_Type() == pExpendables->Get_Type() ||
			m_vecSlot[iIndex]->Get_Size() == 0) && m_vecSlot[iIndex]->Get_Size() < m_iMaximumCnt)
			m_vecSlot[iIndex]->Input_Item(pExpendables);
		else
			Load_Expendables(pExpendables, iIndex + 1);
	}
	else // 체력물약 이외의 아이템은 5개씩 담김
	{
		if ((m_vecSlot[iIndex]->Get_Type() == pExpendables->Get_Type() ||
			m_vecSlot[iIndex]->Get_Size() == 0) && m_vecSlot[iIndex]->Get_Size() < 5)
			m_vecSlot[iIndex]->Input_Item(pExpendables);
		else
			Load_Expendables(pExpendables, iIndex + 1);
	}
}

CExpendables_Inven * CExpendables_Inven::Create(_Device pGraphic_Device)
{
	CExpendables_Inven* pInstance = new CExpendables_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExpendables_Inven::Clone_GameObject(void * pArg)
{
	CExpendables_Inven* pInstance = new CExpendables_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExpendables_Inven::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
