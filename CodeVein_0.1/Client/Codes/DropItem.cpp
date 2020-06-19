#include "stdafx.h"
#include "..\Headers\DropItem.h"

#include "Get_ItemUI.h"

CDropItem::CDropItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CDropItem::CDropItem(const CDropItem & rhs)
	:CGameObject(rhs)
{
}

HRESULT CDropItem::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDropItem::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component(pArg)))
			return E_FAIL;

		return S_OK;
	}

	Ready_Status(pArg);

	//m_pGet_ItemUI = CUI_Manager::Get_Instance()->Get_Get_ItemUI();
	m_pGet_ItemUI = static_cast<CGet_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Get_ItemUI", pArg));
	m_pGet_ItemUI->Ready_GameObject(NULL);

	m_pPickUpMgr = CUI_Manager::Get_Instance()->Get_PickUP_ItemUIMgr();

	return S_OK;
}

_int CDropItem::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	m_dCanGetItemLimitTimeCur += TimeDelta;

	Check_Dist(); //상호작용 대상과 충돌 여부
	Check_Condition(); //조건에 따른 획득 여부

	return S_OK;
}

_int CDropItem::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	if (true == m_bInFrustum)
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;

		if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
	}

	IF_NOT_NULL(m_pGet_ItemUI)
		m_pGet_ItemUI->Late_Update_GameObject(TimeDelta);

	return S_OK;
}

HRESULT CDropItem::Render_GameObject()
{
	return S_OK;
}

void CDropItem::Check_Dist()
{
	_float fDist;
	// 펫은 자동으로 획득되게 해야 하기 때문에 m_bCheck_Start_GetItemUI를 사용하지 않는다.

	//p가 접근, 행동o, 제한 시간 이내라면 불값 참
	//p가 접근, 행동o, 제한 시간 초과라면 불값 거짓

	//p가 접근, 행동x 안획득 불값 거짓

	//p와의 거리가 가까운 경우 참, 아닌경우 거짓
	auto& listPlayer = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	auto& listPet = g_pManagement->Get_GameObjectList(L"Layer_Pet", SCENE_STAGE);

	for (auto& Player_iter : listPlayer)
	{
		if (true == Player_iter->Get_Dead())
			continue;
		else if (false == Player_iter->Get_Enable())
			continue;
		else if (nullptr == Player_iter)
			continue;

		fDist = V3_LENGTH(&(TARGET_TO_TRANS(Player_iter)->Get_Pos() - m_pTransform->Get_Pos()));

		if (m_fCanGetDist >= fDist)
		{
			m_bCheck_Start_GetItemUI = true;
			return;
		}
		else
			m_bCheck_Start_GetItemUI = false;		
	}

	for (auto& Pet_iter : listPet)
	{
		if (true == Pet_iter->Get_Dead())
			continue;
		else if (false == Pet_iter->Get_Enable())
			continue;
		else if (nullptr == Pet_iter)
			continue;

		fDist = V3_LENGTH(&(TARGET_TO_TRANS(Pet_iter)->Get_Pos() - m_pTransform->Get_Pos()));

		if (m_fCanGetDist >= fDist)
		{
			m_bCanGetItem = true;
			return;
		}
		else
			m_bCanGetItem = false;
	}

	return;
}

