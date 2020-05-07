#include "stdafx.h"
#include "CollBullet_Heal.h"

#include "Player.h"
#include "Player_Colleague.h"


CCollBullet_Heal::CCollBullet_Heal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCollBullet_Heal::CCollBullet_Heal(const CCollBullet_Heal & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCollBullet_Heal::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCollBullet_Heal::Ready_GameObject(void * pArg)
{
	// 타입에 따른 힐용 총알과 공격용 총알로 나뉜다.
	// 힐총알은 플레이어에게 반원을 그리면서 다가가며, 닿을 시 HP를 회복시킨다.
	// 힐총알은 플레이어에게 닿을 때까지 쫒아간다.
	// 힐총알 속도는 공격총알보다는 빠른 편이어야 함
	// 공격총알은 몬스터에게 일직선으로 향하며 스피드가 다소 느린 편이어야 한다.

	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	m_fBullet_Speed = 3.f;
	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 200.f;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Bullet_Body", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	lstrcpy(m_pEffect_Tag1, L"Bullet_Body_Aura");
	lstrcpy(m_pEffect_Tag2, L"Bullet_Tail_Particle");

	lstrcpy(m_pEffect_Tag3, L"Bullet_DeadFlash");
	lstrcpy(m_pEffect_Tag4, L"Bullet_DeadSmoke_Base");
	lstrcpy(m_pEffect_Tag5, L"Bullet_DeadSmoke_Black");

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(5); // Red Tail

	return S_OK;
}

_int CCollBullet_Heal::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	_float fPlusHP = 0.f;

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if(nullptr != m_pTarget)
		fPlusHP = (m_pTarget->Get_pTarget_Param()->fHp_Cur / 2.f);

	Enter_Collision();
	Update_Trails(TimeDelta);

	_v3 Temp = V3_NULL;
	if (nullptr != pPlayer)
	{
		Temp = TARGET_TO_TRANS(pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos();
		D3DXVec3Normalize(&Temp, &Temp);
	}

	m_pTransformCom->Add_Pos(m_fBullet_Speed * (_float)TimeDelta, Temp);

	m_dCurTime += TimeDelta;

	// 플레이어와 충돌했다면
	if (false == pPlayer->Get_Target_IsHit())
	{
		_float TempHP = pPlayer->Get_Target_Param().fHp_Max / 0.8f;
		pPlayer->Add_Target_Hp(TempHP);
		//죽음 이펙트
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag3, m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag4, m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag5, m_pTransformCom->Get_Pos());
		m_pBulletBody->Set_Dead();
		//m_pTrailEffect->Set_Dead();

		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_bEffect)
		{
			//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag0, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag1, _v3(), m_pTransformCom);

			m_bEffect = false;
		}
		CParticleMgr::Get_Instance()->Create_Effect_Offset(m_pEffect_Tag2, 0.1f, m_pTransformCom->Get_Pos());
	}

	return S_OK;
}

_int CCollBullet_Heal::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollBullet_Heal::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CCollBullet_Heal::Update_Trails(_double TimeDelta)
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
}

void CCollBullet_Heal::Update_Collider()
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
}

void CCollBullet_Heal::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
}

void CCollBullet_Heal::Enter_Collision()
{
	Update_Collider();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
}

void CCollBullet_Heal::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bCanAttack)
		return;

	_float fHealHP = 0.f;

	/*CGameObject* pColleague = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);
	if (nullptr != pColleague)
		fHealHP = (pColleague->Get_Target_Hp() / 30.f);*/

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

					/*if (false == m_pTarget->Get_Target_IsHit())
					{
						iter->Add_Target_Hp(fHealHP);
						return;
					}*/
						

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

HRESULT CCollBullet_Heal::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollBullet_Heal::SetUp_ConstantTable()
{

	return S_OK;
}

HRESULT CCollBullet_Heal::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CCollBullet_Heal* CCollBullet_Heal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollBullet_Heal*	pInstance = new CCollBullet_Heal(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating Colleague Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCollBullet_Heal::Clone_GameObject(void* pArg)
{
	CCollBullet_Heal*	pInstance = new CCollBullet_Heal(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CColleague_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollBullet_Heal::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecAttackCol)
	{
		Safe_Release(iter);
	}

	CGameObject::Free();
}
