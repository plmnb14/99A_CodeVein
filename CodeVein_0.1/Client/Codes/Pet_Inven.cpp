#include "stdafx.h"
#include "..\Headers\Pet_Inven.h"
#include "UI_Manager.h"

CPet_Inven::CPet_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CPet_Inven::CPet_Inven(const CPet_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CPet_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPet_Inven::Ready_GameObject(void* pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	SetUp_Default();
	
	Add_Pet(CPet::PET_TYPE::PET_POISONBUTTERFLY);
	Add_Pet(CPet::PET_TYPE::PET_DEERKING);
	Add_Pet(CPet::PET_TYPE::PET_DEERKING);
	Add_Pet(CPet::PET_TYPE::PET_POISONBUTTERFLY);

	SetUp_SlotPos(); // 슬롯 위치 설정
	return S_OK;
}

_int CPet_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	if (m_bIsActive && !m_bIsSubActive)
	{
		SetUp_SubUI_Active(true);
		m_bIsSubActive = true;
	}
	else if (!m_bIsActive && m_bIsSubActive)
	{
		SetUp_SubUI_Active(false);
		m_bIsSubActive = false;

		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(false);
	}

	if(m_bIsActive)
		CUI_Manager::Get_Instance()->Get_MouseUI()->Set_Active(true);

	Click_Inven();

	return NO_EVENT;
}

_int CPet_Inven::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CPet_Inven::Render_GameObject()
{
	if (false == m_bIsActive)
		return S_OK;

	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pBuffer, E_FAIL);

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	m_pShader->Begin_Pass(1);

	m_pBuffer->Render_VIBuffer();

	m_pShader->End_Pass();

	m_pShader->End_Shader();

	return S_OK;
}

void CPet_Inven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	

	if (m_pExitIcon->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bIsActive = false;
		CUI_Manager::Get_Instance()->Get_Total_Inven()->Set_Active(true);
		CUI_Manager::Get_Instance()->Get_StatusUI()->Set_Active(true);
	}

	_uint iTempNum = 0;
	//소환
	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		for (auto& iter : m_vecPetSlot)
		{
			if (true == iter->Pt_InRect())
			{
				Reset_SlotSelect();
				iter->Set_Select(true);

				Check_Call_Pet(iter->Get_Select(), iTempNum, iter->Get_PetType());
			}

			++iTempNum;
		}
	}
	
	iTempNum = 0;

	//해제
	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
	{
		for (auto& iter : m_vecPetSlot)
		{
			if (true == iter->Pt_InRect())
			{
				if (true == iter->Get_Select())
				{
					iter->Set_Select(false);
					Check_Call_Pet(iter->Get_Select(), iTempNum, iter->Get_PetType());
				}
			}

			++iTempNum;
		}
	}
}

void CPet_Inven::Reset_SlotSelect()
{
	for (auto& iter : m_vecPetSlot)
		iter->Set_Select(false);

	return;
}

