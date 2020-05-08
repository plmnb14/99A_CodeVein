#include "stdafx.h"
#include "..\Headers\FireHandBall.h"

CFireHandBall::CFireHandBall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireHandBall::CFireHandBall(const CFireHandBall & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireHandBall::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFireHandBall::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	CBT_CreateBuff::BUFF_INFO temp = *(CBT_CreateBuff::BUFF_INFO*)(pArg);

	if (m_pTarget_AIController)
		Safe_Release(m_pTarget_AIController);

	m_pTarget_AIController = temp.pAIController;
	Safe_AddRef(m_pTarget_AIController);

	m_dLifeTime = temp.dLifeTime;

	m_dCurTime = 0;
	m_fEffectOffset = 0.f;
	m_bDead = false;

	m_dLifeTime = temp.dLifeTime;
	
	m_fSpeed = 7.f;

	m_pTransformCom->Set_Pos(m_pTarget_AIController->Get_V3Value(L"Bone_LeftHand"));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	//m_tObjParam.bCanAttack = true;
	//m_tObjParam.fDamage = 20.f;
	
	m_pFireSphere = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"FireBoy_FireHandBall", nullptr));
	m_pFireSphere->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pFireSphere->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pFireSphere, SCENE_STAGE, L"Layer_Effect", nullptr);

	return NOERROR;
}

_int CFireHandBall::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	//OnCollisionEnter();

	// 손에 붙어있음
	if (m_dCurTime < 1.134)
	{
		m_pTransformCom->Set_Pos(m_pTarget_AIController->Get_V3Value(L"Bone_LeftHand"));

		m_vDir = m_pTarget_AIController->Get_V3Value(L"FlameHandBallDir");

		m_fEffectOffset += (_float)TimeDelta;
		if (m_fEffectOffset > 0.01f)
		{
			m_fEffectOffset = 0.f;

			g_pManagement->Create_Effect(L"FireBoy_FireHandBall_BodyFire_Small", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransformCom->Get_Pos(), nullptr);
		}
	}
	// 손에서 떨어짐
	else
	{
		m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta * m_vDir);

		m_fEffectOffset += (_float)TimeDelta;
		if (m_fEffectOffset > 0.01f)
		{
			m_fEffectOffset = 0.f;

			g_pManagement->Create_Effect(L"FireBoy_FireHandBall_BodyFire", m_pTransformCom->Get_Pos() + m_vDir * 1.6f, nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransformCom->Get_Pos(), nullptr);
		}
	}

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		g_pManagement->Create_Effect(L"FireBoy_FireHandBall_Dead_FireExplosion", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"FireBoy_FireHandBall_Dead_Light", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"FireBoy_FireSphere_BreakParticle", m_pTransformCom->Get_Pos(), nullptr);
		
		m_bDead = true;
		m_pFireSphere->Set_Dead();

		// 플레이어 머리에서 불꽃 생성
		CTransform* pPlayerTransCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL));

		if (nullptr != pPlayerTransCom)
		{
			_mat matPlayer = pPlayerTransCom->Get_WorldMat();

			_v3 vPlayerLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[2]);
			_v3 vPlayerPos = *(_v3*)&matPlayer.m[3];

			_v3 vCreate_Pos = vPlayerPos + _v3(0.f, 7.f, 0.f);

			_v3 vDir[16];
			ZeroMemory(vDir, sizeof(_v3) * 16);
			
			vDir[0] = vPlayerLook;

			for (int i = 1; i < 16; ++i)
				vDir[i] = *D3DXVec3TransformNormal(&_v3(), &vPlayerLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(i * 22.5f)));

			for (auto& dir : vDir)
				CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_FireFlame", &BULLET_INFO(vCreate_Pos, dir, 5.f, 3.f));

		}
	}

	return NOERROR;
}

_int CFireHandBall::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFireHandBall::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireHandBall::Update_Collider()
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

HRESULT CFireHandBall::Draw_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

void CFireHandBall::OnCollisionEnter()
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

void CFireHandBall::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

						m_dCurTime = 1000;	// 바로 사망시키기 위해서 현재시간 1000줬음
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

HRESULT CFireHandBall::Add_Component()
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

HRESULT CFireHandBall::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CFireHandBall::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CFireHandBall * CFireHandBall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireHandBall* pInstance = new CFireHandBall(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireHandBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireHandBall::Clone_GameObject(void * pArg)
{
	CFireHandBall* pInstance = new CFireHandBall(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireHandBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireHandBall::Free()
{

	CGameObject::Free();
}
