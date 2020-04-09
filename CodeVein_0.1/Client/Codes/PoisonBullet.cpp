#include "stdafx.h"
#include "..\Headers\PoisonBullet.h"
#include "ParticleMgr.h"

CPoisonBullet::CPoisonBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoisonBullet::CPoisonBullet(const CPoisonBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoisonBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonBullet::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = temp.vDir;
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;
	
	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_fEffectCreateOffset = 0.05f;

	lstrcpy(m_pEffect_Tag0, L"ButterFly_VenomShot_Distortion");
	lstrcpy(m_pEffect_Tag1, L"ButterFly_SoftSmoke_Mist");
	lstrcpy(m_pEffect_Tag2, L"ButterFly_VenomShot");
	lstrcpy(m_pEffect_Tag3, L"ButterFly_VenomShot_SubSmoke");
	lstrcpy(m_pEffect_Tag4, L"ButterFly_VenomShot_PointParticle");
	lstrcpy(m_pEffect_Tag5, L"ButterFly_VenomShot_Chunk");
	lstrcpy(m_pEffect_Tag6, L"ButterFly_VenomShot_Tail");

	lstrcpy(m_pEffect_Tag7, L"ButterFly_VenomShot_DeadSplash");
	lstrcpy(m_pEffect_Tag8, L"ButterFly_VenomShot_DeadMist");
	lstrcpy(m_pEffect_Tag9, L"ButterFly_VenomShot_DeadSmoke");
	lstrcpy(m_pEffect_Tag10, L"ButterFly_PointParticle");

	
	return NOERROR;
}

_int CPoisonBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	if (m_pTransformCom->Get_Pos().y <= 0.f)
		m_dCurTime = 1000;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag7, m_pTransformCom->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag8, m_pTransformCom->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag9, m_pTransformCom->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag10, m_pTransformCom->Get_Pos(), nullptr);
		
		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}

		m_fEffectCreateOffset_Check += _float(TimeDelta);
		if (m_fEffectCreateOffset < m_fEffectCreateOffset_Check)
		{
			m_fEffectCreateOffset_Check = 0.f;
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag2, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag3, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag4, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag5, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag0, m_pTransformCom->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag1, m_pTransformCom->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag6, m_pTransformCom->Get_Pos(), nullptr);
		}
	}


	return NOERROR;
}

_int CPoisonBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPoisonBullet::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CPoisonBullet::Update_Collider()
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

	return S_OK;
}

HRESULT CPoisonBullet::Draw_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

void CPoisonBullet::OnCollisionEnter()
{
	Update_Collider();

	// =============================================================================================
	// 충돌
	// =============================================================================================

	if (m_bPlayerFriendly)
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_MonsterProjectile", SCENE_STAGE));
	}
	else
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));


	// =============================================================================================

}

void CPoisonBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_Dodge())
					{
						iter->Set_Target_CanHit(false);

						// 타겟이 피격 가능하다면
						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						iter->Add_Target_Hp(-m_tObjParam.fDamage);
					}

					m_dCurTime = 1000;	// 바로 사망시키기 위해서 현재시간 100줬음

					break;

				}

				else
				{
					if (bFirst)
					{
						break;
					}
				}
			}
		}
	}
}

HRESULT CPoisonBullet::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPoisonBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CPoisonBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.7f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CPoisonBullet * CPoisonBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPoisonBullet* pInstance = new CPoisonBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPoisonBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPoisonBullet::Clone_GameObject(void * pArg)
{
	CPoisonBullet* pInstance = new CPoisonBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPoisonBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPoisonBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
