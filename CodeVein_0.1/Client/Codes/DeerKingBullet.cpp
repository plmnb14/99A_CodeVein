#include "stdafx.h"
#include "..\Headers\DeerKingBullet.h"
#include "ParticleMgr.h"
#include "Effect.h"

CDeerKingBullet::CDeerKingBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDeerKingBullet::CDeerKingBullet(const CDeerKingBullet & rhs)
	: CMonster(rhs)
{
}

HRESULT CDeerKingBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDeerKingBullet::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;
	m_vDir = temp.vDir;

	m_dCurTime = 0;
	m_bDead = false;
	m_bFire = false;
	m_fEffectOffset = 0.f;
	m_bEffect = false;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"DeerKing_IceBullet_0", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody->Set_ParentObject(this);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);
	

	return NOERROR;
}

_int CDeerKingBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_dCurTime += TimeDelta;

	// 3초 뒤에 발사
	if (3.f < m_dCurTime)
	{
		OnCollisionEnter();

		// Calc Angle
		_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
		V3_NORMAL_SELF(&vRight);
		//_float	fDotX = acosf(D3DXVec3Dot(&_v3{ 0,1,0 }, &m_pTransformCom->Get_Axis(AXIS_Z)));
		_float	fDotY = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
		if (vRight.z > 0)
			fDotY *= -1.f;

		m_pTransformCom->Set_Angle(_v3(0.f, fDotY, 0.f));

		m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

		if (!m_bFire)
		{
			m_bFire = true;
			g_pManagement->Create_Effect(L"IceGirl_Buff_Break_1"					, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceBlock_Particle"						, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"DeerKing_IceBullet_DeadParticle_Stone_0"	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"DeerKing_IceBullet_DeadParticle_0"		, m_pTransformCom->Get_Pos(), nullptr);
		}
	}
	else
	{
		Calc_Dir();
	}


	if (m_dCurTime > m_dLifeTime)
	{
		g_pManagement->Create_Effect(L"DeerKing_IceBullet_DeadParticle_Stone_0", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Up_Small_Particle_0", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceSmoke_Small_0", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceSmoke_Small_1", m_pTransformCom->Get_Pos(), nullptr);

		m_pBulletBody->Set_Dead();
		m_bDead = true;
	}
	else
	{
		m_fEffectOffset += (_float)TimeDelta;

		if (m_fEffectOffset > 0.3f && !m_bFire)
		{
			m_fEffectOffset = 0.f;
			
			g_pManagement->Create_Effect(L"IceSmoke_01"					, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceSmoke_02"					, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_Charge_Hand_Smoke"	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_Charge_Hand_Smoke_2"	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_Charge_Hand_Particle", m_pTransformCom->Get_Pos(), nullptr);


			_int iRand = CCalculater::Random_Num(0, 1);
			if(iRand)
				g_pManagement->Create_Effect(L"DeerKing_IceBullet_ReadySmoke_0", m_pTransformCom->Get_Pos(), nullptr);
			else
				g_pManagement->Create_Effect(L"DeerKing_IceBullet_ReadySmoke_1", m_pTransformCom->Get_Pos(), nullptr);

			g_pManagement->Create_Effect(L"DeerKing_IceBullet_ReadySmoke_2", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"DeerKing_IceBullet_ReadySmoke_3", m_pTransformCom->Get_Pos(), nullptr);

			for (_int i = 0; i < 5; i++)
			{
				g_pManagement->Create_Effect(L"IceGirl_PointParticle_Blue", m_pTransformCom->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"IceGirl_PointParticle_Green", m_pTransformCom->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"IceGirl_FlashParticle_Blue", m_pTransformCom->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"IceGirl_FlashParticle_Green", m_pTransformCom->Get_Pos(), nullptr);
			}
			
		}

		if (!m_bEffect)
		{
			m_bEffect = true;
			g_pManagement->Create_Effect(L"DeerKing_IceBullet_ReadySmoke_0"	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"DeerKing_IceSmoke_Small_0"		, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"DeerKing_IceSmoke_Small_1"		, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_PointParticle_Blue"		, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_PointParticle_Green"		, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_FlashParticle_Blue"		, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_FlashParticle_Green"		, m_pTransformCom->Get_Pos(), nullptr);
		}
	}


	return NOERROR;
}

_int CDeerKingBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDeerKingBullet::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDeerKingBullet::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecAttackCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);
		_v3 LookPos = *D3DXVec3Normalize(&_v3(), &_v3(tmpMat._31, tmpMat._32, tmpMat._33));

		iter->Update(ColPos + LookPos * 2.f);

		++matrixIdx;
	}

	return NOERROR;
}

void CDeerKingBullet::OnCollisionEnter()
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
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));
	}

	// =============================================================================================

}

void CDeerKingBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

						m_dCurTime = 10000;	// 맞으면 제거
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

HRESULT CDeerKingBullet::Calc_Dir()
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL));

	m_vDir = *D3DXVec3Normalize(&_v3(), &(pPlayerTrans->Get_Pos() + _v3(0.f, 0.5f, 0.f) - m_pTransformCom->Get_Pos()));

	return S_OK;
}

HRESULT CDeerKingBullet::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDeerKingBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDeerKingBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return NOERROR;
}

CDeerKingBullet * CDeerKingBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDeerKingBullet* pInstance = new CDeerKingBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDeerKingBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeerKingBullet::Clone_GameObject(void * pArg)
{
	CDeerKingBullet* pInstance = new CDeerKingBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDeerKingBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeerKingBullet::Free()
{

	CMonster::Free();
}
