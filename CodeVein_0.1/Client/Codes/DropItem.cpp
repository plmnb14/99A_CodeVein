#include "stdafx.h"
#include "..\Headers\DropItem.h"

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
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_Status(pArg);

	m_pEffect = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Totem_Fire_BulletBody");
	m_pEffect->Set_Desc(V3_NULL, m_pTransform);
	m_pEffect->Reset_Init();

	m_pGet_ItemUI = static_cast<CGet_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Get_ItemUI", pArg));
	m_pGet_ItemUI->Ready_GameObject(NULL);

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
		cout << "아이템 생성된 효과 발생" << endl;
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
		m_pEffect->Set_Dead();

		return DEAD_OBJ;
	}
	//2.제한 시간 이내
	else
	{
		if (true == m_bCanGetItem)
		{
			m_bCanGetItem = false;

			//"줍는다" 관련 변수가 참인 경우도 포함할것
			//섬광이펙트,파티클이펙트 등등 획득한 티를 내주고
			//획득 ui를 보여줘야함 ("획득")
			//인벤으로 저장

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
			//아이템 종류
			switch (m_eItemType)
			{
			//case ITEM_MATERIAL:
			//	CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_Material(m_eMaterialType);
			//	break;
			//case ITEM_EXPENDABLES:
			//	CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(m_eExpendablesType);
			//	break;
			//case ITEM_WEAPON:
			//	//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon();
			//	break;
			//case ITEM_PET:
			//	CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(m_ePetType);
			//	break;
			

			// 주워진 아이템 이넘값에 따라서 해당 이넘값과 같은 텍스트를 랜더 번호로 설정해서
			// 랜더할 때 해당 번호에 맞는 아이템 리소스를 랜더한다.
			// 조아!


			}
			//"줍는다" 관련 변수가 참인 경우도 포함할것
			//섬광이펙트,파티클이펙트 등등 획득한 티를 내주고
			//획득 ui를 보여줘야함 ("줍는다")
			//인벤으로 저장
			//switch (m_eItemType)
			//{
			//case ITEM_TYPE::ITEM_MATERIAL:
			//	CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_Material(m_eMaterialType);
			//	break;
			//case ITEM_TYPE::ITEM_EXPENDABLES:
			//	CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(m_eExpendablesType);
			//	break;
			//case ITEM_TYPE::ITEM_WEAPON:
			//	CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(); //무기타입을 받아서 해당아이템이 추가되게끔 -> 이제WPN_PARAM 구조체로 받아야!!
			//	break;
			//case ITEM_TYPE::ITEM_PET:
			//	CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(m_ePetType);
			//	break;
			//}

			//g_pManagement->Create_Effect(L"Bullet_DeadFlash", m_pTransform->Get_Pos(), nullptr);
			//g_pManagement->Create_Effect(L"Bullet_DeadSmoke_Base", m_pTransform->Get_Pos(), nullptr);
			//g_pManagement->Create_Effect(L"Bullet_DeadSmoke_Black", m_pTransform->Get_Pos(), nullptr);
			m_bEnable = false;

			return DEAD_OBJ;
		}
	}

	if (true == m_bCheck_Start_GetItemUI)
	{
		// 부딪힌 상태면 m_bCanGetItem이 true가 된다.
		m_pGet_ItemUI->Update_GameObject(TimeDelta);

		// 이미지가 바뀌었을 때 m_bCanGetItem = true;
	}

	return NO_EVENT;
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

	return NO_EVENT;
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

	//// For.Com_NaviMesh
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CDropItem::Ready_Status(void* _pArg)
{
	if (nullptr == _pArg)
	{
		m_eItemType = ITEM_TYPE::ITEM_MATERIAL;
		m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_NORMAL;

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
	IF_NOT_NULL(m_pEffect)
		m_pEffect->Set_Dead();

	IF_NOT_NULL(m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}
