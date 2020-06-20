#include "stdafx.h"
#include "..\Headers\YetiBullet.h"
#include "ParticleMgr.h"

CYetiBullet::CYetiBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CYetiBullet::CYetiBullet(const CYetiBullet & rhs)
	: CMonster(rhs)
{
}

HRESULT CYetiBullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CYetiBullet::Ready_GameObject(void * pArg)
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
	m_tObjParam.fDamage = 130 * pow(1.5f, g_eStageIdx_Cur - 1);

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;
	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_pBulletBody = CParticleMgr::Get_Instance()->Create_EffectReturn(L"DeerKing_IceBullet_0");
	m_pBulletBody->Set_Desc(V3_NULL, nullptr);
	m_pBulletBody->Set_ParentObject(this);
	m_pBulletBody->Reset_Init();

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(5); // Red Tail

	return S_OK;
}

_int CYetiBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	Check_CollisionEvent();
	Update_Trails(TimeDelta);

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// �ð� �ʰ�
	if (m_dCurTime > m_dLifeTime)
	{
		//���� ����Ʈ
		CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_IceBullet_DeadParticle_0", m_pTransformCom->Get_Pos());
		CParticleMgr::Get_Instance()->Create_Effect(L"Yeti_Bullet_Dead_Splash_0", m_pTransformCom->Get_Pos());
		m_pBulletBody->Set_Dead();
		m_pTrailEffect->Set_Dead();

		m_bDead = true;
	}
	// ������
	else
	{
		if (m_bEffect)
		{
			CParticleMgr::Get_Instance()->Create_Effect(L"Yeti_Bullet_Dead_FlashParticle_0", _v3(), m_pTransformCom);

			m_bEffect = false;
		}

		CParticleMgr::Get_Instance()->Create_Effect_Offset(L"Yeti_Bullet_BodySnow_0", 0.1f, m_pTransformCom->Get_Pos());
	}

	return S_OK;
}

_int CYetiBullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CYetiBullet::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CYetiBullet::Update_Trails(_double TimeDelta)
{
	_mat matWorld = m_pTransformCom->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransformCom->Get_WorldMat()._21, sizeof(_v3));

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * -0.05f, vBegin + vDir * 0.05f);
		//m_pTrailEffect->Update_GameObject(TimeDelta);
	}

	return;
}

void CYetiBullet::Update_Collider()
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

void CYetiBullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CYetiBullet::Check_CollisionEvent()
{
	Update_Collider();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));

	return;
}

void CYetiBullet::Check_CollisionHit(list<CGameObject*> plistGameObject)
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
							_uint min = (_uint)(m_tObjParam.fDamage - (m_tObjParam.fDamage * 0.2f));
							_uint max = (_uint)(m_tObjParam.fDamage + (m_tObjParam.fDamage * 0.2f));

							iter->Add_Target_Hp(-(_float)CALC::Random_Num(min, max));
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

HRESULT CYetiBullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CYetiBullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CYetiBullet::Ready_Collider()
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

CYetiBullet* CYetiBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYetiBullet* pInstance = new CYetiBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CYetiBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYetiBullet::Clone_GameObject(void * pArg)
{
	CYetiBullet* pInstance = new CYetiBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CYetiBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYetiBullet::Free()
{
	IF_NOT_NULL(m_pBulletBody)
		m_pBulletBody->Set_Dead();

	IF_NOT_NULL(m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	CMonster::Free();

	return;
}

