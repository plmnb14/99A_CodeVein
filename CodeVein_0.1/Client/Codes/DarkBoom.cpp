#include "stdafx.h"
#include "..\Headers\DarkBoom.h"
#include "ParticleMgr.h"
#include "Effect.h"

CDarkBoom::CDarkBoom(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDarkBoom::CDarkBoom(const CDarkBoom & rhs)
	: CMonster(rhs)
{
}

HRESULT CDarkBoom::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDarkBoom::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_dCurTime = 0;
	m_bDead = false;
	m_fEffectOffset = 0.f;

	//g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_Sphere_1", 0.0f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_Particle", 0.0f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_RingReady", 0.05f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_RingReady", 0.15f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_ReadyDistortion", 0.25f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_ReadyDistortion", 0.35f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_ReadyDistortion", 0.45f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_Particle"  , 0.95f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_LaserBody" , 0.94f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_Particle"  , 0.95f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_RingBottom", 0.95f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_RingBottom", 1.f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Delay(L"QueensKnight_DarkBoom_RingBottom", 1.1f, m_pTransformCom->Get_Pos(), nullptr);

	return NOERROR;
}

_int CDarkBoom::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	// 충돌처리는 이펙트 넣고 나서 할 것임.

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		m_bDead = true;
		OnCollisionEnter();
	}
	// 진행중
	else
	{
		m_fEffectOffset += _float(TimeDelta);
		if (m_fEffectOffset > 0.05f)
		{
			m_fEffectOffset = 0.f;

			for (_int i = 0; i < 5; i++)
			{
				_mat matRotY;
				_v3 vDir = _v3(1.f, 0.f, 1.f);
				D3DXMatrixIdentity(&matRotY);

				D3DXMatrixRotationY(&matRotY, _float(D3DXToRadian(CCalculater::Random_Num_Double(0, 360))));
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
				D3DXVec3Normalize(&vDir, &vDir);

				_float fMinRange = 2.f;
				_v3 vRandPos = vDir * _float(CCalculater::Random_Num_Double(1.7, _double(fMinRange)));

				CEffect* pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"QueensKnight_DarkBoom_Floor_0", nullptr));
				pEffect->Set_Desc(m_pTransformCom->Get_Pos() + vRandPos, nullptr);
				pEffect->Set_FinishPos(m_pTransformCom->Get_Pos());
				//pEffect->Set_ZWrite();
				pEffect->Reset_Init();
				g_pManagement->Add_GameOject_ToLayer_NoClone(pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);

				vRandPos = vDir * _float(CCalculater::Random_Num_Double(1.7, _double(fMinRange)));
				pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"QueensKnight_DarkBoom_Floor_1", nullptr));
				pEffect->Set_Desc(m_pTransformCom->Get_Pos() + vRandPos, nullptr);
				pEffect->Set_FinishPos(m_pTransformCom->Get_Pos());
				//pEffect->Set_ZWrite();
				pEffect->Reset_Init();
				g_pManagement->Add_GameOject_ToLayer_NoClone(pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);

			}
		}
	}

	return NOERROR;
}

_int CDarkBoom::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDarkBoom::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDarkBoom::Update_Collider()
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

void CDarkBoom::OnCollisionEnter()
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
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));
	}

	// =============================================================================================

}

void CDarkBoom::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CDarkBoom::Add_Component()
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

HRESULT CDarkBoom::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDarkBoom::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.4f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CDarkBoom * CDarkBoom::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDarkBoom* pInstance = new CDarkBoom(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDarkBoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDarkBoom::Clone_GameObject(void * pArg)
{
	CDarkBoom* pInstance = new CDarkBoom(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDarkBoom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDarkBoom::Free()
{

	CMonster::Free();
}
