#include "stdafx.h"
#include "..\Headers\DarkGroundBullet.h"
#include "ParticleMgr.h"
#include "Effect.h"

CDarkGroundBullet::CDarkGroundBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDarkGroundBullet::CDarkGroundBullet(const CDarkGroundBullet & rhs)
	: CMonster(rhs)
{
}

HRESULT CDarkGroundBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDarkGroundBullet::Ready_GameObject(void * pArg)
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

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;


	return NOERROR;
}

_int CDarkGroundBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// �ð� �ʰ�
	if (m_dCurTime > m_dLifeTime)
	{
		// ������ ��ü ����
		g_pManagement->Add_GameObject_ToLayer(L"Monster_DarkBoom", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(m_pTransformCom->Get_Pos(), m_vDir, m_fSpeed, 1));
		m_bDead = true;
	}
	// ������
	else
	{
		
	}

	return NOERROR;
}

_int CDarkGroundBullet::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDarkGroundBullet::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDarkGroundBullet::Update_Collider()
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

void CDarkGroundBullet::OnCollisionEnter()
{
	Update_Collider();

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

void CDarkGroundBullet::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CDarkGroundBullet::Add_Component()
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

HRESULT CDarkGroundBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDarkGroundBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// �Ѿ� �߾�
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CDarkGroundBullet * CDarkGroundBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDarkGroundBullet* pInstance = new CDarkGroundBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDarkGroundBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDarkGroundBullet::Clone_GameObject(void * pArg)
{
	CDarkGroundBullet* pInstance = new CDarkGroundBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDarkGroundBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDarkGroundBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
