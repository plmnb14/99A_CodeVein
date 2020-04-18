#include "stdafx.h"
#include "..\Headers\PoisonRotationBullet.h"
#include "ParticleMgr.h"

CPoisonRotationBullet::CPoisonRotationBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoisonRotationBullet::CPoisonRotationBullet(const CPoisonRotationBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoisonRotationBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonRotationBullet::Ready_GameObject(void * pArg)
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

	m_pBulletTransformCom1 = CTransform::Create(m_pGraphic_Dev);
	m_pBulletTransformCom2 = CTransform::Create(m_pGraphic_Dev);

	m_pBulletBody1 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"ButterFly_VenomShot_Body", nullptr));
	m_pBulletBody1->Set_Desc(_v3(0, 0, 0), m_pBulletTransformCom1);
	m_pBulletBody1->Set_Loop(true);
	m_pBulletBody1->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody1, SCENE_STAGE, L"Layer_Effect", nullptr);

	m_pBulletBody2 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"ButterFly_VenomShot_Body", nullptr));
	m_pBulletBody2->Set_Desc(_v3(0, 0, 0), m_pBulletTransformCom2);
	m_pBulletBody2->Set_Loop(true);
	m_pBulletBody2->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody2, SCENE_STAGE, L"Layer_Effect", nullptr);

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

_int CPoisonRotationBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta,  m_vDir);	// 중심점의 Transform
	m_vDir += _v3(0.f, 0.3f, 0.f) * _float(TimeDelta);

	Update_Bullet_Pos(TimeDelta);	// 이펙트용 총알 업데이트

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag7	, m_pBulletTransformCom1->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag8	, m_pBulletTransformCom1->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag9	, m_pBulletTransformCom1->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag10	, m_pBulletTransformCom1->Get_Pos(), nullptr);
		
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag7	, m_pBulletTransformCom2->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag8	, m_pBulletTransformCom2->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag9	, m_pBulletTransformCom2->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag10	, m_pBulletTransformCom2->Get_Pos(), nullptr);

		m_pBulletBody1->Set_Dead();
		m_pBulletBody2->Set_Dead();
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
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag2, m_pBulletTransformCom1->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag3, m_pBulletTransformCom1->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag4, m_pBulletTransformCom1->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag5, m_pBulletTransformCom1->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag0, m_pBulletTransformCom1->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag1, m_pBulletTransformCom1->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag6, m_pBulletTransformCom1->Get_Pos(), nullptr);

			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag2, m_pBulletTransformCom2->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag3, m_pBulletTransformCom2->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag4, m_pBulletTransformCom2->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag5, m_pBulletTransformCom2->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag0, m_pBulletTransformCom2->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag1, m_pBulletTransformCom2->Get_Pos(), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag6, m_pBulletTransformCom2->Get_Pos(), nullptr);

		}
	}


	return NOERROR;
}

_int CPoisonRotationBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPoisonRotationBullet::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CPoisonRotationBullet::Update_Collider()
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

HRESULT CPoisonRotationBullet::Update_Bullet_Pos(_double TimeDelta)
{
	_mat matPos1, matRotParent1, matResult1;
	D3DXMatrixIdentity(&matPos1);
	D3DXMatrixIdentity(&matRotParent1);
	D3DXMatrixIdentity(&matResult1);

	D3DXMatrixTranslation(&matPos1, 0.f, 0.85f, 0.f);
	D3DXMatrixRotationAxis(&matRotParent1, &m_vDir, m_fAngle);
	//D3DXMatrixRotationZ(&matRotParent1, m_fAngle);

	matResult1 = matPos1 * matRotParent1 * m_pTransformCom->Get_WorldMat();

	m_pBulletTransformCom1->Set_WorldMat(matResult1);
	m_pBulletTransformCom1->Set_Pos(*(_v3*)matResult1.m[3]);

	_mat matPos2, matRotXParent2, matRotParent2, matResult2;
	D3DXMatrixIdentity(&matPos2);
	D3DXMatrixIdentity(&matRotParent2);
	D3DXMatrixIdentity(&matResult2);

	D3DXMatrixTranslation(&matPos2, 0.f, 0.85f, 0.f);
	D3DXMatrixRotationAxis(&matRotParent2, &m_vDir, m_fAngle + 180);
	//D3DXMatrixRotationZ(&matRotParent2, m_fAngle + 180);

	matResult2 = matPos2 * matRotParent2 * m_pTransformCom->Get_WorldMat();

	m_pBulletTransformCom2->Set_WorldMat(matResult2);
	m_pBulletTransformCom2->Set_Pos(*(_v3*)matResult2.m[3]);

	m_fAngle += 0.5;

	return S_OK;
}

HRESULT CPoisonRotationBullet::Draw_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

void CPoisonRotationBullet::OnCollisionEnter()
{
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

void CPoisonRotationBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

					m_dCurTime = 1000;	// 바로 사망시키기 위해서 현재시간 1000줬음

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

HRESULT CPoisonRotationBullet::Add_Component()
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

HRESULT CPoisonRotationBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CPoisonRotationBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.8f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CPoisonRotationBullet * CPoisonRotationBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPoisonRotationBullet* pInstance = new CPoisonRotationBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPoisonRotationBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPoisonRotationBullet::Clone_GameObject(void * pArg)
{
	CPoisonRotationBullet* pInstance = new CPoisonRotationBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPoisonRotationBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPoisonRotationBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBulletTransformCom1);
	Safe_Release(m_pBulletTransformCom2);

	CGameObject::Free();
}
