#include "stdafx.h"
#include "..\Headers\DeerKingJumpInPlaceCol.h"
#include "ParticleMgr.h"
#include "Effect.h"

CDeerKingJumpInPlaceCol::CDeerKingJumpInPlaceCol(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDeerKingJumpInPlaceCol::CDeerKingJumpInPlaceCol(const CDeerKingJumpInPlaceCol & rhs)
	: CMonster(rhs)
{
}

HRESULT CDeerKingJumpInPlaceCol::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDeerKingJumpInPlaceCol::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 20.f;



	return NOERROR;
}

_int CDeerKingJumpInPlaceCol::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dLifeTime)
	{
		OnCollisionEnter();

		m_bDead = true;
	}
	else
	{
		if (!m_bEffect)
		{
			m_bEffect = true;

			for (_int i = 0; i < 1000; i++)
			{
				_mat matRotX, matRotY, matRotZ;
				_v3 vDir = _v3(1.f, 1.f, 1.f);
				D3DXMatrixIdentity(&matRotX);
				D3DXMatrixIdentity(&matRotY);
				D3DXMatrixIdentity(&matRotZ);
			
				D3DXMatrixRotationX(&matRotX, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
				D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
				D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotX);
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
				D3DXVec3TransformNormal(&vDir, &vDir, &matRotZ);
				D3DXVec3Normalize(&vDir, &vDir);
			
				_float fMinRange = 10.f;
				_v3 vRandPos = vDir * (fMinRange);
			
				CParticleMgr::Get_Instance()->Create_Effect(L"DeerKing_Point_ExplosionParticle_0", m_pTransformCom->Get_Pos() + vRandPos, nullptr);
			}
			//CParticleMgr::Get_Instance()->Create_Effect_Delay(L"DeerKing_Jump_In_Place_Distortion", 0.2, m_pTransformCom->Get_Pos(), nullptr);
			//CParticleMgr::Get_Instance()->Create_Effect_Delay(L"DeerKing_Jump_In_Place_Distortion", 0.3, m_pTransformCom->Get_Pos(), nullptr);
			//CParticleMgr::Get_Instance()->Create_Effect_Delay(L"DeerKing_Jump_In_Place_Distortion", 0.4, m_pTransformCom->Get_Pos(), nullptr);
		}
	}


	return NOERROR;
}

_int CDeerKingJumpInPlaceCol::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CDeerKingJumpInPlaceCol::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDeerKingJumpInPlaceCol::Update_Collider()
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

void CDeerKingJumpInPlaceCol::OnCollisionEnter()
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));


	// =============================================================================================

}

void CDeerKingJumpInPlaceCol::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

						iter->Add_Target_Hp(-m_tObjParam.fDamage);

						m_dCurTime = 10000;	// 맞으면 제거
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

HRESULT CDeerKingJumpInPlaceCol::Calc_Dir()
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL));

	m_vDir = *D3DXVec3Normalize(&_v3(), &(pPlayerTrans->Get_Pos() + _v3(0.f, 0.5f, 0.f) - m_pTransformCom->Get_Pos()));

	return S_OK;
}

HRESULT CDeerKingJumpInPlaceCol::Add_Component()
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

HRESULT CDeerKingJumpInPlaceCol::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CDeerKingJumpInPlaceCol::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	// 총알 중앙
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 5.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return NOERROR;
}

CDeerKingJumpInPlaceCol * CDeerKingJumpInPlaceCol::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDeerKingJumpInPlaceCol* pInstance = new CDeerKingJumpInPlaceCol(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDeerKingJumpInPlaceCol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeerKingJumpInPlaceCol::Clone_GameObject(void * pArg)
{
	CDeerKingJumpInPlaceCol* pInstance = new CDeerKingJumpInPlaceCol(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDeerKingJumpInPlaceCol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDeerKingJumpInPlaceCol::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
