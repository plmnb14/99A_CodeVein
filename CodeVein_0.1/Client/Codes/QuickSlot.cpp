#include "stdafx.h"
#include "..\Headers\QuickSlot.h"
#include "UI_Manager.h"
#include "Player.h"

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
	m_fSizeX = 80.f;
	m_fSizeY = 80.f;
	m_bIsActive = true;

	SetUp_Default();

	return NOERROR;
}

_int CQuickSlot::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_vecQuickSlot.size() <= m_iSelect)
	{
		m_eType = CExpendables::EXPEND_END;
		m_pItemCntFont->Update_NumberValue(0.f);
		m_iIndex = 7;
	}
	else if(m_vecQuickSlot.size() > m_iSelect)
	{
		m_eType = CExpendables::EXPEND_TYPE(m_vecQuickSlot[m_iSelect]->Get_Type());
		m_pItemCntFont->Update_NumberValue(_float(m_vecQuickSlot[m_iSelect]->Get_Size()));
	}
		
	

	switch (m_eType)
	{
	case CExpendables::Expend_MaximumUp:
		m_iIndex = 1;
		break;
	case CExpendables::Expend_Hp:
		m_iIndex = 2;
		break;
	case CExpendables::Expend_Return:
		m_iIndex = 3;
		break;
	case CExpendables::Expend_Blood:
		m_iIndex = 4;
		break;
	case CExpendables::Expend_Cheet:
		m_iIndex = 5;
		break;
	case CExpendables::Expend_SuperArmor:
		m_iIndex = 6;
		break;
	case CExpendables::EXPEND_END:
		m_iIndex = 7;
		break;
	}
	
	// 소비템 창에서 실시간으로 퀵슬롯 등록 정보를 얻어와서 벡터에 저장	
	m_vecQuickSlot = *CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Get_QuickSlot();
	
	Move_QuickSlot();

	m_pItemCntFont->Set_Active(m_bIsActive);

	if (g_pInput_Device->Key_Up(DIK_DOWN))
		Use_QuickSlot_Item();
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

	_uint iIndex = 0;

	LOOP(2)
	{
		if (0 == i)
			iIndex = 0;
		else if (1 == i)
			iIndex = m_iIndex;

		if (FAILED(SetUp_ConstantTable(iIndex)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	

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

HRESULT CQuickSlot::SetUp_ConstantTable(_uint iIndex)
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

void CQuickSlot::SetUp_Default()
{
	m_pItemCntFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	m_pItemCntFont->Set_UI_Pos(m_fPosX, m_fPosY + m_fSizeY * 0.4f);
	m_pItemCntFont->Set_UI_Size(10.4f, 20.f);
	m_pItemCntFont->Set_ViewZ(m_fViewZ - 0.1f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pItemCntFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CQuickSlot::Move_QuickSlot()
{
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
}
//
//CExpendables::EXPEND_TYPE CQuickSlot::Use_Item()
//{
//	_uint iType = 0;
//	if (m_vecQuickSlot.size() <= 0)
//		return CExpendables::EXPEND_END;
//	if (nullptr == m_vecQuickSlot[m_iSelect])
//		return CExpendables::EXPEND_END;
//	if (m_vecQuickSlot[m_iSelect]->Get_Size() == 0)
//		return CExpendables::EXPEND_END;
//
//	
//	iType = m_vecQuickSlot[m_iSelect]->Get_Type();
//	
//	CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Use_Expendableas(m_vecQuickSlot[m_iSelect]);
//
//	return CExpendables::EXPEND_TYPE(iType);
//}

void CQuickSlot::Use_QuickSlot_Item()
{
	if (m_vecQuickSlot.size() <= m_iSelect)
		return;
	if (m_vecQuickSlot.size() <= 0 ||
		m_vecQuickSlot[m_iSelect]->Get_Size() == 0 ||
		m_vecQuickSlot[m_iSelect]->Get_Type() == CExpendables::EXPEND_END)
		return;
	
	CPlayer* pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	OBJECT_PARAM tPlayerParam = pPlayer->Get_Target_Param();
	CExpendables_Inven* pExpendInven = CUI_Manager::Get_Instance()->Get_Expendables_Inven();

	g_pSoundManager->Play_Sound(L"UI_QuickSlotUse.wav", CSoundManager::CHANNELID::QuickSlot_Use_Item, CSoundManager::Ambient_Sound);

	switch (m_vecQuickSlot[m_iSelect]->Get_Type())
	{
	case CExpendables::Expend_MaximumUp:
	{		
		pExpendInven->Use_Expendableas(m_vecQuickSlot[m_iSelect]);
		pExpendInven->Set_MaximumItemCnt(pExpendInven->Get_MaximumItemCnt() + 1);
		
	}
		break;
	case CExpendables::Expend_Hp:
	{
		// HP 증가
		pExpendInven->Use_Expendableas(m_vecQuickSlot[m_iSelect]);
		pPlayer->Add_Target_Hp(300.f);
	}
		break;
	case CExpendables::Expend_Return:
	{
		// 귀환 아이템
	}
		break;
	case CExpendables::Expend_Cheet:
	{
		// 치트 물약
		pExpendInven->Use_Expendableas(m_vecQuickSlot[m_iSelect]);
		pPlayer->Add_Target_Hp(300.f);
		pPlayer->Add_Target_Mana(_short(50));
	}
		break;
	case CExpendables::Expend_Blood:
	{
		// 연혈 증가
		pExpendInven->Use_Expendableas(m_vecQuickSlot[m_iSelect]);
		pPlayer->Add_Target_Mana(_short(20));
	}
		break;
	case CExpendables::Expend_SuperArmor:
	{
		// 슈퍼아머(무적상태)
		pPlayer->Set_Target_SuperArmor(true);
	}
		break;
	case CExpendables::EXPEND_END:
		return;
	}
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

