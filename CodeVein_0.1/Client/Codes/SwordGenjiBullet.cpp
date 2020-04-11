#include "stdafx.h"
#include "..\Headers\SwordGenjiBullet.h"
#include "ParticleMgr.h"

CSwordGenjiBullet::CSwordGenjiBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSwordGenjiBullet::CSwordGenjiBullet(const CSwordGenjiBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordGenjiBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordGenjiBullet::Ready_GameObject(void * pArg)
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

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"RockBullet_Body", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	//lstrcpy(m_pEffect_Tag0, L"Bullet_Body");
	//lstrcpy(m_pEffect_Tag1, L"Bullet_Body_Aura");
	//lstrcpy(m_pEffect_Tag2, L"Bullet_Tail_Particle");

	//lstrcpy(m_pEffect_Tag3, L"Bullet_DeadFlash");
	//lstrcpy(m_pEffect_Tag4, L"Bullet_DeadSmoke_Base");
	//lstrcpy(m_pEffect_Tag5, L"Bullet_DeadSmoke_Black");

	m_pTrailEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pTrailEffect->Set_TrailIdx(4); // Red Tail

	return NOERROR;
}

_int CSwordGenjiBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();
	Update_Trails(TimeDelta);

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트
		//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag3, _v3(), m_pTransformCom);
		//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag4, _v3(), m_pTransformCom);
		//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag5, _v3(), m_pTransformCom);
		m_pBulletBody->Set_Dead();

		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_bEffect)
		{
			//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag0, _v3(), m_pTransformCom);
			//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag1, _v3(), m_pTransformCom);

			m_bEffect = false;
		}

		m_fEffectCreateOffset_Check += _float(TimeDelta);
		if (m_fEffectCreateOffset < m_fEffectCreateOffset_Check)
		{
			m_fEffectCreateOffset_Check = 0.f;

			//CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag2, _v3(), m_pTransformCom);
		}
	
	}


	return NOERROR;
}

_int CSwordGenjiBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSwordGenjiBullet::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CSwordGenjiBullet::Update_Collider()
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

HRESULT CSwordGenjiBullet::Draw_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

void CSwordGenjiBullet::Update_Trails(_double TimeDelta)
{
	_mat matWorld = m_pTransformCom->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransformCom->Get_WorldMat()._21, sizeof(_v3));
	//m_vDir

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * -0.05f, vBegin + vDir * 0.05f);
		m_pTrailEffect->Update_GameObject(TimeDelta);
	}
}

void CSwordGenjiBullet::OnCollisionEnter()
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

void CSwordGenjiBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	//cout << "uykiuyuh" << endl;

	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
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

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}

						if (false == iter->Get_Target_Dodge())
						{
							// 무기 공격력의 +-20%까지 랜덤범위
							_uint min = (_uint)(m_tObjParam.fDamage - (m_tObjParam.fDamage * 0.2f));
							_uint max = (_uint)(m_tObjParam.fDamage + (m_tObjParam.fDamage * 0.2f));

							iter->Add_Target_Hp((_float)-CALC::Random_Num(min, max));
							g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

							m_dCurTime = 100;
						}
					}

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

HRESULT CSwordGenjiBullet::Add_Component()
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

HRESULT CSwordGenjiBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CSwordGenjiBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
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

CSwordGenjiBullet * CSwordGenjiBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSwordGenjiBullet* pInstance = new CSwordGenjiBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CSwordGenjiBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwordGenjiBullet::Clone_GameObject(void * pArg)
{
	CSwordGenjiBullet* pInstance = new CSwordGenjiBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CSwordGenjiBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordGenjiBullet::Free()
{
	Safe_Release(m_pTrailEffect);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
