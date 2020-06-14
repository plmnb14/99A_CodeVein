#include "stdafx.h"
#include "PlayerSkillBullet.h"
#include "TexEffect.h"

CPlayerSkillBullet::CPlayerSkillBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayerSkillBullet::CPlayerSkillBullet(const CPlayerSkillBullet& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerSkillBullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPlayerSkillBullet::Ready_GameObject(void * pArg)
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
	m_fBullet_Speed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;

	m_fBullet_Delay = 0.6f;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 1300.f;

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(11);


	// ÃÑ¾Ë »ý¼º Á÷Àü ÀÌÆåÆ®
	_v3 vEffectPos = temp.vCreatePos;

	CTexEffect* pEff = static_cast<CTexEffect*>(CManagement::Get_Instance()->Clone_GameObject_Return(L"Player_Skill_Gun_FlashLight", nullptr));
	pEff->Set_Desc(vEffectPos, nullptr);
	pEff->Set_Delay(true, m_fBullet_Delay);
	pEff->Set_LayerIdx(1);
	pEff->Reset_Init();
	CManagement::Get_Instance()->Add_GameOject_ToLayer_NoClone(pEff, SCENE_STAGE, L"Layer_Effect", nullptr);

	pEff = static_cast<CTexEffect*>(CManagement::Get_Instance()->Clone_GameObject_Return(L"Player_Skill_Gun_PurpleLightning", nullptr));
	pEff->Set_Desc(vEffectPos, nullptr);
	pEff->Set_Delay(true, m_fBullet_Delay);
	pEff->Set_LayerIdx(2);
	pEff->Reset_Init();
	CManagement::Get_Instance()->Add_GameOject_ToLayer_NoClone(pEff, SCENE_STAGE, L"Layer_Effect", nullptr);

	pEff = static_cast<CTexEffect*>(CManagement::Get_Instance()->Clone_GameObject_Return(L"Player_Skill_Gun_ShotSmoke", nullptr));
	pEff->Set_Desc(vEffectPos, nullptr);
	pEff->Set_Delay(true, m_fBullet_Delay);
	pEff->Set_LayerIdx(1);
	pEff->Reset_Init();
	CManagement::Get_Instance()->Add_GameOject_ToLayer_NoClone(pEff, SCENE_STAGE, L"Layer_Effect", nullptr);

	g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Smoke_Blue_0", 0.f, vEffectPos, nullptr);
	g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Smoke_Blue_1", 0.f, vEffectPos, nullptr);
	g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Smoke_DarkBlue", 0.f, vEffectPos, nullptr);
	g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Smoke_DarkBlue", 0.2f,vEffectPos, nullptr);
	g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Smoke_DarkBlue", 0.3f,vEffectPos, nullptr);

	return S_OK;
}

