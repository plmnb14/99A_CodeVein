#include "stdafx.h"
#include "..\Headers\SwordBullet.h"
#include "ParticleMgr.h"
#include "Effect.h"

CSwordBullet::CSwordBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CSwordBullet::CSwordBullet(const CSwordBullet & rhs)
	: CMonster(rhs)
{
}

HRESULT CSwordBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordBullet::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = temp.vDir;
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	m_pBulletBody_01 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"IceGirl_SwordBullet_Mesh", nullptr));
	m_pBulletBody_01->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody_01->Set_ParentObject(this);
	m_pBulletBody_01->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody_01, SCENE_STAGE, L"Layer_Effect", nullptr);
	
	return NOERROR;
}

_int CSwordBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dLifeTime)
	{
		m_pBulletBody_01->Set_Dead();

		m_bDead = true;
	}
	else
	{
		m_fEffectOffset += (_float)TimeDelta;

		if (m_fEffectOffset > 0.01f)
		{
			m_fEffectOffset = 0.f;
			g_pManagement->Create_Effect(L"IceGirl_PointParticle_Blue", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_PointParticle_Green", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_FlashParticle_Blue", m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect(L"IceGirl_FlashParticle_Green", m_pTransformCom->Get_Pos(), nullptr);
		}
		
	}

	return NOERROR;
}

_int CSwordBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSwordBullet::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CSwordBullet::Update_Collider()
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

	return NOERROR;
}

void CSwordBullet::OnCollisionEnter()
{
	// =============================================================================================
	// �浹
	// =============================================================================================

	if (m_bPlayerFriendly)
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_MonsterProjectile", SCENE_STAGE));
	}
	else
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));


	// =============================================================================================

}

void CSwordBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// ���� �Ұ����̸� üũ ����
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;
	//���� ������Ʈ�� �޾ƿͼ�
	for (auto& iter : plistGameObject)
	{
		// ���� �� ���ٸ� ����
		if (false == iter->Get_Target_CanHit())
			continue;

		// ���� ���� Vec �ݶ��̴��� ���Ѵ�.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// �����ݶ��̴��� ��
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// ��ü ��ü�� ��ǥ�� �ݶ��̴�.
				if (vecIter->Check_Sphere(vecCol))
				{
					// ù��°�� ��豸 �ݶ��̴��ϱ� ���� �ݶ��̴��� �浹ó�� �Ѵ�.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						// Ÿ���� �ǰ� �����ϴٸ�
						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						iter->Add_Target_Hp(-m_tObjParam.fDamage);
					}

					m_dCurTime = 1000;	// �ٷ� �����Ű�� ���ؼ� ����ð� 100����

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

HRESULT CSwordBullet::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CSwordBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CSwordBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// �Ѿ� �߾�
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.9f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return NOERROR;
}

CSwordBullet * CSwordBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSwordBullet* pInstance = new CSwordBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CSwordBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwordBullet::Clone_GameObject(void * pArg)
{
	CSwordBullet* pInstance = new CSwordBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CSwordBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
