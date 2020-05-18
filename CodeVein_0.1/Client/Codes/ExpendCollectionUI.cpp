#include "stdafx.h"
#include "..\Headers\ExpendCollectionUI.h"
#include "UI_Manager.h"

CExpendCollectionUI::CExpendCollectionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CExpendCollectionUI::CExpendCollectionUI(const CExpendCollectionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CExpendCollectionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CExpendCollectionUI::Ready_GameObject(void * pArg)
{
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	
	return NOERROR;
}

_int CExpendCollectionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	Update_SubUI();
	Click_SubUI();
	
	return NO_EVENT;
}

void CExpendCollectionUI::SetUp_Default()
{
	CExpendOptionUI* pOption = nullptr;

	LOOP(6)
	{
		pOption = static_cast<CExpendOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendOptionUI", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		pOption->Set_UI_Size(50.f, 50.f);
		m_vecOption.push_back(pOption);
	}

	m_vecOption[0]->Set_Type(CExpendables::Expend_MaximumUp);
	m_vecOption[1]->Set_Type(CExpendables::Expend_Hp);
	m_vecOption[2]->Set_Type(CExpendables::Expend_Return);
	m_vecOption[3]->Set_Type(CExpendables::Expend_Blood);
	m_vecOption[4]->Set_Type(CExpendables::Expend_Cheet);
	m_vecOption[5]->Set_Type(CExpendables::Expend_SuperArmor);

	m_pInfoUI = static_cast<CExpend_InfoUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Expend_InfoUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pInfoUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pBuyUI = static_cast<CExpendBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendBuyUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBuyUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CExpendCollectionUI::Update_SubUI()
{
	_uint iIdx = 0;
	for (auto& iter : m_vecOption)
	{
		iter->Set_UI_Pos(90.f + 52.f * (iIdx % 5), 200.f + 52.f * (iIdx / 5));
		iter->Set_Active(m_bIsActive);
		iIdx++;
	}

	if (m_pInfoUI)
		m_pInfoUI->Set_Active(m_bIsActive);
}

void CExpendCollectionUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;

	_uint iIdx = 0;
	for (auto& iter : m_vecOption)
	{
		if (iter->Pt_InRect() && CExpendables::EXPEND_END != iter->Get_Type())
		{			
			m_pInfoUI->Set_Type(iter->Get_Type());
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{	
				m_pBuyUI->Set_Active(true);
				m_pBuyUI->Set_Type(iter->Get_Type());

				Click_ExpendSlot_Sound(iIdx);
			}
		}
		iIdx++;
	}
}

void CExpendCollectionUI::Click_ExpendSlot_Sound(_uint iIdx)
{
	if (iIdx > 5)
		return;
	_uint iChannel = CSoundManager::CHANNELID::Purchase_GeneralShop_ExpendSlot01 + iIdx;

	g_pSoundManager->Play_Sound(L"UI_CommonClick.wav", CSoundManager::CHANNELID(iChannel), CSoundManager::Ambient_Sound);
}


CExpendCollectionUI * CExpendCollectionUI::Create(_Device pGraphic_Device)
{
	CExpendCollectionUI* pInstance = new CExpendCollectionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CExpendCollectionUI::Clone_GameObject(void * pArg)
{
	CExpendCollectionUI* pInstance = new CExpendCollectionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CExpendCollectionUI::Free()
{
	CUI::Free();
}