void CPet_Inven::Check_Call_Pet(_bool _Check_Get_SlotSelect, _uint _Idx, CPet::PET_TYPE _eType)
{
	CGameObject* pTempPet = nullptr;

	auto& PetContainer = g_pManagement->Get_GameObjectList(L"Layer_Pet", SCENE_STAGE);
	//기존 소환물 체크
	for (auto& list_iter : PetContainer)
	{
		//없는 경우 넘어가기
		if (nullptr == list_iter)
			continue;
		//소환된 개체 불러오기
		else if (true == list_iter->Get_Enable())
		{
			pTempPet = list_iter;
			break;
		}
	}
	//소환
	if (true == _Check_Get_SlotSelect)
	{
		//기존 소환물 없는 경우
		if (nullptr == pTempPet)
		{
			//해당 펫 종류가 있는지 확인
			for (auto& list_iter : PetContainer)
			{
				if(nullptr == list_iter)
					continue;
				else if (_eType == static_cast<CPet*>(list_iter)->Get_PetType())
				{
					pTempPet = list_iter;
					break;
				}
			}
			
			//해당 펫을 검색했는데 없다면
			if (nullptr == pTempPet)
			{
				if (CPet::PET_TYPE::PET_POISONBUTTERFLY == _eType)
				{
					pTempPet = g_pManagement->Clone_GameObject_Return(L"Pet_PoisonButterFly", &CPet::PET_STATUS(_eType));
					g_pManagement->Add_GameOject_ToLayer_NoClone(pTempPet, SCENE_STAGE, L"Layer_Pet", nullptr);
					return;
				}
				else if (CPet::PET_TYPE::PET_DEERKING == _eType)
				{
					pTempPet = g_pManagement->Clone_GameObject_Return(L"Pet_DeerKing", &CPet::PET_STATUS(_eType));
					g_pManagement->Add_GameOject_ToLayer_NoClone(pTempPet, SCENE_STAGE, L"Layer_Pet", nullptr);
					return;
				}
			}
			//해당 펫을 찾았다면
			else
			{
				static_cast<CPet*>(pTempPet)->Function_Check_Navi();
				static_cast<CPet*>(pTempPet)->Play_Deformation();
				pTempPet->Set_Enable(true);
				pTempPet = nullptr;
				return;
			}
		}
		//기존 소환물이 있는 경우
		else
		{
			//펫 종류가 같은 경우
			if (_eType == static_cast<CPet*>(pTempPet)->Get_PetType())
				return;
			//펫 종류가 다른 경우
			else
			{
				static_cast<CPet*>(pTempPet)->Set_Summon(true);
				pTempPet->Set_Enable(false);
				pTempPet = nullptr;

				for (auto& list_iter : PetContainer)
				{
					if (nullptr == list_iter)
						continue;
					else if (_eType == static_cast<CPet*>(list_iter)->Get_PetType())
					{
						pTempPet = list_iter;
						break;
					}
				}

				if (nullptr == pTempPet)
				{
					if (CPet::PET_TYPE::PET_POISONBUTTERFLY == _eType)
					{
						pTempPet = g_pManagement->Clone_GameObject_Return(L"Pet_PoisonButterFly", &CPet::PET_STATUS(_eType));
						g_pManagement->Add_GameOject_ToLayer_NoClone(pTempPet, SCENE_STAGE, L"Layer_Pet", nullptr);
						return;
					}
					else if (CPet::PET_TYPE::PET_DEERKING == _eType)
					{
						pTempPet = g_pManagement->Clone_GameObject_Return(L"Pet_DeerKing", &CPet::PET_STATUS(_eType));
						g_pManagement->Add_GameOject_ToLayer_NoClone(pTempPet, SCENE_STAGE, L"Layer_Pet", nullptr);
						return;
					}
				}
				else
				{
					static_cast<CPet*>(pTempPet)->Function_Check_Navi();
					static_cast<CPet*>(pTempPet)->Play_Deformation();
					pTempPet->Set_Enable(true);
					pTempPet = nullptr;
					return;
				}
			}
		}
	}
	//해제
	else
	{
		if (nullptr == pTempPet)
			return;
		else
		{
			static_cast<CPet*>(pTempPet)->Set_Summon(true);
			pTempPet->Set_Enable(false);
			pTempPet = nullptr;
			return;
		}
	}

	return;
}

HRESULT CPet_Inven::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Inven::SetUp_Default()
{
	m_fPosX = 229.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(40.f, 40.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);

	m_pSummonsBtn = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSummonsBtn, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	m_pSummonsBtn->Set_UI_Pos(m_fPosX + 70.f, m_fPosY - 203.f);
	m_pSummonsBtn->Set_UI_Size(40.f, 40.f);
	m_pSummonsBtn->Set_Type(CInventory_Icon::ICON_SUMMONS);

	return S_OK;
}

HRESULT CPet_Inven::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, 2)))
		return E_FAIL;

	return S_OK;
}

void CPet_Inven::SetUp_SlotPos()
{
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecPetSlot)
	{
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 52.f * (iIdx % 5), m_fPosY - 150.f + 52.f * (iIdx / 5));
		vector_iter->Set_ViewZ(m_fViewZ - 0.1f);
		iIdx++;
	}
}

void CPet_Inven::SetUp_SubUI_Active(_bool bIsActive)
{
	_uint iIdx = 0;
	for (auto& vector_iter : m_vecPetSlot)
	{
		vector_iter->Set_Active(bIsActive);
		iIdx++;
	}

	m_pExitIcon->Set_Active(bIsActive);
	m_pSummonsBtn->Set_Active(bIsActive);
}

void CPet_Inven::Add_Pet(CPet::PET_TYPE ePetType)
{
	CPet_Slot* pPetSlot = static_cast<CPet_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PetSlot", nullptr));
	pPetSlot->Set_PetType(ePetType);
	pPetSlot->Set_UI_Size(50.f, 50.f);
	g_pManagement->Add_GameOject_ToLayer_NoClone(pPetSlot, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	m_vecPetSlot.push_back(pPetSlot);

	SetUp_SlotPos();
}

CPet_Inven* CPet_Inven::Create(_Device pGraphic_Device)
{
	CPet_Inven* pInstance = new CPet_Inven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_Inven");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Inven::Clone_GameObject(void * pArg)
{
	CPet_Inven* pInstance = new CPet_Inven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_Inven");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Inven::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);

	CUI::Free();

	return;
}
