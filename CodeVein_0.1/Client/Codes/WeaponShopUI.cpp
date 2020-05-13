#include "stdafx.h"
#include "..\Headers\WeaponShopUI.h"
#include "Player.h"
#include "UI_Manager.h"
#include "CollisionMgr.h"

CWeaponShopUI::CWeaponShopUI(_Device pDevice)
	: CUI(pDevice)
{
}

CWeaponShopUI::CWeaponShopUI(const CWeaponShopUI & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponShopUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CWeaponShopUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	m_bIsActive = false;
	m_fAlpha = 1.f;

	CWeaponShopOptionUI* pInstance = nullptr;
	LOOP(4)
	{
		pInstance = static_cast<CWeaponShopOptionUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponShopOptionUI", nullptr));
		pInstance->Setup_InvenType(CWeaponShopOptionUI::WEAPONSHOP_OPTION(i));
		g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecOption.push_back(pInstance);
	}
	
	m_pTransformCom->Set_Scale(_v3(1.5f, 2.246f, 1.f));
	m_pTransformCom->Set_Angle(V3_NULL);
	m_pTransformCom->Set_Angle(AXIS_Y, D3DXToRadian(20.f));

	m_bFirestMenu = true;
	
	return NOERROR;
}

_int CWeaponShopUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_ALPHA_UI, this);

	if (nullptr == m_pTarget)
		return NO_EVENT;

	CTransform* pTargetTrans = TARGET_TO_TRANS(m_pTarget);
	
	_v3 vLook = pTargetTrans->Get_Axis(AXIS_Z);
	_v3 vRight = pTargetTrans->Get_Axis(AXIS_X);
	
	// x Ãà ¹æÇâº¤ÅÍ
	V3_NORMAL_SELF(&vLook);
	V3_NORMAL_SELF(&vRight);

	// x Ãà ¹æÇâº¤ÅÍ + ÇÃ·¹ÀÌ¾î
	_v3 vPosition = pTargetTrans->Get_Pos() + (vLook * 0.25f) + (vRight * 0.8f) + (WORLD_UP * 1.5f);
	m_pTransformCom->Set_Pos(vPosition);
	
	_float fOriginYRadian = pTargetTrans->Get_Angle().y;
	_float fAdditionalYRadian = fOriginYRadian + D3DXToRadian(160.f);
	m_pTransformCom->Set_Angle(AXIS_Y, fAdditionalYRadian);

	if (m_bFirestMenu)
	{
		LOOP(4)
		{
			TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
			TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.2476f, 1.f));
			TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
			TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.285f, 0.f) + *V3_NORMAL_SELF(&vLook) * +0.001f);
			m_vecOption[i]->Set_Menu(CWeaponShopOptionUI::WEAPONSHOP_MENU::MENU_NONE);
			m_vecOption[i]->Setup_InvenType(CWeaponShopOptionUI::WEAPONSHOP_OPTION(i));
			m_vecOption[i]->Set_Active(m_bIsActive);
		}
	}
	else
	{
		if (CWeaponShopOptionUI::OPTION_BUY == m_eState)
		{
			LOOP(4)
				m_vecOption[i]->Set_Active(false);
			LOOP(2)
			{
				TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
				TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.2476f, 1.f));
				TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
				TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.08f, 0.f) + *V3_NORMAL_SELF(&vLook) * +0.001f);
				m_vecOption[i]->Set_Menu(CWeaponShopOptionUI::WEAPONSHOP_MENU::MENU_BUY);
				m_vecOption[i]->Set_BuyOption(CWeaponShopOptionUI::WEAPONSHOP_OPTION_BUY(i));
				m_vecOption[i]->Set_Active(m_bIsActive);
			}
		}
		else if (CWeaponShopOptionUI::OPTION_SELL == m_eState)
		{
			LOOP(4)
				m_vecOption[i]->Set_Active(false);
			LOOP(3)
			{
				TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
				TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.2476f, 1.f));
				TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
				TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.18f, 0.f) + *V3_NORMAL_SELF(&vLook) * +0.001f);
				m_vecOption[i]->Set_Menu(CWeaponShopOptionUI::WEAPONSHOP_MENU::MENU_SELL);
				m_vecOption[i]->Set_BuyOption(CWeaponShopOptionUI::WEAPONSHOP_OPTION_BUY(i));
				m_vecOption[i]->Set_Active(m_bIsActive);
			}
		}
		else if (CWeaponShopOptionUI::OPTION_UPGRADE == m_eState)
		{
			LOOP(4)
				m_vecOption[i]->Set_Active(false);
			LOOP(2)
			{
				TARGET_TO_TRANS(m_vecOption[i])->Set_Angle(m_pTransformCom->Get_Angle());
				TARGET_TO_TRANS(m_vecOption[i])->Set_Scale(_v3(0.87f, 0.2476f, 1.f));
				TARGET_TO_TRANS(m_vecOption[i])->Set_At(m_pTransformCom->Get_At());
				TARGET_TO_TRANS(m_vecOption[i])->Set_Pos(m_pTransformCom->Get_Pos() + _v3(0.f, _float(i) * -0.2f + 0.08f, 0.f) + *V3_NORMAL_SELF(&vLook) * +0.001f);
				m_vecOption[i]->Set_Menu(CWeaponShopOptionUI::WEAPONSHOP_MENU::MENU_UPGRADE);
				m_vecOption[i]->Set_BuyOption(CWeaponShopOptionUI::WEAPONSHOP_OPTION_BUY(i));
				m_vecOption[i]->Set_Active(m_bIsActive);
			}
		}

	}
	

	if (!m_bIsActive)
	{
		m_bFirestMenu = true;

		//m_pStageSelectUI->Set_Active(false);
		//CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_Active(false);
	}
	
	_v3 vWorldPos;
	memcpy(vWorldPos, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	Compute_ViewZ(&vWorldPos);

	Click_Option();
	Check_Key();

	return NO_EVENT;
}