_int CPlayerSkillBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	if (m_fBullet_Delay > 0.f)
	{
		m_fBullet_Delay -= (_float)TimeDelta;
		return S_OK;
	}
	else if(!m_pBulletBody_0 && !m_pBulletBody_1)
	{
		m_pBulletBody_0 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Player_Skill_Gun_BulletBody_Hor", nullptr));
		m_pBulletBody_0->Set_Desc(V3_NULL, nullptr);
		m_pBulletBody_0->Set_ParentObject(this);
		m_pBulletBody_0->Reset_Init();
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody_0, SCENE_STAGE, L"Layer_Effect", nullptr);

		m_pBulletBody_1 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Player_Skill_Gun_BulletBody_Ver", nullptr));
		m_pBulletBody_1->Set_Desc(V3_NULL, nullptr);
		m_pBulletBody_1->Set_ParentObject(this);
		m_pBulletBody_1->Reset_Init();
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody_1, SCENE_STAGE, L"Layer_Effect", nullptr);

		CSoundManager::CHANNELID eChannel = CSoundManager::CHANNELID::Effect_SFX_01;
		_int iRand = CCalculater::Random_Num(0, 2);
		if (0 == iRand)
			eChannel = CSoundManager::CHANNELID::Effect_SFX_01;
		else if (1 == iRand)
			eChannel = CSoundManager::CHANNELID::Effect_SFX_02;
		else if (2 == iRand)
			eChannel = CSoundManager::CHANNELID::Effect_SFX_03;

		g_pSoundManager->Stop_Sound(eChannel);
		g_pSoundManager->Play_Sound(L"PlayerGunSkill_ShotEff.ogg", eChannel, CSoundManager::Effect_Sound);
	}

	Enter_Collision();
	Update_Trails(TimeDelta);

	m_pTransformCom->Add_Pos(m_fBullet_Speed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// Time Out
	if (m_dCurTime > m_dLifeTime)
	{
		// Death Effect
		if (nullptr == m_pBulletBody_0 || nullptr == m_pBulletBody_1 || nullptr == m_pTrailEffect)
			return E_FAIL;

		m_pBulletBody_0->Set_Dead();
		m_pBulletBody_1->Set_Dead();
		m_pTrailEffect->Set_Dead();

		CTexEffect* pEff = static_cast<CTexEffect*>(CManagement::Get_Instance()->Clone_GameObject_Return(L"Player_Skill_Gun_Bullet_DeadFlash", nullptr));
		pEff->Set_Desc(m_pTransformCom->Get_Pos(), nullptr);
		pEff->Set_LayerIdx(1);
		pEff->Reset_Init();
		CManagement::Get_Instance()->Add_GameOject_ToLayer_NoClone(pEff, SCENE_STAGE, L"Layer_Effect", nullptr);
		
		pEff = static_cast<CTexEffect*>(CManagement::Get_Instance()->Clone_GameObject_Return(L"Player_Skill_Gun_Bullet_DeadShockWave", nullptr));
		pEff->Set_Desc(m_pTransformCom->Get_Pos(), nullptr);
		pEff->Set_LayerIdx(1);
		pEff->Reset_Init();
		CManagement::Get_Instance()->Add_GameOject_ToLayer_NoClone(pEff, SCENE_STAGE, L"Layer_Effect", nullptr);
		
		g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Bullet_DeadSmoke_Move_Purple", 0.f, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Bullet_DeadSmoke_Blue_0", 0.25f, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Bullet_DeadSmoke_Blue_1", 0.25f, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"Player_Skill_Gun_Smoke_DarkBlue", 0.25f, m_pTransformCom->Get_Pos(), nullptr);

		m_bDead = true;
	}
	// InComing
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}
	}

	return S_OK;
}

_int CPlayerSkillBullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CPlayerSkillBullet::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CPlayerSkillBullet::Update_Collider()
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

	return;
}

void CPlayerSkillBullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CPlayerSkillBullet::Enter_Collision()
{
	Update_Collider();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));

	return;
}

void CPlayerSkillBullet::Update_Trails(_double TimeDelta)
{
	_mat matWorld = m_pTransformCom->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransformCom->Get_WorldMat()._21, sizeof(_v3));

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * -0.05f, vBegin + vDir * 0.05f);
		// m_pTrailEffect->Update_GameObject(TimeDelta);
	}

	return;
}

void CPlayerSkillBullet::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;

	for (auto& iter : plistGameObject)
	{
		if (false == iter->Get_Target_CanHit())
			continue;

		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				if (vecIter->Check_Sphere(vecCol))
				{
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					iter->Set_Target_CanHit(false);
					iter->Add_Target_Hp(-m_tObjParam.fDamage);

					m_dCurTime = 100;

					break;

				}
				else
				{
					if (bFirst)
						break;
				}
			}
		}
	}

	return;
}

HRESULT CPlayerSkillBullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerSkillBullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CPlayerSkillBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// Bullet center
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

CPlayerSkillBullet* CPlayerSkillBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPlayerSkillBullet* pInstance = new CPlayerSkillBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPlayerSkillBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayerSkillBullet::Clone_GameObject(void * pArg)
{
	CPlayerSkillBullet* pInstance = new CPlayerSkillBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPlayerSkillBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayerSkillBullet::Free()
{
	if (nullptr != m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	if (nullptr != m_pBulletBody_0)
		m_pBulletBody_0->Set_Dead();

	if (nullptr != m_pBulletBody_1)
		m_pBulletBody_1->Set_Dead();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}

