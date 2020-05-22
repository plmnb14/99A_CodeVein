#include "stdafx.h"
#include "..\Headers\BloodCodeSelectUI.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CBloodCodeSelectUI::CBloodCodeSelectUI(_Device pDevice)
	: CUI(pDevice)
{
	
}

CBloodCodeSelectUI::CBloodCodeSelectUI(const CBloodCodeSelectUI & rhs)
	: CUI(rhs)
{
	
}

HRESULT CBloodCodeSelectUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CBloodCodeSelectUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	
	return NOERROR;
}

_int CBloodCodeSelectUI::Update_GameObject(_double TimeDelta)
{
	if (m_bIsActive && !m_bIsSubActive)
	{
		for (auto& iter : m_vecBloodCodeSlot)
			iter->Set_Active(true);

		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(false);

		for (auto& iter : m_vecBloodCodeSlot)
			iter->Set_Active(false);

		m_bIsSubActive = false;
	}

	if (!m_bIsActive)
		return NO_EVENT;
	else
		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(true);

	CUI::Update_GameObject(TimeDelta);

	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	_v3 vLookX = m_pTransformCom->Get_Axis(AXIS_X);
	_v3 vLookY = m_pTransformCom->Get_Axis(AXIS_Y);
	_v3 vLookZ = m_pTransformCom->Get_Axis(AXIS_Z);

	_uint idx = 0;
	for (auto& iter : m_vecBloodCodeSlot)
	{
		TARGET_TO_TRANS(iter)->Set_Pos(m_pTransformCom->Get_Pos() + *V3_NORMAL_SELF(&vLookX) * -0.25f + *V3_NORMAL_SELF(&vLookY) * 0.3f
		+ *V3_NORMAL_SELF(&vLookX) * _float(idx % 5) * 0.33f + *V3_NORMAL_SELF(&vLookY) * -0.3f * _float(idx / 5) + *V3_NORMAL_SELF(&vLookZ) * -0.01f);
		TARGET_TO_TRANS(iter)->Set_Angle(m_pTransformCom->Get_Angle());
		iter->Set_Active(m_bIsActive);
	
		++idx;
	}

	Click_BloodCodeSlot();
	
	return NO_EVENT;
}

HRESULT CBloodCodeSelectUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	
	return NOERROR;
}

HRESULT CBloodCodeSelectUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;
	
	return NOERROR;
}

void CBloodCodeSelectUI::Click_BloodCodeSlot()
{
	if (!m_bIsActive)
		return;
	
	for (auto& iter : m_vecBloodCodeSlot)
	{
		if (CCollisionMgr::Collision_Ray(iter, g_pInput_Device->Get_Ray(), &m_fCross))
		{
			iter->Set_Select(true);
			m_eID = iter->Get_CodeID();
			CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_CodeID(m_eID);
			
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB)/* && iter->Get_Release()*/)
			{
				// 블러드 코드 선택 시, 연혈 릴리즈 창 활성화 & 블러드 코드 선택 창 비활성화
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"Slot_Regist.ogg", CSoundManager::CHANNELID::UI_Click, CSoundManager::Effect_Sound);
				CUI_Manager::Get_Instance()->Stop_Play_UISound(L"UI_Open2.ogg", CSoundManager::CHANNELID::UI_SFX_01, CSoundManager::Effect_Sound);

				CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_PlayerBloodCodeType(m_eID);
				CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_IsChoise(true);	
				CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_Alpha(0.2f);
				CUI_Manager::Get_Instance()->Get_BloodCode_Owner()->Set_CodeID(m_eID);
				CUI_Manager::Get_Instance()->Get_Skill_ReleaseUI()->Set_CodeID(m_eID);

				
			}
		}
		else
			iter->Set_Select(false);
	}
}

void CBloodCodeSelectUI::Release_BloodCode(BloodCode_ID eBloodCodeID)
{
	switch (eBloodCodeID)
	{
	case BloodCode_Fighter:
	{
		m_vecBloodCodeSlot[0]->Set_Release(true);
	}
		return;
	case BloodCode_Prometheus:
	{
		m_vecBloodCodeSlot[1]->Set_Release(true);
	}
		return;
	case BloodCode_Artemis:
	{
		m_vecBloodCodeSlot[2]->Set_Release(true);
	}
		return;
	case BloodCode_DarkKnight:
	{
		m_vecBloodCodeSlot[3]->Set_Release(true);
	}
		return;
	case BloodCode_Queen:
	{
		m_vecBloodCodeSlot[4]->Set_Release(true);
	}
		return;
	}
}

void CBloodCodeSelectUI::SetUp_Default()
{
	// 블러드코드 슬롯 생성
	CBloodCodeSlot* pInstance = nullptr;

	LOOP(5)
	{
		pInstance = static_cast<CBloodCodeSlot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BloodCodeSlot", nullptr));
		
		TARGET_TO_TRANS(pInstance)->Set_Scale(_v3(0.3f, 0.223f, 1.f));

		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecBloodCodeSlot.push_back(pInstance);
	}

	m_vecBloodCodeSlot[0]->Set_CodeID(BloodCode_Fighter);
	m_vecBloodCodeSlot[1]->Set_CodeID(BloodCode_Prometheus);
	m_vecBloodCodeSlot[2]->Set_CodeID(BloodCode_Artemis);
	m_vecBloodCodeSlot[3]->Set_CodeID(BloodCode_DarkKnight);
	m_vecBloodCodeSlot[4]->Set_CodeID(BloodCode_Queen);

	m_vecBloodCodeSlot[0]->Set_Release(true);
}

CBloodCodeSelectUI * CBloodCodeSelectUI::Create(_Device pGraphic_Device)
{
	CBloodCodeSelectUI* pInstance = new CBloodCodeSelectUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBloodCodeSelectUI::Clone_GameObject(void * pArg)
{
	CBloodCodeSelectUI* pInstance = new CBloodCodeSelectUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBloodCodeSelectUI::Free()
{
	Safe_Release(m_pTransformCom);

	CUI::Free();
}
