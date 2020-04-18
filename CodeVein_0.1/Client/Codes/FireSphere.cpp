#include "stdafx.h"
#include "..\Headers\FireSphere.h"
#include "ParticleMgr.h"
#include "Effect.h"

CFireSphere::CFireSphere(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireSphere::CFireSphere(const CFireSphere & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireSphere::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFireSphere::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = temp.vDir;
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;
	
	m_fMaxDegree = 10.f;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"FireBoy_FireSphere_BodyFire", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pBulletBody->Set_Loop(true);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	m_pBulletBody_Sub = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"FireBoy_FireSphere_BodyFire_Sub", nullptr));
	m_pBulletBody_Sub->Set_Desc(m_vDir * 0.5f, m_pTransformCom);
	m_pBulletBody_Sub->Set_Loop(true);
	m_pBulletBody_Sub->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody_Sub, SCENE_STAGE, L"Layer_Effect", nullptr);

	return NOERROR;
}

_int CFireSphere::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();

	//Look_At_Target(TimeDelta);	// 보류

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		g_pManagement->Create_Effect(L"FireBoy_FireSphere_BreakFire", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"FireBoy_FireSphere_BreakParticle", m_pTransformCom->Get_Pos(), nullptr);

		m_pBulletBody->Set_Dead();
		m_pBulletBody_Sub->Set_Dead();

		m_bDead = true;
	}
	// 진행중
	else
	{
		m_fEffectOffset += (_float)TimeDelta;
		if (m_fEffectOffset > 0.01f)
		{
			m_fEffectOffset = 0.f;
			
			//g_pManagement->Create_Effect(L"FireBoy_FireSphere_BodyFire", V3_NULL, m_pTransformCom);
			g_pManagement->Create_Effect(L"FireBoy_FireSphere_Tail", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransformCom->Get_Pos(), nullptr);
		}
	}

	return NOERROR;
}

_int CFireSphere::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFireSphere::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireSphere::Update_Collider()
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

void CFireSphere::OnCollisionEnter()
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));


	// =============================================================================================

}

void CFireSphere::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

					if (false == iter->Get_Target_IsDodge())
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

void CFireSphere::Look_At_Target(_double dTimeDelta)
{
	//  잠시 보류


	//_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();


	//_v3 vOriginDir = *(_v3*)(&m_pTransformCom->Get_WorldMat().m[2]);
	//vOriginDir.y = 0.f;
	//D3DXVec3Normalize(&vOriginDir, &vOriginDir);

	//_v3 vToTarget = vPlayer_Pos - m_pTransformCom->Get_Pos();
	//D3DXVec3Normalize(&vToTarget, &vToTarget);

	//float fTemp = D3DXVec3Dot(&vOriginDir, &vToTarget);
	////1.0보다 커짐 방지
	//if (fTemp > 1.0f)
	//	fTemp = 0.999f;

	//_float fRadian = acosf(fTemp);

	//_v3 vRight;
	//D3DXVec3Normalize(&vRight, (_v3*)(&m_pTransformCom->Get_WorldMat().m[0]));

	//_float fTempDir;

	//if (0 < D3DXVec3Dot(&vRight, &vToTarget))
	//{
	//	if (D3DXToDegree(fRadian) > m_fMaxDegree)
	//		fTempDir = -m_fMaxDegree;
	//	else
	//		fTempDir = -D3DXToDegree(fRadian);

	//}
	//else
	//{
	//	if (180 - D3DXToDegree(fRadian) > m_fMaxDegree)
	//		fTempDir = m_fMaxDegree;
	//	else
	//		fTempDir = 180 - D3DXToDegree(fRadian);
	//}

	//D3DXVec3TransformNormal(&m_vDir, &m_vDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(_float(fTempDir * dTimeDelta))));
}

HRESULT CFireSphere::Add_Component()
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

HRESULT CFireSphere::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CFireSphere::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CFireSphere * CFireSphere::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireSphere* pInstance = new CFireSphere(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireSphere::Clone_GameObject(void * pArg)
{
	CFireSphere* pInstance = new CFireSphere(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireSphere::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
