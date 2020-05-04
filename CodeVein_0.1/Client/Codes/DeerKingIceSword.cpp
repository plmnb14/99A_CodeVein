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

	// �罿�� �����忡�� ������.
	m_vDir = m_pTarget_AIController->Get_V3Value(L"IceSword_Dir");

	m_pTransformCom->Set_Pos(m_pTarget_AIController->Get_V3Value(L"IceSword_Pos"));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;

	return NOERROR;
}

_int CDeerKingIceSword::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_dCurTime += TimeDelta;

	m_vDir = m_pTarget_AIController->Get_V3Value(L"IceSword_Dir");
	m_pTransformCom->Set_Pos(m_pTarget_AIController->Get_V3Value(L"IceSword_Pos"));


	if (m_dCurTime > m_dLifeTime)
	{
		m_bDead = true;
	}
	else
	{

	}

	// �浹ü ��ġ�� ���� ����
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

HRESULT CDeerKingIceSword::Render_GameObject()
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

		iter->Update(ColPos);

		++matrixIdx;
	}

	return NOERROR;
}

void CDeerKingIceSword::OnCollisionEnter()
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

void CDeerKingIceSword::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CDeerKingIceSword::Add_Component()
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

HRESULT CDeerKingIceSword::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDeerKingIceSword::Ready_Collider()
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTarget_AIController);

	CGameObject::Free();
}
