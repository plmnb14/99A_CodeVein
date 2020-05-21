#include "stdafx.h"
#include "..\Headers\FireGround.h"
#include "ParticleMgr.h"
#include "Effect.h"

CFireGround::CFireGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireGround::CFireGround(const CFireGround & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireGround::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFireGround::Ready_GameObject(void * pArg)
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

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffectReadyFireOn = false;
	m_bEffectFloorOn = false;
	m_bStartBoom = false;
	m_fEffectOffset = 0.f;

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 250.f * pow(1.5f, g_sStageIdx_Cur - 1);

	return NOERROR;
}

_int CFireGround::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	
	m_dTimeDelta = TimeDelta;
	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{

		

		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_dCurTime > 0.01f && !m_bEffectReadyFireOn)
		{
			for (_int i = 0; i < 14; i++)
			{
				_mat matRotY;
				_v3 vDir = _v3(1.f, 0.f, 1.f);
				D3DXMatrixIdentity(&matRotY);

				D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(25.7 * i)));
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
				D3DXVec3Normalize(&vDir, &vDir);

				_float fMinRange = 2.f;
				_v3 vRandPos = vDir * (fMinRange);

				CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_ReadyFire", i * 0.03f, m_pTransformCom->Get_Pos() + vRandPos + _v3(0.f, 0.45f, 0.f), nullptr);
			}
			m_bEffectReadyFireOn = true;
		}

		if (m_dCurTime > 0.25f && !m_bEffectFloorOn)
		{
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_Floor", m_pTransformCom->Get_Pos() + _v3(0.f, 0.2f, 0.f), nullptr);
			m_bEffectFloorOn = true;
		}

		// 생성 후 0.75초 뒤 폭발 시작
		if (m_dCurTime > 0.75f && !m_bStartBoom)
		{
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_BoomCircle", m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_BoomParticle_01", m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_BoomParticle_02", m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_BoomFire", 0.1f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			m_bStartBoom = true;
		}

		m_fEffectOffset += _float(TimeDelta);
		if (m_bStartBoom && m_fEffectOffset > 0.6f)
		{
			m_fEffectOffset = 0.f;
			for (_int i = 0; i < 10; i++)
			{
				_mat matRotY;
				_v3 vDir = _v3(1.f, 0.f, 1.f);
				D3DXMatrixIdentity(&matRotY);

				D3DXMatrixRotationY(&matRotY, _float(D3DXToRadian(CCalculater::Random_Num_Double(0, 360))));
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
				D3DXVec3Normalize(&vDir, &vDir);

				_float fMinRange = 2.f;
				_v3 vRandPos = vDir * _float(CCalculater::Random_Num_Double(1.7, _double(fMinRange)));
				
				CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_AfterFire_01", _float(CCalculater::Random_Num_Double(0, 1.0)), m_pTransformCom->Get_Pos() + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0.6, 0.7)), 0.f), nullptr);
				CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_AfterFire_02", _float(CCalculater::Random_Num_Double(0, 1.0)), m_pTransformCom->Get_Pos() + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0.6, 0.7)), 0.f), nullptr);
				CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_Particle",m_pTransformCom->Get_Pos(), nullptr);
			}
		}
	}

	//최초로 바닥에 불을 생성한 후  일정시간 뒤 불덩어리가 터지면서 순간적으로 충돌처리

	// 불 폭발 시작
	//if (false == m_bFinishBoom && true == m_bStartBoom)
	if (true == m_bStartBoom)
	{
		m_dCurBoomTime += TimeDelta;

		if (false == m_bFinishCol)	//충돌하면 true로 바뀜
		{
			OnCollisionEnter();
			m_bFireTickDmg = true;	// 장판 충돌 On
		}

		// 불 폭발 시작 후 잠깐만 콜라이더 On 시킴
		if (m_dCurBoomTime > 0.2f)
		{
			m_bFinishCol = true;	// 충돌처리 End
		}
	}

	// 폭발 후 장판 틱 데미지
	if (true == m_bFireTickDmg)
	{
		OnCollisionEnter();
	}


	return NOERROR;
}

_int CFireGround::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFireGround::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireGround::Update_Collider()
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

void CFireGround::OnCollisionEnter()
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

void CFireGround::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

					// 폭발 충돌
					if (false == m_bFireTickDmg)
					{
						if (false == iter->Get_Target_IsDodge())
						{
							iter->Set_Target_CanHit(false);

							// 타겟이 피격 가능하다면
							if (iter->Get_Target_IsHit())
								iter->Set_HitAgain(true);

							iter->Hit_Target(m_tObjParam.fDamage);
						}

						m_bFinishCol = true;	// 충돌 완료

						break;
					}
					// 장판 충돌
					else
					{
						iter->Hit_Target(m_tObjParam.fDamage * _float(m_dTimeDelta));
					}
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

HRESULT CFireGround::Add_Component()
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

HRESULT CFireGround::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CFireGround::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 2.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CFireGround * CFireGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireGround* pInstance = new CFireGround(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireGround::Clone_GameObject(void * pArg)
{
	CFireGround* pInstance = new CFireGround(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireGround::Free()
{

	CMonster::Free();
}
