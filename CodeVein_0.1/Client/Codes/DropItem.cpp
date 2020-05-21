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

	//m_pEffect = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Totem_Fire_BulletBody");
	//m_pEffect->Set_Desc(V3_NULL, m_pTransform);
	//m_pEffect->Reset_Init();
	 
	//m_pGet_ItemUI = CUI_Manager::Get_Instance()->Get_Get_ItemUI();
	m_pGet_ItemUI = static_cast<CGet_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Get_ItemUI", pArg));
	m_pGet_ItemUI->Ready_GameObject(NULL);

	m_pPickUpMgr = CUI_Manager::Get_Instance()->Get_PickUP_ItemUIMgr();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	return S_OK;
}

_int CDropItem::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	m_dCanGetItemLimitTimeCur += TimeDelta;
	m_fTempEffectLimitTime += (_float)TimeDelta;

	//상호작용 대상과 충돌 여부 체크
	Check_Dist();

	//0.05초마다 이펙트효과 발생
	if (m_fTempEffectLimitTime > 0.05f)
	{
		m_fTempEffectLimitTime = 0.f;
		//cout << "아이템 생성된 효과 발생" << endl;
		switch (m_eItemGrade)
		{
		case ITEM_GRADE_NORMAL:
			g_pManagement->Create_Effect(L"ItemObject", m_pTransform->Get_Pos());
			break;
		case ITEM_GRADE_RARE:
			g_pManagement->Create_Effect(L"ItemObject_Purple", m_pTransform->Get_Pos());
			break;
		case ITEM_GRADE_UNIQUE:
			g_pManagement->Create_Effect(L"ItemObject_Red", m_pTransform->Get_Pos());
			break;
		case ITEM_GRADE_LEGEND:
			g_pManagement->Create_Effect(L"ItemObject_Yellow", m_pTransform->Get_Pos());
			break;
		}
	}

	//1.제한 시간 초과된 상태
	if (m_dCanGetItemLimitTimeMax <= m_dCanGetItemLimitTimeCur)
	{
		//아이템 등급
		switch (m_eItemGrade)
		{
		case ITEM_GRADE_NORMAL:
			break;
		case ITEM_GRADE_RARE:
			break;
		case ITEM_GRADE_UNIQUE:
			break;
		case ITEM_GRADE_LEGEND:
			break;
		}
		//단순 소멸
		//g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_0", m_pTransform->Get_Pos());
		//g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_1", m_pTransform->Get_Pos());
		//g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_Particle", m_pTransform->Get_Pos());
		m_bEnable = false;
		//m_pEffect->Set_Dead();

		//return DEAD_OBJ;
	}
	//2.제한 시간 이내
	else
	{
		//if (true == m_bCanGetItem)
		//{
		//	m_bCanGetItem = false;

		//	//"줍는다" 관련 변수가 참인 경우도 포함할것
		//	//섬광이펙트,파티클이펙트 등등 획득한 티를 내주고
		//	//획득 ui를 보여줘야함 ("획득")
		//	//인벤으로 저장

		//	//아이템 등급
		//	switch (m_eItemGrade)
		//	{
		//	case ITEM_GRADE_NORMAL:
		//		break;
		//	case ITEM_GRADE_RARE:
		//		break;
		//	case ITEM_GRADE_UNIQUE:
		//		break;
		//	case ITEM_GRADE_LEGEND:
		//		break;
		//	}
		//}

		if (true == m_pGet_ItemUI->Get_Show_ItemName())
		{
			CItem_Manager* pItem_Mgr = CItem_Manager::Get_Instance();
			CUI_Manager* pUI_Mgr = CUI_Manager::Get_Instance();
			CWeapon* pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));

			if (!pWeapon)
				return E_FAIL;

			CPickUp_ItemUI* pPickUp = static_cast<CPickUp_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PickUP_ItemUI", nullptr));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pPickUp, SCENE_MORTAL, L"Layer_PickUp_ItemUI", nullptr);

			pPickUp->Update_GameObject(TimeDelta);
			pPickUp->Set_Active(true);
			
			//CPickUp_ItemUIManager* pPickUpMgr = static_cast<CPickUp_ItemUIManager*>(g_pManagement->Clone_GameObject_Return(L"GameObject_PIckUP_ItemUIMgr", nullptr));
			//g_pManagement->Add_GameOject_ToLayer_NoClone(pPickUpMgr, SCENE_MORTAL, L"Layer_PickUP_ItemUIMgr", nullptr);

			//pPickUpMgr->Update_GameObject(TimeDelta);
			//pPickUpMgr->Set_Active(true);

			m_pPickUpMgr->Update_GameObject(TimeDelta);
			m_pPickUpMgr->Set_Active(true);

			switch (m_eItem_NameType)
			{
				case NAMETYPE_WpnAll_Gun_Bayonet:
				{
					pUI_Mgr->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun));
					m_iRenderIndex = 0;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Gun_ReverTardor:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn));
					m_iRenderIndex = 1;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
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
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Gun_DarkBayonet:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Gun_Black));
					m_iRenderIndex = 3;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Halberd_SolidPiercer:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_));
					m_iRenderIndex = 4;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Halberd_Black:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Black));
					m_iRenderIndex = 5;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Halberd_DarkBodish:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hal));
					m_iRenderIndex = 6;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Halberd_Nakil:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Halberd_Military));
					m_iRenderIndex = 7;
					//pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					pPickUp->Set_ItemNumber_Index(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Hammer_ImperseAnker:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_));
					m_iRenderIndex = 8;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Hammer_Black:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Black));
					m_iRenderIndex = 9;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Hammer_DarkHeavyAxe:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer));
					m_iRenderIndex = 10;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_Hammer_Nakil:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_Hammer_Military));
					m_iRenderIndex = 11;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_LSword_Tsubai:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_));
					m_iRenderIndex = 12;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_LSword_Black:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Black));
					m_iRenderIndex = 13;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_LSword_Nakil:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_Military));
					m_iRenderIndex = 14;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_LSword_DarkTsubai:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_LSword_));
					m_iRenderIndex = 15;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_SSword_Red:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword));
					m_iRenderIndex = 16;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_SSword_Executioner:
				{
					//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_));
					m_iRenderIndex = 17;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_SSword_Nakil:
				{
					m_iRenderIndex = 18;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
					m_pPickUpMgr->Set_Count_GetItem(m_iCount_GetItem);
					m_pPickUpMgr->Set_vecPickUp_Item(pPickUp);
					break;
				}
				case NAMETYPE_WpnAll_SSword_DarkBrodeSword:
				{
					CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(pWeapon->Get_WeaponParam(Wpn_SSword_Black));
					m_iRenderIndex = 19;
					pItem_Mgr->Set_PickUp_Number(m_iRenderIndex);
					m_bCanGetItem = true;
					++m_iCount_GetItem;
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
				case NAMETYPE_Pet_PoisonButterfly:
				{
					CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(CPet::PET_TYPE::PET_POISONBUTTERFLY);
					m_bCanGetItem = true;
					break;
				}
				case NAMETYPE_Pet_DeerKing:
				{
					CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(CPet::PET_TYPE::PET_DEERKING);
					m_bCanGetItem = true;
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
			m_bCanGetItem = false;

			if (true == m_bCanGetItem)
			{
				m_bEnable = false;
				m_bCanGetItem = false;
				return DEAD_OBJ;
			}
	}
	

	if (true == m_bCheck_Start_GetItemUI/* && 2.f <= D3DXVec3Length(&(m_pTransform->Get_Pos() - TARGET_TO_TRANS(m_pTarget)->Get_Pos()))*/)
	{
		m_pGet_ItemUI->Update_GameObject(TimeDelta);
		//m_pGet_ItemUI->Set_Active(true);
		// 이미지가 바뀌었을 때 m_bCanGetItem = true;
	}
	/*else
		m_pGet_ItemUI->Set_Active(false);*/


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

	if (nullptr != m_pGet_ItemUI)
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
		{
			m_bCheck_Start_GetItemUI = false;
		}
			
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
}

void CDropItem::Check_PosY()
{
	//m_pTransform->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransform->Get_Pos()));

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
		m_eItemType = ITEM_TYPE::ITEM_MATERIAL;
		m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_NORMAL;
		m_eItem_NameType = ITEM_NAMETYPE::NAMETYPE_WpnAll_Gun_Bayonet;
		m_eMaterialType = CMaterial::MATERIAL_TYPE::Queen_Steel;

		m_pTransform->Set_Pos(_v3{ 1.f, 0.f, 1.f });
		m_dCanGetItemLimitTimeMax = 5;
	}
	else
	{
		ITEM_STATUS info = *(ITEM_STATUS*)_pArg;
		m_eItemType = info.eItemType;

		switch (m_eItemType)
		{
		case ITEM_TYPE::ITEM_MATERIAL:
			m_eMaterialType = info.eMaterialType;
			break;
		case ITEM_TYPE::ITEM_EXPENDABLES:
			m_eExpendablesType = info.eExpendablesType;
			break;
		case ITEM_TYPE::ITEM_WEAPON:
			m_eWeaponType = info.eWeaponType;
			break;
		case ITEM_TYPE::ITEM_PET:
			m_ePetType = info.ePetType;
			
		}

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
	//IF_NOT_NULL(m_pEffect)
	//	m_pEffect->Set_Dead();

	IF_NOT_NULL(m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	IF_NOT_NULL(m_pGet_ItemUI)
		Safe_Release(m_pGet_ItemUI);

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}
