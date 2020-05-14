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
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;

	SetUp_Default();

	return NOERROR;
}

_int CMaterialCollectionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	
	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	Update_SubUI();
	Click_SubUI();
	return NO_EVENT;
}

_int CMaterialCollectionUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMaterialCollectionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
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

	for (auto& iter : m_vecOption)
	{
		if (iter->Pt_InRect() && iter->Get_Type() != CMaterial::MATERIAL_END)
		{
			m_pInfoUI->Set_Type(iter->Get_Type());

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				Buy_Material(iter);			
			}
			
		}
	}
}

void CMaterialCollectionUI::Buy_Material(CMaterialOptionUI* pOption)
{
	switch (pOption->Get_Type())
	{
	case CMaterial::Queen_Steel:
	{
		m_iCost = 100;
	}
		break;
	case CMaterial::Queen_Titanium:
	{
		m_iCost = 200;

	}
		break;
	case CMaterial::Queen_Tungsten:
	{
		m_iCost = 300;
	}
		break;
	}

	_uint iTotalCost = m_iCost * m_iBuyCnt; // 총 비용 = 개당 가격 * 사는 개수

	_uint iHazeCnt = CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt();

	// 가지고 있는 헤이즈가 구매 총 비용 이상일 경우 -> 돈 충분함 -> 구매한다.
	if (iHazeCnt >= iTotalCost)
	{
		// 구매해서 재료 인벤에 넣는다.
		CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(pOption->Get_Type(), m_iBuyCnt);
		// 헤이즈를 탕진한 만큼 감소시킨다
		CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(-iTotalCost);
	}
	// 가지고 있는 헤이즈보다 구매 총 비용이 더 클 경우 -> 돈 부족함 -> 구매X
	else
	{
		cout << "비용 부족!" << endl;
		return;
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
	Safe_Release(m_pTransformCom);

	CUI::Free();
}
