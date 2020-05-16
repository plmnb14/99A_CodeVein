#include "stdafx.h"
#include "..\Headers\MaterialBuyUI.h"
#include "UI_Manager.h"

CMaterialBuyUI::CMaterialBuyUI(_Device pDevice)
	: CUI(pDevice)
{
}

CMaterialBuyUI::CMaterialBuyUI(const CMaterialBuyUI & rhs)
	: CUI(rhs)
{
}

HRESULT CMaterialBuyUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CMaterialBuyUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return NOERROR;
}

_int CMaterialBuyUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	Update_SubUI(TimeDelta);
	Click_SubUI();

	return NO_EVENT;
}

_int CMaterialBuyUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMaterialBuyUI::Render_GameObject()
{
	if (!m_bIsActive && m_fAlpha <= 0.f)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(m_iIndex)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(5);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CMaterialBuyUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_GeneralStoreBuyUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMaterialBuyUI::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

void CMaterialBuyUI::SetUp_Default()
{
	m_iIndex = 0;

	m_fPosX = 650.f;
	m_fPosY = 350.f;
	m_fSizeX = 512.f;
	m_fSizeY = 256.f;

	m_pCntMinusOption = static_cast<CBuyOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BuyOptionUI", nullptr));
	m_pCntMinusOption->Set_UI_Pos(m_fPosX - 150.f, 350.f);
	m_pCntMinusOption->Set_UI_Size(50.f, 50.f);
	m_pCntMinusOption->Set_ViewZ(m_fViewZ - 0.1f);
	m_pCntMinusOption->Set_UI_Index(1);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCntMinusOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pCntPlusOption = static_cast<CBuyOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BuyOptionUI", nullptr));
	m_pCntPlusOption->Set_UI_Pos(m_fPosX + 150.f, 350.f);
	m_pCntPlusOption->Set_UI_Size(50.f, 50.f);
	m_pCntPlusOption->Set_ViewZ(m_fViewZ - 0.1f);
	m_pCntPlusOption->Set_UI_Index(2);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pCntPlusOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pDecisionOption = static_cast<CBuyOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BuyOptionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pDecisionOption, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pDecisionOption->Set_UI_Pos(m_fPosX, 420.f);
	m_pDecisionOption->Set_UI_Size(150.f, 80.f);
	m_pDecisionOption->Set_UI_Index(3);
	m_pDecisionOption->Set_ViewZ(m_fViewZ - 0.1f);

	m_pBuyCntFont = static_cast<CPlayerFontUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PlayerFontUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBuyCntFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	m_pBuyCntFont->Set_UI_Pos(m_fPosX, m_fPosY);
	m_pBuyCntFont->Set_UI_Size(20.f, 40.8f);
	m_pBuyCntFont->Set_ViewZ(m_fViewZ - 0.1f);
}

void CMaterialBuyUI::Update_SubUI(_double TimeDelta)
{
	m_pCntMinusOption->Set_Active(m_bIsActive);
	m_pCntPlusOption->Set_Active(m_bIsActive);
	m_pDecisionOption->Set_Active(m_bIsActive);
	m_pBuyCntFont->Set_Active(m_bIsActive);

	m_pBuyCntFont->Update_NumberValue(_float(m_iBuyCnt));

	if (m_bIsActive && m_fAlpha < 1.f)
		m_fAlpha += _float(TimeDelta) * 1.2f;
	if (!m_bIsActive && m_fAlpha > 0.f)
		m_fAlpha -= _float(TimeDelta) * 1.2f;

	if (!m_bIsActive)
	{
		m_iBuyCnt = 0;
	}
}

void CMaterialBuyUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;
	if (m_pCntMinusOption->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_iBuyCnt > 0)
			m_iBuyCnt--;
		else return;
	}
	else if (m_pCntPlusOption->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_iBuyCnt < 9)
			m_iBuyCnt++;
		else return;
	}
	else if (m_pDecisionOption->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_eType == CMaterial::MATERIAL_END)
			return;

		Buy_Item();
	}
}

void CMaterialBuyUI::SetUp_Cost()
{
	// 개당 가격 설정
	switch (m_eType)
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
}

void CMaterialBuyUI::Buy_Item()
{
	SetUp_Cost();

	_uint iTotalCost = m_iCost * m_iBuyCnt; // 총 비용 = 개당 가격 * 사는 개수

	_ulong iHazeCnt = CUI_Manager::Get_Instance()->Get_HazeUI()->Get_Haze_Cnt();

	// 가지고 있는 헤이즈가 구매 총 비용 이상일 경우 -> 돈 충분함 -> 구매한다.
	if (iHazeCnt >= iTotalCost)
	{
		// 구매해서 소비 인벤에 넣는다.
		CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(m_eType, m_iBuyCnt);
		// 헤이즈를 탕진한 만큼 감소시킨다
		CUI_Manager::Get_Instance()->Get_HazeUI()->Accumulate_Haze(-(_int)iTotalCost);

		m_bIsActive = false;
	}
	// 가지고 있는 헤이즈보다 구매 총 비용이 더 클 경우 -> 돈 부족함 -> 구매X
	else
	{
		cout << "비용 부족!" << endl;
		m_bIsActive = false;
		return;
	}
}

CMaterialBuyUI * CMaterialBuyUI::Create(_Device pGraphic_Device)
{
	CMaterialBuyUI* pInstance = new CMaterialBuyUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("MaterialBuyUI Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMaterialBuyUI::Clone_GameObject(void * pArg)
{
	CMaterialBuyUI* pInstance = new CMaterialBuyUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("MaterialBuyUI Clone Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMaterialBuyUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
