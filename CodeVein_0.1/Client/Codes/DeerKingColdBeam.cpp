#include "stdafx.h"
#include "..\Headers\DeerKingColdBeam.h"
#include "ParticleMgr.h"
#include "Effect.h"

CDeerKingColdBeam::CDeerKingColdBeam(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDeerKingColdBeam::CDeerKingColdBeam(const CDeerKingColdBeam & rhs)
	: CMonster(rhs)
{
}

HRESULT CDeerKingColdBeam::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDeerKingColdBeam::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = temp.vDir;
	m_dLifeTime = 1;

	m_dCurTime = 0;
	m_bDead = false;
	m_fEffectOffset = 0.f;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;


	m_vDir += _v3(0.f, _float(CCalculater::Random_Num_Double(0.3, 0.45)), 0.f);

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;
	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_pEffect  = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"DeerKing_IceBlock_0", nullptr));
	//m_pEffect->Set_Desc(_v3(0, -1.5f, 0.f) + m_pTransformCom->Get_Axis(AXIS_Z) * - 2.f, nullptr);
	m_pEffect->Set_Desc(_v3(0.f, 0.f, 0.f), nullptr);
	m_pEffect->Set_ParentObject(this);
	m_pEffect->Set_ZWrite();
	m_pEffect->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);

	for (_int i = 0; i < 5; i++)
		CParticleMgr::Get_Instance()->Create_Effect_Delay(L"DeerKing_Snow_Up_LongLine_1", 0.15f, m_pTransformCom->Get_Pos() + _v3(0.f, 1.f, 0.f) + m_vDir * 0.3f, nullptr);

	return NOERROR;
}

_int CDeerKingColdBeam::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
	{
		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::DearKing_SFX_02);
		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_BOSSGUY_ICE_ATTACK_011.ogg"), CSoundManager::CHANNELID::DearKing_SFX_02, CSoundManager::SOUND::Effect_Sound);

		return DEAD_OBJ;
	}

	m_dCurTime += TimeDelta;



	if (m_dCurTime > m_dLifeTime)
	{
		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_01", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_02", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_03", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Bubble_BreakSmoke", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Bubble_BreakSmoke", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Bubble_BreakSmoke", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransformCom->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);

		m_pEffect->Set_Dead();
		m_bDead = true;
	}
	else
	{
		if (m_dCurTime < 0.1f)
			m_pTransformCom->Add_Pos(_float(20.f * TimeDelta), m_vDir);

		m_fEffectOffset += _float(TimeDelta);
		if (m_fEffectOffset > 0.3f)
		{
			m_fEffectOffset = 0.f;

			//CParticleMgr::Get_Instance()->Create_Effect(L"IceSmoke_01", m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), nullptr);
			//CParticleMgr::Get_Instance()->Create_Effect(L"IceSmoke_02", m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), nullptr);
		}
	}

	if (m_dCurTime <= 0.3f)
		OnCollisionEnter();

	return NOERROR;
}

_int CDeerKingColdBeam::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDeerKingColdBeam::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDeerKingColdBeam::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecAttackCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);
		_v3 LookPos = *D3DXVec3Normalize(&_v3(), &_v3(tmpMat._31, tmpMat._32, tmpMat._33));
		
		iter->Update(ColPos + LookPos * 3.f);

		++matrixIdx;
	}

	return NOERROR;
}

void CDeerKingColdBeam::OnCollisionEnter()
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

void CDeerKingColdBeam::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CDeerKingColdBeam::Add_Component()
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

HRESULT CDeerKingColdBeam::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDeerKingColdBeam::Ready_Collider()
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

	return NOERROR;
}

CDeerKingColdBeam * CDeerKingColdBeam::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDeerKingColdBeam* pInstance = new CDeerKingColdBeam(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDeerKingColdBeam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeerKingColdBeam::Clone_GameObject(void * pArg)
{
	CDeerKingColdBeam* pInstance = new CDeerKingColdBeam(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDeerKingColdBeam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeerKingColdBeam::Free()
{

	CMonster::Free();
}