void CDropItem::Check_Condition()
{	
	//제한 시간 이내로 획득할 경우
	if (m_dCanGetItemLimitTimeCur <= m_dCanGetItemLimitTimeMax)
	{
		if (true == m_pGet_ItemUI->Get_Show_ItemName())
		{
			CItem_Manager* pItem_Mgr = CItem_Manager::Get_Instance();
			CUI_Manager* pUI_Mgr = CUI_Manager::Get_Instance();
			CWeapon* pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));

			if (!pWeapon)
				return;

			CPickUp_ItemUI* pPickUp = static_cast<CPickUp_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PickUP_ItemUI", nullptr));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pPickUp, SCENE_MORTAL, L"Layer_PickUp_ItemUI", nullptr);

			pPickUp->Update_GameObject(DELTA_60);
			pPickUp->Set_Active(true);

			//CPickUp_ItemUIManager* pPickUpMgr = static_cast<CPickUp_ItemUIManager*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PIckUP_ItemUIMgr", nullptr));
			//g_pManagement->Add_GameOject_ToLayer_NoClone(pPickUpMgr, SCENE_MORTAL, L"Layer_PickUP_ItemUIMgr", nullptr);

			//pPickUpMgr->Update_GameObject(TimeDelta);
			//pPickUpMgr->Set_Active(true);

			m_pPickUpMgr->Update_GameObject(DELTA_60);
			m_pPickUpMgr->Set_Active(true);

			switch (m_eItem_NameType)
			{
			case NAMETYPE_WpnAll_Gun_Bayonet:
			{
				pUI_Mgr->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun));
				m_iRenderIndex = 0;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Gun_ReverTardor:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Black));
				m_iRenderIndex = 1;
				pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Gun_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Military));
				m_iRenderIndex = 2;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Gun_DarkBayonet:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Slave));
				m_iRenderIndex = 3;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_SolidPiercer:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd));
				m_iRenderIndex = 4;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_Black:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Black));
				m_iRenderIndex = 5;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_DarkBodish:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Slave));
				m_iRenderIndex = 6;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Military));
				m_iRenderIndex = 7;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_ImperseAnker:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer));
				m_iRenderIndex = 8;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_Black:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Black));
				m_iRenderIndex = 9;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_DarkHeavyAxe:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Slave));
				m_iRenderIndex = 10;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Military));
				m_iRenderIndex = 11;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_Tsubai:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword));
				m_iRenderIndex = 12;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_Black:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Black));
				m_iRenderIndex = 13;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Military));
				m_iRenderIndex = 14;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_DarkTsubai:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Slave));
				m_iRenderIndex = 15;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_Red:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword));
				m_iRenderIndex = 16;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_Executioner:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Black));
				m_iRenderIndex = 17;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Military));
				m_iRenderIndex = 18;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_DarkBrodeSword:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Slave));
				m_iRenderIndex = 19;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_MaximumUp:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_MaximumUp);
				m_iRenderIndex = 20;
				pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Hp:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Hp);
				m_iRenderIndex = 21;
				pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Return:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Return);
				m_iRenderIndex = 22;
				m_bCanGetItem = true;
				++m_iCount_GetItem;

				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Blood:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Blood);
				m_iRenderIndex = 23;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;

				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Cheet:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Cheet);
				m_iRenderIndex = 24;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;

				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_SuperArmor:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_SuperArmor);
				m_iRenderIndex = 25;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;

				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Queen_Steel:
			{
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel, 1);
				m_iRenderIndex = 26;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Queen_Titanium:
			{
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium, 1);
				m_iRenderIndex = 27;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Queen_Tungsten:
			{
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten, 1);
				m_iRenderIndex = 28;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Artemis:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Artemis);
				m_iRenderIndex = 29;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Assassin:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Assassin);
				m_iRenderIndex = 30;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_DarkKnight:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_DarkKnight);
				m_iRenderIndex = 31;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Queen:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Queen);
				m_iRenderIndex = 32;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Berserker:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Berserker);
				m_iRenderIndex = 33;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Hephaestus:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Hephaestus);
				m_iRenderIndex = 34;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Fighter:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Fighter);
				m_iRenderIndex = 35;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Heimdal:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Heimdal);
				m_iRenderIndex = 36;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Hermes:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Hermes);
				m_iRenderIndex = 37;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Atlas:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Atlas);
				m_iRenderIndex = 38;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Prometheus:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Prometheus);
				m_iRenderIndex = 39;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Pet_PoisonButterfly:
			{
				CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(NAMETYPE_Pet_PoisonButterfly);
				m_iRenderIndex = 40;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Pet_DeerKing:
			{
				CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(NAMETYPE_Pet_DeerKing);
				m_iRenderIndex = 41;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_End:
			{
				/*m_bCanGetItem = false;
				m_iCount_GetItem = 0;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);*/
				break;
			}
			}
		}
		else
		{
			m_bCanGetItem = false;
			m_iCount_GetItem = 0;
			m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
		}

		if (true == m_bCanGetItem)
		{
			m_bEnable = false;
			m_bCanGetItem = false;

			g_pSoundManager->Stop_Sound(CSoundManager::Effect_SFX_01);
			g_pSoundManager->Play_Sound(L"ItemGetSound.ogg", CSoundManager::Effect_SFX_01, CSoundManager::Effect_Sound);

			CItem_Manager* pItem_Mgr = CItem_Manager::Get_Instance();
			CUI_Manager* pUI_Mgr = CUI_Manager::Get_Instance();
			CWeapon* pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));

			if (!pWeapon)
				return;

			CPickUp_ItemUI* pPickUp = static_cast<CPickUp_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PickUP_ItemUI", nullptr));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pPickUp, SCENE_MORTAL, L"Layer_PickUp_ItemUI", nullptr);

			switch (m_eItem_NameType)
			{
			case NAMETYPE_WpnAll_Gun_Bayonet:
			{
				pUI_Mgr->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun));
				m_iRenderIndex = 0;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Gun_ReverTardor:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Black));
				m_iRenderIndex = 1;
				pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Gun_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Military));
				m_iRenderIndex = 2;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Gun_DarkBayonet:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Slave));
				m_iRenderIndex = 3;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_SolidPiercer:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd));
				m_iRenderIndex = 4;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_Black:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Black));
				m_iRenderIndex = 5;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_DarkBodish:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Slave));
				m_iRenderIndex = 6;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Halberd_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Military));
				m_iRenderIndex = 7;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_ImperseAnker:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer));
				m_iRenderIndex = 8;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_Black:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Black));
				m_iRenderIndex = 9;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_DarkHeavyAxe:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Slave));
				m_iRenderIndex = 10;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_Hammer_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Military));
				m_iRenderIndex = 11;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_Tsubai:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword));
				m_iRenderIndex = 12;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_Black:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Black));
				m_iRenderIndex = 13;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Military));
				m_iRenderIndex = 14;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_LSword_DarkTsubai:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Slave));
				m_iRenderIndex = 15;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_Red:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword));
				m_iRenderIndex = 16;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_Executioner:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Black));
				m_iRenderIndex = 17;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_Nakil:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Military));
				m_iRenderIndex = 18;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_WpnAll_SSword_DarkBrodeSword:
			{
				CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Slave));
				m_iRenderIndex = 19;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_MaximumUp:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_MaximumUp);
				m_iRenderIndex = 20;
				pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Hp:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Hp);
				m_iRenderIndex = 21;
				pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Return:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Return);
				m_iRenderIndex = 22;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Blood:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Blood);
				m_iRenderIndex = 23;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_Cheet:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_Cheet);
				m_iRenderIndex = 24;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Expend_SuperArmor:
			{
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(CExpendables::EXPEND_TYPE::Expend_SuperArmor);
				m_iRenderIndex = 25;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Queen_Steel:
			{
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(CMaterial::MATERIAL_TYPE::Queen_Steel, 1);
				m_iRenderIndex = 26;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Queen_Titanium:
			{
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(CMaterial::MATERIAL_TYPE::Queen_Titanium, 1);
				m_iRenderIndex = 27;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Queen_Tungsten:
			{
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_MultiMaterial(CMaterial::MATERIAL_TYPE::Queen_Tungsten, 1);
				m_iRenderIndex = 28;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Artemis:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Artemis);
				m_iRenderIndex = 29;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Assassin:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Assassin);
				m_iRenderIndex = 30;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_DarkKnight:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_DarkKnight);
				m_iRenderIndex = 31;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Queen:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Queen);
				m_iRenderIndex = 32;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Berserker:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Berserker);
				m_iRenderIndex = 33;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Hephaestus:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Hephaestus);
				m_iRenderIndex = 34;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Fighter:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Fighter);
				m_iRenderIndex = 35;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Heimdal:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Heimdal);
				m_iRenderIndex = 36;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Hermes:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Hermes);
				m_iRenderIndex = 37;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Atlas:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Atlas);
				m_iRenderIndex = 38;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_BloodCode_Prometheus:
			{
				CUI_Manager::Get_Instance()->Get_BloodCode_SelectUI()->Release_BloodCode(BloodCode_ID::BloodCode_Prometheus);
				m_iRenderIndex = 39;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Pet_PoisonButterfly:
			{
				CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(NAMETYPE_Pet_PoisonButterfly);
				m_iRenderIndex = 40;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			case NAMETYPE_Pet_DeerKing:
			{
				CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(NAMETYPE_Pet_DeerKing);
				m_iRenderIndex = 41;
				m_bCanGetItem = true;
				++m_iCount_GetItem;
				pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
				m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
				m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
				break;
			}
			}
		}
	}
	else
		m_bEnable = false;

	if (true == m_bCheck_Start_GetItemUI/* && 2.f <= D3DXVec3Length(&(m_pTransform->Get_Pos() - TARGET_TO_TRANS(m_pTarget)->Get_Pos()))*/)
	{
		m_pGet_ItemUI->Update_GameObject(DELTA_60);
		//m_pGet_ItemUI->Set_Active(true);
		// 이미지가 바뀌었을 때 m_bCanGetItem = true;
	}
	/*else
	m_pGet_ItemUI->Set_Active(false);*/

	return;
}

