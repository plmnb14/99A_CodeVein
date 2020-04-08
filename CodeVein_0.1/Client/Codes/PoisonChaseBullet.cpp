#include "stdafx.h"
#include "..\Headers\PoisonChaseBullet.h"
#include "ParticleMgr.h"

CPoisonChaseBullet::CPoisonChaseBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoisonChaseBullet::CPoisonChaseBullet(const CPoisonChaseBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoisonChaseBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonChaseBullet::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = temp.vDir;
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_tObjParam.bCanHit = true;
	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));



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

_int CPoisonChaseBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();
	Check_PhyCollider();

	_v3 vPlayerPos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();
	m_vDir = *D3DXVec3Normalize(&_v3(), &( (vPlayerPos + _v3(0.f, 1.f, 0.f)) - m_pTransformCom->Get_Pos()));

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
		//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag10, m_pTransformCom->Get_Pos(), nullptr);
		CParticleMgr::Get_Instance()->Create_ParticleEffect(m_pEffect_Tag10, 0.5f, m_pTransformCom->Get_Pos(), nullptr);

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

_int CPoisonChaseBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPoisonChaseBullet::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CPoisonChaseBullet::Update_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);
	}


	for (auto& iter : m_vecAttackCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);
	}


	return S_OK;
}

HRESULT CPoisonChaseBullet::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

void CPoisonChaseBullet::Check_PhyCollider()
{
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		
		cout << "추적 총알 사망" << endl;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			m_bDead = false;
		}
		else
		{
			m_dCurTime = 1000;	// 죽음
		}
	}
}

void CPoisonChaseBullet::OnCollisionEnter()
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

void CPoisonChaseBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

					cout << "응 투사체 부딪힘" << endl;

					iter->Set_Target_CanHit(false);
					iter->Add_Target_Hp(m_tObjParam.fDamage);

					m_dCurTime = 100;	// 바로 사망시키기 위해서 현재시간 100줬음

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

HRESULT CPoisonChaseBullet::Add_Component()
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

HRESULT CPoisonChaseBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CPoisonChaseBullet::Ready_Collider()
{
	////////// 피직 콜라이더

	m_vecPhysicCol.reserve(1);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecPhysicCol.push_back(pCollider);



	/////////// 공격 콜라이더

	m_vecAttackCol.reserve(1);

	// 총알 중앙
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.7f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CPoisonChaseBullet * CPoisonChaseBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPoisonChaseBullet* pInstance = new CPoisonChaseBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPoisonChaseBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPoisonChaseBullet::Clone_GameObject(void * pArg)
{
	CPoisonChaseBullet* pInstance = new CPoisonChaseBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPoisonChaseBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPoisonChaseBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
