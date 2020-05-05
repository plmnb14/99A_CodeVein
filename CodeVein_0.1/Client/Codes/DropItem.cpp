#include "stdafx.h"
#include "..\Headers\DropItem.h"

CDropItem::CDropItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CItem(pGraphic_Device)
{
}

CDropItem::CDropItem(const CDropItem & rhs)
	:CItem(rhs)
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

	m_pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Totem_Fire_BulletBody", nullptr));
	m_pEffect->Set_Desc(V3_NULL, m_pTransform);
	m_pEffect->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pEffect, SCENE_STAGE, L"Layer_Item", nullptr);

	return S_OK;
}

_int CDropItem::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_dCanGetItemLimitTimeCur += TimeDelta;
	m_fEffectLimitTime += (_float)TimeDelta;

	//상호작용 대상과 충돌 여부 체크
	//Check_PosY();
	Check_Dist();

	if (true == m_bCanGetItem)
		cout << "충돌로 인한 아이템 얻기 가능" << endl;

	if (m_fEffectLimitTime > 0.05f)
	{
		m_fEffectLimitTime = 0.f;
		g_pManagement->Create_Effect(L"Totem_Fire_BulletBody", m_pTransform->Get_Pos());
		g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransform->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransform->Get_Pos(), nullptr);
	}

	//1.제한 시간 초과
	if (m_dCanGetItemLimitTimeMax <= m_dCanGetItemLimitTimeCur)
	{
		//단순 소멸
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_0", m_pTransform->Get_Pos());
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_1", m_pTransform->Get_Pos());
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_Particle", m_pTransform->Get_Pos());
		m_pEffect->Set_Dead();
		m_bEnable = false;
		return DEAD_OBJ;
	}
	//2.상호작용 대상이 획득
	//획득모션,제한시간 이내인 경우
	if (true == m_bCanGetItem && m_dCanGetItemLimitTimeMax >= m_dCanGetItemLimitTimeCur)
	{
		//인벤으로 저장
		g_pManagement->Create_Effect(L"Bullet_DeadFlash", m_pTransform->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"Bullet_DeadSmoke_Base", m_pTransform->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"Bullet_DeadSmoke_Black", m_pTransform->Get_Pos(), nullptr);
		m_bEnable = false;
		return DEAD_OBJ;
	}

	return NO_EVENT;
}

_int CDropItem::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CDropItem::Render_GameObject()
{
	return S_OK;
}

void CDropItem::Check_Dist()
{
	_float fDist;
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
			m_bCanGetItem = true;
			return;
		}
		else
			m_bCanGetItem = false;
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

HRESULT CDropItem::Add_Component(void* _pArg)
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	//// For.Com_NaviMesh
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CDropItem::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CDropItem::Ready_Status(void * _pArg)
{
	if (nullptr == _pArg)
	{
		m_eItemType = ITEM_TYPE::ITEM_MATERIAL;
		m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_NORMAL;
		m_pTransform->Set_Pos(_v3{ 1.f, 0.f, 1.f });
		m_dCanGetItemLimitTimeMax = 5;
	}
	else
	{
		ITEM_STATUS info = *(ITEM_STATUS*)_pArg;
		m_eItemType = info.eItemType;
		m_eItemGrade = info.eItemGradeType;
		m_dCanGetItemLimitTimeMax = info.dCanGetLimitTimeMax;
		m_pTransform->Set_Pos(info.vBirthPos);
	}

	m_dCanGetItemLimitTimeCur = 0;
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

HRESULT CDropItem::Ready_Collider()
{
	m_vecPhysicCol.reserve(1);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.6f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransform->Get_WorldMat().m[3][0], m_pTransform->Get_WorldMat().m[3][1], m_pTransform->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecPhysicCol.push_back(pCollider);

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
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);

	CItem::Free();

	return;
}