HRESULT CDropItem::Add_Component(void* _pArg)
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimization)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDropItem::Ready_Status(void* _pArg)
{
	if (nullptr == _pArg)
	{
		m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_NORMAL;
		m_eItem_NameType = ITEM_NAMETYPE::NAMETYPE_Queen_Steel;

		m_pTransform->Set_Pos(_v3{ 1.f, 0.f, 1.f });
		m_dCanGetItemLimitTimeMax = 5;
	}
	else
	{
		ITEM_STATUS info = *(ITEM_STATUS*)_pArg;

		m_eItemGrade = info.eItemGradeType;
		m_eItem_NameType = info.eItem_NameType;
		m_dCanGetItemLimitTimeMax = info.dCanGetLimitTimeMax;
		m_pTransform->Set_Pos(info.vBirthPos);
	}

	m_dCanGetItemLimitTimeCur = 0;
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

HRESULT CDropItem::SetUp_ConstantTable(CShader* pShader)
{
	return S_OK;
}

CDropItem* CDropItem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDropItem* pInstance = new CDropItem(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDropItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDropItem::Clone_GameObject(void * pArg)
{
	CDropItem* pInstance = new CDropItem(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDropItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDropItem::Free()
{
	IF_NOT_NULL(m_pGet_ItemUI)
		Safe_Release(m_pGet_ItemUI);

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}
