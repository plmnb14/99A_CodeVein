#include "stdafx.h"
#include "..\Headers\Pet_Bullet.h"

CPet_Bullet::CPet_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPet(pGraphic_Device)
{
}

CPet_Bullet::CPet_Bullet(const CPet_Bullet & rhs)
	: CPet(rhs)
{
}

HRESULT CPet_Bullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPet_Bullet::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}
	else
	{
		Ready_Effect(pArg);

		return S_OK;
	}
}

_int CPet_Bullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	Enter_Collision();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dLifeTime)
	{
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_0", m_pTransformCom->Get_Pos());
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_1", m_pTransformCom->Get_Pos());
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_Particle", m_pTransformCom->Get_Pos());
		m_pBulletBody->Set_Dead();

		m_bDead = true;
	}
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}

		m_fEffectOffset += (_float)TimeDelta;
		if (m_fEffectOffset > 0.1f)
		{
			m_fEffectOffset = 0.f;
			switch (m_eBulletType)
			{
				//임시로 토템총알을 복붙했습니다
			case PET_BULLET_POISON:
				m_fEffectOffset = 0.f;
				g_pManagement->Create_Effect(L"Totem_Fire_BulletBody", m_pTransformCom->Get_Pos() + m_vDir * 1.3f);
				g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransformCom->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransformCom->Get_Pos(), nullptr);
				break;
			case PET_BULLET_ICE:
				break;
			case PET_BULLET_FIRE:
				break;
			case PET_BULLET_NONE:
				break;
			default:
				break;
			}
		}
	}

	return NO_EVENT;
}

_int CPet_Bullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CPet_Bullet::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CPet_Bullet::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecAttackCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	return;
}

void CPet_Bullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CPet_Bullet::Enter_Collision()
{
	Update_Collider();
//	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));

	return;
}

void CPet_Bullet::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;

	for (auto& iter : plistGameObject)
	{
		if (false == iter->Get_Target_CanHit())
			continue;

		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				if (vecIter->Check_Sphere(vecCol))
				{
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					iter->Set_Target_CanHit(false);
					iter->Add_Target_Hp(-m_tObjParam.fDamage);

					m_dCurTime = 100;	// 바로 사망시키기 위해서 현재시간 100줬음

					break;

				}
				else
				{
					if (bFirst)
						break;
				}
			}
		}
	}

	return;
}

HRESULT CPet_Bullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Bullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CPet_Bullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.4f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CPet_Bullet::Ready_Effect(void * pArg)
{
	PET_BULLET_STATUS temp = *(PET_BULLET_STATUS*)(pArg);
	switch (temp.eType)
	{
		//임시로 토템총알을 복붙했습니다
	case PET_BULLET_TYPE::PET_BULLET_POISON:
		m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Totem_Fire_BulletBody", nullptr));
		m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
		m_pBulletBody->Reset_Init();
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);
		break;
	case PET_BULLET_TYPE::PET_BULLET_ICE:
		break;
	case PET_BULLET_TYPE::PET_BULLET_FIRE:
		break;
	case PET_BULLET_TYPE::PET_BULLET_NONE:
		break;
	}

	m_eBulletType = temp.eType;

	m_vDir = temp.vDir;
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(V3_ONE);

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 400.f;

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;
	m_fEffectOffset = 0.f;

	return S_OK;
}

CPet_Bullet* CPet_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPet_Bullet* pInstance = new CPet_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Bullet::Clone_GameObject(void * pArg)
{
	CPet_Bullet* pInstance = new CPet_Bullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Bullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}