_int CWeaponShopUI::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	m_matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	m_matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	return NO_EVENT;
}

HRESULT CWeaponShopUI::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();
	m_pShaderCom->Begin_Pass(6);

	m_pBufferCom->Render_VIBuffer();
	m_pShaderCom->End_Pass();
	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CWeaponShopUI::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (!m_bIsActive)
		return NOERROR;
	if (nullptr == pShader ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;

	pShader->Begin_Shader();
	pShader->Begin_Pass(6);

	m_pBufferCom->Render_VIBuffer();
	pShader->End_Pass();
	pShader->End_Shader();

	return NOERROR;
}

HRESULT CWeaponShopUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_WeaponShopUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_3dUI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CWeaponShopUI::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", pShader, _uint(0))))
		return E_FAIL;

	pShader->Set_Texture("g_DepthTexture", g_pManagement->Get_Target_Texture(L"Target_DepthUI"));
	return NOERROR;
}

void CWeaponShopUI::Click_Option()
{
	if (!m_bIsActive)
		return;

	_int iIdx = 0;

	for (auto& iter : m_vecOption)
	{
		if (CCollisionMgr::Collision_Ray(iter, g_pInput_Device->Get_Ray(), &m_fCross))
		{
			Reset_Option();
			iter->Set_Select(true);

			if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
			{
				if (m_bFirestMenu)
				{
					if (0 == iIdx)
					{
						m_eState = CWeaponShopOptionUI::OPTION_UPGRADE;
						m_bFirestMenu = false;
					}
					else if (1 == iIdx)
					{
						m_eState = CWeaponShopOptionUI::OPTION_BUY;
						m_bFirestMenu = false;
					}
					else if (2 == iIdx)
					{
						m_eState = CWeaponShopOptionUI::OPTION_SELL;
						m_bFirestMenu = false;
					}
					else if (3 == iIdx)
					{
						Set_Active(false);
						m_bFirestMenu = true;
					}
				}
				else if (CWeaponShopOptionUI::OPTION_UPGRADE == m_eState)
				{
					if (0 == iIdx) // ¹«±â
					{
						m_pUpgradeInven->Set_Active(true);
						m_pBuyUI->Set_Active(true);
						m_pBuyUI->Set_ShopType(CWeaponBuyUI::SHOP_UPGRADE);
					}
					else if (1 == iIdx) // ÈíÇ÷¾ÆÀå
						_int a = 0;

					m_bPopupOn = true;
					Set_Active(false);
				}
				else if (CWeaponShopOptionUI::OPTION_BUY == m_eState)
				{
					if (0 == iIdx) // ¹«±â
					{
						m_pBuyInven->Set_Active(true);
						m_pBuyUI->Set_Active(true);
						m_pBuyUI->Set_ShopType(CWeaponBuyUI::SHOP_WEAPON_BUY);
					}
					else if (1 == iIdx) // ÈíÇ÷¾ÆÀå
						_int a = 0;

					m_bPopupOn = true;
					Set_Active(false);
				}
				else if (CWeaponShopOptionUI::OPTION_SELL == m_eState)
				{
					if (0 == iIdx) // ¹«±â
					{
						m_pSellInven->Set_Active(true);
						m_pBuyUI->Set_Active(true);
						m_pBuyUI->Set_ShopType(CWeaponBuyUI::SHOP_WEAPON_SELL);
					}
					else if (1 == iIdx) // ÈíÇ÷¾ÆÀå
						_int a = 0;

					m_bPopupOn = true;
					Set_Active(false);
				}

				//Active_SubUI(iter);
			}
				
		}
		else
		{
			iter->Set_Select(false);
		}
	
		++iIdx;
	}
}

