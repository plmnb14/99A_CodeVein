#include "stdafx.h"
#include "..\Headers\HunterBullet.h"
#include "ParticleMgr.h"

CHunterBullet::CHunterBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CHunterBullet::CHunterBullet(const CHunterBullet & rhs)
	: CMonster(rhs)
{
}

HRESULT CHunterBullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CHunterBullet::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	MONSTER_BULLET_STATUS info = *(MONSTER_BULLET_STATUS*)pArg;

	m_eBulletType = info.eBulletType;
	m_vDir = info.vDir;
	m_fSpeed = info.fSpeed;
	m_dLifeTime = info.dLifeTime;

	m_pTransformCom->Set_Pos(info.vCreatePos);
	m_pTransformCom->Set_Scale(V3_ONE);

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 300.f * pow(1.5f, g_sStageIdx_Cur - 1);

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;

	m_pBulletBody_0 = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Hunter_Bullet_Body_Lightning_Hor_0");
	m_pBulletBody_0->Set_Desc(V3_NULL, nullptr);
	m_pBulletBody_0->Set_ParentObject(this);
	m_pBulletBody_0->Reset_Init();

	m_pBulletBody_1 = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Hunter_Bullet_Body_Lightning_Ver_0");
	m_pBulletBody_1->Set_Desc(V3_NULL, nullptr);
	m_pBulletBody_1->Set_ParentObject(this);
	m_pBulletBody_1->Reset_Init();

	m_pBulletBody_2 = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Hunter_Bullet_Body_LinePoint_Hor");
	m_pBulletBody_2->Set_Desc(V3_NULL, nullptr);
	m_pBulletBody_2->Set_ParentObject(this);
	m_pBulletBody_2->Reset_Init();

	m_pBulletBody_3 = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Hunter_Bullet_Body_LinePoint_Ver");
	m_pBulletBody_3->Set_Desc(V3_NULL, nullptr);
	m_pBulletBody_3->Set_ParentObject(this);
	m_pBulletBody_3->Reset_Init();

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(11);

	return S_OK;
}

_int CHunterBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	Update_Trails(TimeDelta);
	Check_CollisionEvent();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// ½Ã°£ ÃÊ°ú
	if (m_dCurTime > m_dLifeTime)
	{
		//Á×À½ ÀÌÆåÆ®
		CParticleMgr::Get_Instance()->Create_Effect(L"Hunter_Bullet_Dead_Smoke_0", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(L"Hunter_Bullet_Dead_Smoke_1", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(L"Hunter_Bullet_Dead_Lightning", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(L"Hunter_Bullet_Dead_Flash", m_pTransformCom->Get_Pos());
		
		m_pBulletBody_0->Set_Dead();
		m_pBulletBody_1->Set_Dead();
		m_pBulletBody_2->Set_Dead();
		m_pBulletBody_3->Set_Dead();

		m_pTrailEffect->Set_Dead();

		if (0 == CCalculater::Random_Num(0, 1))
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Effect_SFX_03);
			g_pSoundManager->Play_Sound(L"SE_THUNDER_ELEMENTS_001.ogg", CSoundManager::Effect_SFX_03, CSoundManager::Effect_Sound);
		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::Effect_SFX_03);
			g_pSoundManager->Play_Sound(L"SE_THUNDER_ELEMENTS_002.ogg", CSoundManager::Effect_SFX_03, CSoundManager::Effect_Sound);
		}
		
		m_bDead = true;
	}
	// ÁøÇàÁß
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}

		CParticleMgr::Get_Instance()->Create_Effect_Offset(L"Hunter_Bullet_Tail_Lightning_Particle", 0.01f, m_pTransformCom->Get_Pos());
	}

	return S_OK;
}

_int CHunterBullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CHunterBullet::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CHunterBullet::Update_Trails(_double TimeDelta)
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

void CHunterBullet::Update_Collider()
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

void CHunterBullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CHunterBullet::Check_CollisionEvent()
{
	Update_Collider();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));

	return;
}

void CHunterBullet::Check_CollisionHit(list<CGameObject*> plistGameObject)
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

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						if (true == iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						if (false == iter->Get_Target_IsDodge())
						{
							iter->Hit_Target(m_tObjParam.fDamage);
							g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

							m_dCurTime = 100;
						}
					}
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

HRESULT CHunterBullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHunterBullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CHunterBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// ÃÑ¾Ë Áß¾Ó
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.3f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CHunterBullet* CHunterBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHunterBullet* pInstance = new CHunterBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CHunterBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHunterBullet::Clone_GameObject(void * pArg)
{
	CHunterBullet* pInstance = new CHunterBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CHunterBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHunterBullet::Free()
{
	IF_NOT_NULL(m_pBulletBody_0)
		m_pBulletBody_0->Set_Dead();

	IF_NOT_NULL(m_pBulletBody_1)
		m_pBulletBody_1->Set_Dead();

	IF_NOT_NULL(m_pBulletBody_2)
		m_pBulletBody_2->Set_Dead();

	IF_NOT_NULL(m_pBulletBody_3)
		m_pBulletBody_3->Set_Dead();

	IF_NOT_NULL(m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	IF_NOT_NULL(m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	CMonster::Free();

	return;
}

