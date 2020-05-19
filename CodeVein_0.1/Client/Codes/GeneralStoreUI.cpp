#include "stdafx.h"
#include "..\Headers\GeneralStoreUI.h"
#include "UI_Manager.h"

CGeneralStoreUI::CGeneralStoreUI(_Device pDevice)
	: CUI(pDevice)
{
}

CGeneralStoreUI::CGeneralStoreUI(const CGeneralStoreUI & rhs)
	: CUI(rhs)
{
}

HRESULT CGeneralStoreUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CGeneralStoreUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 1280.f;
	m_fSizeY = 700.f;

	SetUp_Default();

	return NOERROR;
}

_int CGeneralStoreUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	
	Update_SubUI();
	Click_SubUI();

	//// 디버깅용
	//if(g_pInput_Device->Key_Up(DIK_P))
	//	m_bIsActive = !m_bIsActive;
	
	return NO_EVENT;
}

_int CGeneralStoreUI::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CGeneralStoreUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);
	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	if (FAILED(SetUp_ConstantTable(0)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(1);
	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CGeneralStoreUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_GeneralStoreUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CGeneralStoreUI::SetUp_ConstantTable(_uint iIndex)
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

void CGeneralStoreUI::SetUp_Default()
{
	LOOP(2)
	{
		m_pShopIcon[i] = static_cast<CInventory_Icon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_InvenIcon", nullptr));
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pShopIcon[i], SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_pShopIcon[i]->Set_UI_Size(30.f, 30.f);
		m_pShopIcon[i]->Set_UI_Pos(50.f + 100.f * i, 100.f);
	}
	m_pShopIcon[0]->Set_Type(CInventory_Icon::ICON_TYPE::ICON_EXPEND);
	m_pShopIcon[1]->Set_Type(CInventory_Icon::ICON_TYPE::ICON_MTRL);

	// 소비상점UI 생성
	m_pExpendCollectionUI = static_cast<CExpendCollectionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_ExpendCollectionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pExpendCollectionUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	// 재료상점UI 생성
	m_pMaterialCollectionUI = static_cast<CMaterialCollectionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialCollectionUI", nullptr));
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pMaterialCollectionUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CGeneralStoreUI::Update_SubUI()
{
	LOOP(2)
	{
		m_pShopIcon[i]->Set_Active(m_bIsActive);
		m_pShopIcon[i]->Set_UI_Pos(80.f + 80.f * i, 130.f);
		m_pShopIcon[i]->Set_UI_Size(40.f, 40.f);
	}

	if (!m_bIsActive)
	{
		m_pExpendCollectionUI->Set_Active(false);
		m_pMaterialCollectionUI->Set_Active(false);
		m_bIsOpen = false;
	}
	else if (m_bIsActive && !m_bIsOpen)
	{
		m_pExpendCollectionUI->Set_Active(true);
		m_bIsOpen = true;
	}
}

void CGeneralStoreUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;

	// 소비템 버튼 클릭시
	if (m_pShopIcon[0]->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_pExpendCollectionUI->Set_Active(true);
		m_pMaterialCollectionUI->Set_Active(false);

		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::CHANNELID::Purchase_GeneralShop_Icon01, CSoundManager::Ambient_Sound);
	}

	// 재료 버튼 클릭시
	if (m_pShopIcon[1]->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_pExpendCollectionUI->Set_Active(false);
		m_pMaterialCollectionUI->Set_Active(true);

		g_pSoundManager->Play_Sound(L"UI_CommonHover.wav", CSoundManager::CHANNELID::Purchase_GeneralShop_Icon02, CSoundManager::Ambient_Sound);
	}
}

CGeneralStoreUI * CGeneralStoreUI::Create(_Device pGraphic_Device)
{
	CGeneralStoreUI* pInstance = new CGeneralStoreUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CGeneralStoreUI::Clone_GameObject(void * pArg)
{
	CGeneralStoreUI* pInstance = new CGeneralStoreUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CGeneralStoreUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