void CWeaponShopUI::Reset_Option()
{
	for (auto& iter : m_vecOption)
	{
		iter->Set_Select(false);
	}
}

void CWeaponShopUI::Check_Key()
{
	if (!m_bIsActive && !m_bPopupOn)
		return;

	if (g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_RB))
	{
		if (m_bPopupOn || !m_bFirestMenu)
		{
			switch (m_eState)
			{
			case Client::CWeaponShopOptionUI::OPTION_UPGRADE:
			case Client::CWeaponShopOptionUI::OPTION_BUY:
			case Client::CWeaponShopOptionUI::OPTION_SELL:
			{
				m_bFirestMenu = true;
				if (m_bPopupOn)
				{
					m_eState = CWeaponShopOptionUI::OPTION_END;

					m_pSellInven->Set_Active(false);
					m_pBuyInven->Set_Active(false);
					m_pUpgradeInven->Set_Active(false);
					m_pBuyUI->Set_Active(false);
				}
				break;
			}
			}

		}
		else if(m_bFirestMenu)
		{
			Set_Active(false);
		}
	}
}

void CWeaponShopUI::Setup_AfterClone()
{
	m_pBuyUI = static_cast<CWeaponBuyUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_WeaponBuyUI", nullptr));
	m_pBuyUI->Set_Parent(this);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBuyUI, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pBuyInven = static_cast<CWeapon_Inven_InShop*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_Inven_InShop", nullptr));
	m_pBuyInven->Setup_InvenType(CWeapon_Inven_InShop::INVEN_SHOP_OPTION::SHOP_WEAPON_BUY);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBuyInven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pSellInven = static_cast<CWeapon_Inven_InShop*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_Inven_InShop", nullptr));
	m_pSellInven->Setup_InvenType(CWeapon_Inven_InShop::INVEN_SHOP_OPTION::SHOP_WEAPON_SELL);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSellInven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);

	m_pUpgradeInven = static_cast<CWeapon_Inven_InShop*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon_Inven_InShop", nullptr));
	m_pUpgradeInven->Setup_InvenType(CWeapon_Inven_InShop::INVEN_SHOP_OPTION::SHOP_WEAPON_UPGRADE);
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pUpgradeInven, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
}

void CWeaponShopUI::Active_SubUI(CWeaponShopOptionUI* pSelectOption)
{
	if (!m_bIsActive ||
		nullptr == pSelectOption)
		return;

	switch (pSelectOption->Get_Option())
	{
	case CWeaponShopOptionUI::OPTION_UPGRADE:
		//CUI_Manager::Get_Instance()->Get_StageSelectUI()->Set_Active(!CUI_Manager::Get_Instance()->Get_StageSelectUI()->Get_Active());
		break;
	case CWeaponShopOptionUI::OPTION_BUY: 
		//CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Set_Active(!CUI_Manager::Get_Instance()->Get_BloodCode_Menu()->Get_Active());
		break;
	case CWeaponShopOptionUI::OPTION_SELL:
		break;
	case CWeaponShopOptionUI::OPTION_BYEBYE:
		break;
	}
}

CWeaponShopUI * CWeaponShopUI::Create(_Device pGraphic_Device)
{
	CWeaponShopUI* pInstance = new CWeaponShopUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CWeaponShopUI::Clone_GameObject(void * pArg)
{
	CWeaponShopUI* pInstance = new CWeaponShopUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CWeaponShopUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
