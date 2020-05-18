#include "stdafx.h"
#include "..\Headers\FireFlame.h"
#include "ParticleMgr.h"
#include "Effect.h"

CFireFlame::CFireFlame(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireFlame::CFireFlame(const CFireFlame & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireFlame::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFireFlame::Ready_GameObject(void * pArg)
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

	m_dCurTime = 0;
	m_fEffectOffset = 0.f;
	m_bDead = false;
	m_bFireMove = true;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	return NOERROR;
}

_int CFireFlame::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;


	// �ð��� �ʰ��� ������ ���� ���� �پ�����
	if (m_pTransformCom->Get_Pos().y < 0.f)
	{
		m_bFireMove = false;
		m_pTransformCom->Set_Pos_Axis(0.f, AXIS_Y);
	}
	// ���� ���߿��� �������� ��
	else
	{
		if (m_bFireMove)
		{
			m_pTransformCom->Add_Pos(m_fSpeed * _float(TimeDelta) * m_vDir);

			if (abs(m_vDir.y) < 0.9)
				m_vDir.y -= 1.f * _float(TimeDelta) * 2;
		}
	}

	m_dCurTime += TimeDelta;

	// �ð� �ʰ�
	if (m_dCurTime > m_dLifeTime)
	{
		m_bDead = true;
		
		g_pManagement->Create_Effect(L"FireBoy_FireFlame_DeadFire", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"FireBoy_FireGround_BoomParticle_01", m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect(L"FireBoy_FireGround_BoomParticle_02", m_pTransformCom->Get_Pos(), nullptr);
	}
	// ������
	else
	{
		m_fEffectOffset += (_float)TimeDelta;
		if (m_fEffectOffset > 0.01f)
		{
			m_fEffectOffset = 0.f;

			g_pManagement->Create_Effect(L"FireBoy_FireFlame", m_pTransformCom->Get_Pos(), nullptr);
		}

		g_pManagement->Create_Effect_Offset(L"FireBoy_FireBullet_Particle_01", 0.1f, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Offset(L"FireBoy_FireBullet_Particle_02", 0.1f, m_pTransformCom->Get_Pos(), nullptr);
	}


	OnCollisionEnter();


	return NOERROR;
}

_int CFireFlame::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFireFlame::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireFlame::Update_Collider()
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

void CFireFlame::OnCollisionEnter()
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
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));
	}

	// =============================================================================================

}

void CFireFlame::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

						m_dCurTime = 1000;	// �ٷ� �����Ű�� ���ؼ� ����ð� 100����
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

HRESULT CFireFlame::Add_Component()
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

HRESULT CFireFlame::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CFireFlame::Ready_Collider()
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

	return S_OK;
}

CFireFlame * CFireFlame::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireFlame* pInstance = new CFireFlame(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireFlame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireFlame::Clone_GameObject(void * pArg)
{
	CFireFlame* pInstance = new CFireFlame(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireFlame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireFlame::Free()
{
	CMonster::Free();
}
