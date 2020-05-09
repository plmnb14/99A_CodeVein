#include "stdafx.h"
#include "..\Headers\QuickSlot.h"

#include "Expendables_Inven.h"
#include "Button_UI.h"
#include "NumberUI.h"
#include "UI_Manager.h"

CQuickSlot::CQuickSlot(_Device pDevice)
	: CUI(pDevice)
{

}

CQuickSlot::CQuickSlot(const CQuickSlot & rhs)
	: CUI(rhs)
{
	
}

HRESULT CQuickSlot::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CQuickSlot::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	CUI::Ready_GameObject(pArg);

	m_fPosX = 120.f;
	m_fPosY = 500.f;
	m_fSizeX = 50.f;
	m_fSizeY = 50.f;
	m_bIsActive = true;

	SetUp_Default();

	return NOERROR;
}

_int CQuickSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_vecQuickSlot.size() > m_iSelect)
	{
		m_iIndex = m_vecQuickSlot[m_iSelect]->Get_Type();
		m_pNumberUI->Set_UI_Index(m_vecQuickSlot[m_iSelect]->Get_NumberUI()->Get_UI_Index());
	}
		
	else
	{
		m_iIndex = CExpendables::EXPEND_END;
		m_pNumberUI->Set_UI_Index(0);
	}
	
	// 소비템 창에서 실시간으로 퀵슬롯 등록 정보를 얻어와서 벡터에 저장	
	m_vecQuickSlot = *CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_QuickSlot();
	
	// ADD: 퀵슬롯 ->  방향으로 
	if (g_pInput_Device->Key_Up(DIK_ADD))
	{
		if (m_iSelect >= m_vecQuickSlot.size() - 1)
			m_iSelect = 0;
		else
			++m_iSelect;

	}
	// SUBTRACT: 퀵슬롯 <- 방향으로
	if (g_pInput_Device->Key_Up(DIK_SUBTRACT))
	{
		if (m_iSelect > 0)
			--m_iSelect;
		else
			m_iSelect = _uint(m_vecQuickSlot.size()) - 1;
	}

	LOOP(2)
		m_vecDecoUI[i]->Set_Active(m_bIsActive);
	m_pNumberUI->Set_Active(m_bIsActive);

	return NO_EVENT;
}

_int CQuickSlot::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CQuickSlot::Render_GameObject()
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

HRESULT CQuickSlot::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_QuickSlotUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CQuickSlot::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, m_iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CQuickSlot::SetUp_Default()
{
	CUI::UI_DESC* pDesc = nullptr;

	// 화살표
	//==============================================
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX - 30.f;
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = 20.f;
	pDesc->fSizeY = 20.f;
	pDesc->iIndex = 5;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ButtonUI", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	m_vecDecoUI.push_back(static_cast<CButton_UI*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL)));

	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX + 30.f;
	pDesc->fPosY = m_fPosY;
	pDesc->fSizeX = 20.f;
	pDesc->fSizeY = 20.f;
	pDesc->iIndex = 6;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_ButtonUI", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	m_vecDecoUI.push_back(static_cast<CButton_UI*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL)));
	//==============================================

	// 카운트 UI
	pDesc = new CUI::UI_DESC;
	pDesc->fPosX = m_fPosX;
	pDesc->fPosY = m_fPosY + 30.f;
	pDesc->fSizeX = 20.f;
	pDesc->fSizeY = 20.f;
	pDesc->iIndex = 0;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_NumberUI", SCENE_MORTAL, L"Layer_PlayerUI", pDesc);
	m_pNumberUI = static_cast<CNumberUI*>(g_pManagement->Get_GameObjectBack(L"Layer_PlayerUI", SCENE_MORTAL));
	m_pNumberUI->Set_Active(true);
}

CExpendables::EXPEND_TYPE CQuickSlot::Use_Item()
{
	_uint iType = 0;
	if (m_vecQuickSlot.size() <= 0)
		return CExpendables::EXPEND_END;
	if (nullptr == m_vecQuickSlot[m_iSelect])
		return CExpendables::EXPEND_END;
	if (m_vecQuickSlot[m_iSelect]->Get_Size() == 0)
		return CExpendables::EXPEND_END;

	
	iType = m_vecQuickSlot[m_iSelect]->Get_Type();
	
	CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Use_Expendableas(m_vecQuickSlot[m_iSelect]);

	return CExpendables::EXPEND_TYPE(iType);
}



CQuickSlot * CQuickSlot::Create(_Device pGraphic_Device)
{
	CQuickSlot* pInstance = new CQuickSlot(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CQuickSlot Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuickSlot::Clone_GameObject(void * pArg)
{
	CQuickSlot* pInstance = new CQuickSlot(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CQuickSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuickSlot::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}

