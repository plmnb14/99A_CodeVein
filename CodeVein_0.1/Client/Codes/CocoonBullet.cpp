#include "stdafx.h"
#include "..\Headers\CocoonBullet.h"

CCocoonBullet::CCocoonBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CCocoonBullet::CCocoonBullet(const CCocoonBullet & rhs)
	: CMonster(rhs)
{
}

HRESULT CCocoonBullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCocoonBullet::Ready_GameObject(void * pArg)
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

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 120.f * pow(1.5f, g_eStageIdx_Cur - 1);

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;
	m_fEffectOffset = 0.f;

	switch (m_eBulletType)
	{
	case MONSTER_BULLET_TYPE::BULLET_ELECTRON:
	case MONSTER_BULLET_TYPE::BULLET_NORMAL:
	case MONSTER_BULLET_TYPE::BULLET_FIRE:
		m_pBulletBody = CParticleMgr::Get_Instance()->Create_EffectReturn(L"FireBoy_FireBullet_Mid");
		break;
	case MONSTER_BULLET_TYPE::BULLET_ICE:
		m_pBulletBody = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Totem_Ice_BulletBody");
		break;
	}
	m_pBulletBody->Set_Desc(V3_NULL, m_pTransformCom);
	m_pBulletBody->Reset_Init();

	return S_OK;
}

_int CCocoonBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;
	
	Check_CollisionEvent();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dLifeTime)
	{
		m_pBulletBody->Set_Dead();

		m_bDead = true;
	}
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}

		m_fEffectOffset += (_float)TimeDelta;
		if (m_fEffectOffset > 0.1f)
		{
			m_fEffectOffset = 0.f;
			
			switch (m_eBulletType)
			{
			case MONSTER_BULLET_TYPE::BULLET_ELECTRON:
			case MONSTER_BULLET_TYPE::BULLET_NORMAL:
			case MONSTER_BULLET_TYPE::BULLET_FIRE:
				g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransformCom->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransformCom->Get_Pos(), nullptr);
				break;
			case MONSTER_BULLET_TYPE::BULLET_ICE:
				g_pManagement->Create_Effect(L"DeerKing_IceSmoke_Small_0", m_pTransformCom->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"DeerKing_IceSmoke_Small_1", m_pTransformCom->Get_Pos(), nullptr);
				break; 
			}
		}
	}

	return NO_EVENT;
}

_int CCocoonBullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CCocoonBullet::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CCocoonBullet::Update_Collider()
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

void CCocoonBullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CCocoonBullet::Check_CollisionEvent()
{
	Update_Collider();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));

	return;
}

void CCocoonBullet::Check_CollisionHit(list<CGameObject*> plistGameObject)
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

HRESULT CCocoonBullet::Add_Component()
{
	if (FAILED(CMonster::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CMonster::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CMonster::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCocoonBullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CCocoonBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

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

CCocoonBullet * CCocoonBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCocoonBullet* pInstance = new CCocoonBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCocoonBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCocoonBullet::Clone_GameObject(void * pArg)
{
	CCocoonBullet* pInstance = new CCocoonBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CCocoonBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCocoonBullet::Free()
{
	IF_NOT_NULL(m_pBulletBody)
		m_pBulletBody->Set_Dead();

	CMonster::Free();

	return;
}

