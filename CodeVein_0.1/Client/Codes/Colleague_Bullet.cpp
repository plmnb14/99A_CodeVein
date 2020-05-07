#include "stdafx.h"
#include "Colleague_Bullet.h"

CColleague_Bullet::CColleague_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CColleague_Bullet::CColleague_Bullet(const CColleague_Bullet& rhs)
	: CGameObject(rhs)
{
}

HRESULT CColleague_Bullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CColleague_Bullet::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = temp.vDir;
	m_fBullet_Speed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	//m_tObjParam.fDamage = 200.f;


	//m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Bullet_Body", nullptr));
	//m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	//m_pBulletBody->Reset_Init();
	//g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	//m_pTrailEffect = g_pManagement->Create_Trail();
	//m_pTrailEffect->Set_TrailIdx(5); // Red Tail

	return S_OK;
}

_int CColleague_Bullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	Enter_Collision();
	Update_Trails(TimeDelta);

	m_pTransformCom->Add_Pos(m_fBullet_Speed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트
		//m_pBulletBody->Set_Dead();
		//m_pTrailEffect->Set_Dead();

		CParticleMgr::Get_Instance()->Create_Effect(L"Colleague_Skill_PinkSmoke_0", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(L"Colleague_Skill_PinkSmoke_1", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(L"Colleague_Skill_Blood", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect_Delay(L"Colleague_Skill_BloodSmoke_0", 0.3f, m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect_Delay(L"Colleague_Skill_BloodSmoke_1", 0.3f, m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect_Delay(L"Colleague_Skill_DistortionSmoke", 0.5f, m_pTransformCom->Get_Pos());

		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}
	}

	return S_OK;
}

_int CColleague_Bullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CColleague_Bullet::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CColleague_Bullet::Update_Collider()
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

void CColleague_Bullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CColleague_Bullet::Enter_Collision()
{
	Update_Collider();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));

	return;
}

void CColleague_Bullet::Update_Trails(_double TimeDelta)
{
	_mat matWorld = m_pTransformCom->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransformCom->Get_WorldMat()._21, sizeof(_v3));

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * -0.05f, vBegin + vDir * 0.05f);
		// m_pTrailEffect->Update_GameObject(TimeDelta);
	}

	return;
}

void CColleague_Bullet::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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

					m_dCurTime = 100;

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

HRESULT CColleague_Bullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CColleague_Bullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CColleague_Bullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.3f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CColleague_Bullet* CColleague_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CColleague_Bullet* pInstance = new CColleague_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CColleague_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CColleague_Bullet::Clone_GameObject(void * pArg)
{
	CColleague_Bullet* pInstance = new CColleague_Bullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CColleague_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CColleague_Bullet::Free()
{
	//Safe_Release(m_pTrailEffect);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}

