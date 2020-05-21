#include "stdafx.h"
#include "..\Headers\DeerKingIceSword.h"
#include "ParticleMgr.h"
#include "Effect.h"

CDeerKingIceSword::CDeerKingIceSword(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDeerKingIceSword::CDeerKingIceSword(const CDeerKingIceSword & rhs)
	: CMonster(rhs)
{
}

HRESULT CDeerKingIceSword::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDeerKingIceSword::Ready_GameObject(void * pArg)
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
	m_bDead = false;

	// Transform
	m_pTransformCom->Set_Pos(V3_NULL);
	m_pTransformCom->Set_Scale(V3_ONE);
	m_vAngle = V3_NULL;
	//m_vAngle.x = 30.f;
	//m_vAngle.z = -90.f;

	// 사슴왕 블랙보드에서 가져옴.
	m_vDir = m_pTarget_AIController->Get_V3Value(L"IceSword_Dir");

	//_mat matAttatch = m_pTarget_AIController->Get_MatValue(L"Bone_LeftHandAttach");
	//_mat matParent = m_pTarget_AIController->Get_MatValue(L"Self_Mat");
	//m_pTransformCom->Calc_ParentMat(&(matAttatch * matParent));

	//m_pTransformCom->Set_Pos(m_pTarget_AIController->Get_V3Value(L"IceSword_Pos"));
	//m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 250.f * pow(1.5f, g_sStageIdx_Cur - 1);

	m_pSwordEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"DeerKing_IceSword", nullptr));
	m_pSwordEffect->Set_Desc(V3_NULL, nullptr);
	m_pSwordEffect->Set_Angle(_v3(0.f, 0.f, 90.f));
	m_pSwordEffect->Set_ParentObject(this);
	m_pSwordEffect->Set_ZWrite();
	m_pSwordEffect->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pSwordEffect, SCENE_STAGE, L"Layer_Effect", nullptr);

	return NOERROR;
}

_int CDeerKingIceSword::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_dCurTime += TimeDelta;

	m_vDir = m_pTarget_AIController->Get_V3Value(L"IceSword_Dir");
	//m_pTransformCom->Set_Pos(m_pTarget_AIController->Get_V3Value(L"IceSword_Pos"));

	_v3 vSelfDir = m_pTarget_AIController->Get_V3Value(L"Self_Dir");
	
	_mat matAttatch = m_pTarget_AIController->Get_MatValue(L"Mat_LeftHandAttach");
	_mat matParent = m_pTarget_AIController->Get_MatValue(L"Self_Mat");
	_mat matLast = (matAttatch * matParent);
	_v3 vPos = _v3(matLast._41, matLast._42, matLast._43);
	m_pTransformCom->Calc_ParentMat(&(matLast));

	if (m_dCurTime > 0.5f)
	{
	//	m_vAngle.x -= _float(TimeDelta) * 130.f;
	}

	m_pTransformCom->Set_Angle(AXIS_X, D3DXToRadian(m_vAngle.x));
	m_pTransformCom->Set_Angle(AXIS_Y, D3DXToRadian(m_vAngle.y));
	m_pTransformCom->Set_Angle(AXIS_Z, D3DXToRadian(m_vAngle.z));

	if (m_dCurTime > m_dLifeTime)
	{
		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::DearKing_SFX_02);
		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_BOSSGUY_ICE_ATTACK_011.ogg"), CSoundManager::CHANNELID::DearKing_SFX_02, CSoundManager::SOUND::Effect_Sound);

		m_bDead = true;
		m_pSwordEffect->Set_Dead();

		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_01",vPos+ m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_02",vPos+ m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_03",vPos+ m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_01",vPos + m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_01",vPos + m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle",vPos + m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", vPos + m_vDir * 0.6f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", vPos + m_vDir * 1.3f, nullptr);
		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", vPos + m_vDir * 1.8f, nullptr);
	}
	else
	{
		if (m_dCurTime > 0.1f && m_bEffect)
		{
			m_bEffect = true;

			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_01",vPos + m_vDir * 1.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_02",vPos + m_vDir * 1.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle",vPos + m_vDir * 1.3f, nullptr);
		}

		m_fEffectOffset += _float(TimeDelta);
		if (m_fEffectOffset > 0.01f)
		{
			m_fEffectOffset = 0.f;

			CParticleMgr::Get_Instance()->Create_Effect(L"IceSmoke_01", vPos + m_vDir * 1.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceSmoke_02", vPos + m_vDir * 1.3f, nullptr);
		}
	}

	// 충돌체 위치는 추후 조정
	OnCollisionEnter();

	return NOERROR;
}

_int CDeerKingIceSword::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDeerKingIceSword::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDeerKingIceSword::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecAttackCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos + m_vDir * 2.7f);

		++matrixIdx;
	}

	return NOERROR;
}

void CDeerKingIceSword::OnCollisionEnter()
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

void CDeerKingIceSword::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

						iter->Hit_Target(m_tObjParam.fDamage);
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

HRESULT CDeerKingIceSword::Add_Component()
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

HRESULT CDeerKingIceSword::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDeerKingIceSword::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.7f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return NOERROR;
}

CDeerKingIceSword * CDeerKingIceSword::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDeerKingIceSword* pInstance = new CDeerKingIceSword(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDeerKingIceSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeerKingIceSword::Clone_GameObject(void * pArg)
{
	CDeerKingIceSword* pInstance = new CDeerKingIceSword(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDeerKingIceSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeerKingIceSword::Free()
{
	Safe_Release(m_pTarget_AIController);

	CMonster::Free();
}
