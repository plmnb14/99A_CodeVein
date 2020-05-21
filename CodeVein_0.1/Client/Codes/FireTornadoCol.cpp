#include "stdafx.h"
#include "..\Headers\FireTornadoCol.h"

CFireTornadoCol::CFireTornadoCol(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireTornadoCol::CFireTornadoCol(const CFireTornadoCol & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireTornadoCol::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CFireTornadoCol::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_dLifeTime = temp.dLifeTime;
	
	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_dCurTime = 0;
	m_bDead = false;

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 250.f * pow(1.5f, g_eStageIdx_Cur - 1);

	return NOERROR;
}

_int CFireTornadoCol::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	OnCollisionEnter();

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		m_bDead = true;
	}

	return NOERROR;
}

_int CFireTornadoCol::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CFireTornadoCol::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireTornadoCol::Update_Collider()
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

HRESULT CFireTornadoCol::Draw_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

void CFireTornadoCol::OnCollisionEnter()
{
	// =============================================================================================
	// 충돌
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

void CFireTornadoCol::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						// 타겟이 피격 가능하다면
						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						iter->Hit_Target(m_tObjParam.fDamage);

						//m_dCurTime = 1000;	// 바로 사망시키기 위해서 현재시간 1000줬음
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

HRESULT CFireTornadoCol::Add_Component()
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

HRESULT CFireTornadoCol::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CFireTornadoCol::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 2.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CFireTornadoCol * CFireTornadoCol::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireTornadoCol* pInstance = new CFireTornadoCol(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireTornadoCol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireTornadoCol::Clone_GameObject(void * pArg)
{
	CFireTornadoCol* pInstance = new CFireTornadoCol(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireTornadoCol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireTornadoCol::Free()
{

	CMonster::Free();
}
