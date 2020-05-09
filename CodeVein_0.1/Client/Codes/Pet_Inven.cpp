#include "stdafx.h"
#include "..\Headers\Pet_Inven.h"

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

HRESULT CPet_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return S_OK;
}

_int CPet_Inven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.0f);

	_uint iIdx = 0;
	for (auto& vector_iter : m_vecPetSlot)
	{
		cout << m_vecPetSlot.size() << endl;
		vector_iter->Set_UI_Pos(m_fPosX - 100.f + 50.f * (iIdx % 5), m_fPosY - 150.f + 50.f * (iIdx / 5));
		vector_iter->Set_Active(m_bIsActive);
		iIdx++;
	}

	m_pExitIcon->Set_Active(m_bIsActive);

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

void CPet_Inven::Add_Pet(ITEM_GRADE_TYPE _eGrade, CPet::PET_TYPE _eType)
{
	CUI::UI_DESC* pDesc = nullptr;
	CPet_Slot* pSlot = nullptr;

	pDesc = new CUI::UI_DESC;

	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 50.f;
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_PetSlot", SCENE_MORTAL, L"Layer_PetUI", pDesc);
	pSlot = static_cast<CPet_Slot*>(g_pManagement->Get_GameObjectBack(L"Layer_PetUI", SCENE_MORTAL));
	pSlot->Set_Grade(_eGrade); //펫 등급설정
	pSlot->Set_Type(_eType); //펫 종류 설정
	m_vecPetSlot.push_back(pSlot);

	// 슬롯 생성시 위치 조정
	for (_uint i = 0; i < m_vecPetSlot.size(); ++i)
	{
		m_vecPetSlot[i]->Set_Active(m_bIsActive);
		m_vecPetSlot[i]->Set_ViewZ(m_fViewZ - 0.1f);
		m_vecPetSlot[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 130.f + 52.f * (i / 5));
	}

	return;
}

void CPet_Inven::Sell_Pet(_uint iDelete)
{
	return;
}

void CPet_Inven::Active_Pet(CPet_Slot * pSlot)
{
	return;
}

void CPet_Inven::Click_Inven()
{
	//비활성화시 작용x
	if (false == m_bIsActive)
		return;

	//슬롯 탐색
	for (auto& pSlot : m_vecPetSlot)
	{
		//슬롯과 충돌했다면
		if (true == pSlot->Pt_InRect())
		{
			//왼쪽버튼 누름, 등록
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB) && !pSlot->Get_Select())
				Regist_PetSlot(pSlot);
			//오른쪽버튼 누름, 해제
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB) && pSlot->Get_Select())
				UnRegist_PetSlot(pSlot);
		}
	}

	return;
}

void CPet_Inven::Regist_PetSlot(CPet_Slot* _pPetSlot)
{
	//종류 없음, 등록 취소
	if (_pPetSlot->Get_Type() == CPet::PET_TYPE::PET_TYPE_END)
		return;

	//대상 타입이 없을경우
	if (m_eType == CPet::PET_TYPE::PET_TYPE_END)
	{
		m_eType = (CPet::PET_TYPE)_pPetSlot->Get_Type();
		_pPetSlot->Set_Select(true);
	}

	return;
}

void CPet_Inven::UnRegist_PetSlot(CPet_Slot* _pPetSlot)
{
	//종류 없음, 해제 취소
	if (_pPetSlot->Get_Type() == CPet::PET_TYPE::PET_TYPE_END)
		return;

	//대상 찾음
	if (_pPetSlot->Get_Type() == m_eType)
	{
		m_eType = CPet::PET_TYPE::PET_TYPE_END;
		_pPetSlot->Set_Select(false);
	}

	return;
}

HRESULT CPet_Inven::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MenuWindow", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Inven::SetUp_Default()
{
	m_fPosX = WINCX * 0.3f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;
	m_bIsActive = false;

	m_pExitIcon = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExitIcon, SCENE_MORTAL, L"Layer_PetUI", nullptr);
	m_pExitIcon->Set_UI_Pos(m_fPosX + 120.f, m_fPosY - 203.f);
	m_pExitIcon->Set_UI_Size(35.f, 45.f);
	m_pExitIcon->Set_Type(CInventory_Icon::ICON_EXIT);

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
