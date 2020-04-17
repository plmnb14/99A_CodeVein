#include "stdafx.h"
#include "..\Headers\FireGround.h"
#include "ParticleMgr.h"
#include "Effect.h"

CFireGround::CFireGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireGround::CFireGround(const CFireGround & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireGround::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFireGround::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	return NOERROR;
}

_int CFireGround::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	

	m_dCurTime += TimeDelta;

	// �ð� �ʰ�
	if (m_dCurTime > m_dLifeTime)
	{

		

		m_bDead = true;
	}
	// ������
	else
	{
		if (m_dCurTime > 0.01f && !m_bEffectReadyFireOn)
		{
			for (_int i = 0; i < 14; i++)
			{
				_mat matRotY;
				_v3 vDir = _v3(1.f, 0.f, 1.f);
				D3DXMatrixIdentity(&matRotY);

				D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(25.7 * i)));
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
				D3DXVec3Normalize(&vDir, &vDir);

				_float fMinRange = 2.f;
				_v3 vRandPos = vDir * (fMinRange);

				CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_ReadyFire", i * 0.03f, m_pTransformCom->Get_Pos() + vRandPos + _v3(0.f, 0.45f, 0.f), nullptr);
			}
			m_bEffectReadyFireOn = true;
		}

		if (m_dCurTime > 0.25f && !m_bEffectFloorOn)
		{
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_Floor", m_pTransformCom->Get_Pos() + _v3(0.f, 0.2f, 0.f), nullptr);
			m_bEffectFloorOn = true;
		}

		// ���� �� 0.75�� �� ���� ����
		if (m_dCurTime > 0.75f && !m_bStartBoom)
		{
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_BoomCircle", m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_BoomParticle_01", m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_BoomParticle_02", m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_BoomFire", 0.1f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.f, 0.f), nullptr);
			m_bStartBoom = true;
		}

		m_fEffectOffset += _float(TimeDelta);
		if (m_bStartBoom && m_fEffectOffset > 0.6f)
		{
			m_fEffectOffset = 0.f;
			for (_int i = 0; i < 10; i++)
			{
				_mat matRotY;
				_v3 vDir = _v3(1.f, 0.f, 1.f);
				D3DXMatrixIdentity(&matRotY);

				D3DXMatrixRotationY(&matRotY, _float(D3DXToRadian(CCalculater::Random_Num_Double(0, 360))));
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
				D3DXVec3Normalize(&vDir, &vDir);

				_float fMinRange = 2.f;
				_v3 vRandPos = vDir * _float(CCalculater::Random_Num_Double(0, _double(fMinRange)));
				
				CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_AfterFire_01", _float(CCalculater::Random_Num_Double(0, 1.0)), m_pTransformCom->Get_Pos() + vRandPos + _v3(0.f, 0.45f, 0.f), nullptr);
				CParticleMgr::Get_Instance()->Create_Effect_Delay(L"FireBoy_FireGround_AfterFire_02", _float(CCalculater::Random_Num_Double(0, 1.0)), m_pTransformCom->Get_Pos() + vRandPos + _v3(0.f, 0.45f, 0.f), nullptr);
				CParticleMgr::Get_Instance()->Create_Effect(L"FireBoy_FireGround_Particle",m_pTransformCom->Get_Pos(), nullptr);
			}
		}
	}

	//���ʷ� �ٴڿ� ���� ������ ��  �����ð� �� �ҵ���� �����鼭 ���������� �浹ó��

	// �� ���� ����
	if (false == m_bFinishBoom && true == m_bStartBoom)
	{
		m_dCurBoomTime += TimeDelta;

		if(false == m_bFinishCol)	//�浹�ϸ� true�� �ٲ�
			OnCollisionEnter();

		// �� ���� ���� �� ��� �ݶ��̴� On ��Ŵ
		if (m_dCurBoomTime > 0.2f)
		{
			m_bFinishCol = true;	// �浹ó�� End
		}
	}



	return NOERROR;
}

_int CFireGround::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFireGround::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireGround::Update_Collider()
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

void CFireGround::OnCollisionEnter()
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));


	// =============================================================================================

}

void CFireGround::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

					m_bFinishCol = true;	// �浹 �Ϸ�

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

HRESULT CFireGround::Add_Component()
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

HRESULT CFireGround::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CFireGround::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// �Ѿ� �߾�
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 2.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CFireGround * CFireGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireGround* pInstance = new CFireGround(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireGround::Clone_GameObject(void * pArg)
{
	CFireGround* pInstance = new CFireGround(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireGround::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
