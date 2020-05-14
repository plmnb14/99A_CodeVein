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
	if (FAILED(Add_Component()))
		return E_FAIL;
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

_int CExpendCollectionUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CExpendCollectionUI::Render_GameObject()
{
	
	return NOERROR;
}

HRESULT CExpendCollectionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	//// For.Com_Renderer
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
	//	return E_FAIL;

	//// For.Com_Texture
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_MaterialShopUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	//// For.Com_Shader
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	//// for.Com_VIBuffer
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CExpendCollectionUI::SetUp_ConstantTable(_uint iIndex)
{
	return NOERROR;
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

	m_pBuyUI = static_cast<CGeneralStoreBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_GeneralStoreBuyUI", nullptr));
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

	for (auto& iter : m_vecOption)
	{
		if (iter->Pt_InRect() && CExpendables::EXPEND_END != iter->Get_Type())
		{
			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				m_pInfoUI->Set_Type(iter->Get_Type());
				m_pBuyUI->Set_Active(true);
				//Buy_Expendable(iter);
			}
				
		}
	}
}

void CExpendCollectionUI::Buy_Expendable(CExpendOptionUI* pOption)
{
	switch (pOption->Get_Type())
	{
	case CExpendables::Expend_MaximumUp:
	{
		m_iCost = 100;
	}
	break;
	case CExpendables::Expend_Hp:
	{
		m_iCost = 50;

	}
	break;
	case CExpendables::Expend_Return:
	{
		m_iCost = 100;
	}
	break;
	case CExpendables::Expend_Blood:
	{
		m_iCost = 60;
	}
	break;
	case CExpendables::Expend_Cheet:
	{
		m_iCost = 300;
	}
	break;
	case CExpendables::Expend_SuperArmor:
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
		CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_MultiExpendables(pOption->Get_Type(), m_iBuyCnt);
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
