#include "stdafx.h"
#include "..\Headers\MaterialCollectionUI.h"
#include "UI_Manager.h"

CMaterialCollectionUI::CMaterialCollectionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CMaterialCollectionUI::CMaterialCollectionUI(const CMaterialCollectionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CMaterialCollectionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMaterialCollectionUI::Ready_GameObject(void * pArg)
{
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return NOERROR;
}

_int CMaterialCollectionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	Update_SubUI();
	Click_SubUI();

	return NO_EVENT;
}

void CMaterialCollectionUI::SetUp_Default()
{
	// 아이템 옵션 슬롯 생성(퀸 스틸, 퀸 티탄, 퀸 텅스텐
	CMaterialOptionUI* pOption = nullptr;
	LOOP(3)
	{
		pOption = static_cast<CMaterialOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialOptionUI", nullptr));
		pOption->Set_UI_Size(50.f, 50.f);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecOption.push_back(pOption);
	}
	m_vecOption[0]->Set_Type(CMaterial::Queen_Steel);
	m_vecOption[1]->Set_Type(CMaterial::Queen_Titanium);
	m_vecOption[2]->Set_Type(CMaterial::Queen_Tungsten);

	// 아이템 설명 UI 생성
	m_pInfoUI = static_cast<CMaterial_InfoUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Material_InfoUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pInfoUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pInfoUI->Set_Type(CMaterial::MATERIAL_TYPE::Queen_Steel);

	// 아이템 구매 UI 생성
	m_pBuyUI = static_cast<CMaterialBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialBuyUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBuyUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CMaterialCollectionUI::Update_SubUI()
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

void CMaterialCollectionUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;

	_uint iIdx = 0;
	for (auto& iter : m_vecOption)
	{
		if (iter->Pt_InRect() && iter->Get_Type() != CMaterial::MATERIAL_END)
		{	
			if (m_pSoundOption != iter)
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_CommonHover.wav", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
			m_pSoundOption = iter;

			m_pInfoUI->Set_Type(iter->Get_Type());
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				m_pBuyUI->Set_Active(true);
				m_pBuyUI->Set_Type(iter->Get_Type());

				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_CommonClick.wav", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
			}
		}
		iIdx++;
	}
}


CMaterialCollectionUI * CMaterialCollectionUI::Create(_Device pGraphic_Device)
{
	CMaterialCollectionUI* pInstance = new CMaterialCollectionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMaterialCollectionUI::Clone_GameObject(void * pArg)
{
	CMaterialCollectionUI* pInstance = new CMaterialCollectionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMaterialCollectionUI::Free()
{
	CUI::Free();
}
