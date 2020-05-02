#include "stdafx.h"
#include "..\Headers\CocoonBigBullet.h"

CCocoonBigBullet::CCocoonBigBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCocoonBigBullet::CCocoonBigBullet(const CCocoonBigBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCocoonBigBullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCocoonBigBullet::Ready_GameObject(void * pArg)
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
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Totem_Fire_BulletBody", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);
	
	return S_OK;
}

_int CCocoonBigBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	Enter_Collision();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dLifeTime)
	{
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_0", m_pTransformCom->Get_Pos());
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_1", m_pTransformCom->Get_Pos());
		g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_Particle", m_pTransformCom->Get_Pos());
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
		if (m_fEffectOffset > 0.06f)
		{
			m_fEffectOffset = 0.f;
			g_pManagement->Create_Effect(L"Totem_Fire_BulletBody", m_pTransformCom->Get_Pos() + m_vDir * 1.3f);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_01", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"FireBoy_FireBullet_Particle_02", m_pTransformCom->Get_Pos(), nullptr);
		}
	}

	return NO_EVENT;
}

_int CCocoonBigBullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CCocoonBigBullet::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CCocoonBigBullet::Update_Collider()
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

void CCocoonBigBullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CCocoonBigBullet::Enter_Collision()
{
	Update_Collider();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));

	return;
}

void CCocoonBigBullet::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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

					m_dCurTime = 100;	// 바로 사망시키기 위해서 현재시간 100줬음

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

HRESULT CCocoonBigBullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCocoonBigBullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CCocoonBigBullet::Ready_Collider()
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

CCocoonBigBullet * CCocoonBigBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCocoonBigBullet* pInstance = new CCocoonBigBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCocoonBigBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCocoonBigBullet::Clone_GameObject(void * pArg)
{
	CCocoonBigBullet* pInstance = new CCocoonBigBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CCocoonBigBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCocoonBigBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}


