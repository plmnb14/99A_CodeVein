#include "stdafx.h"
#include "CollBullet_Heal.h"

#include "Player.h"
#include "Player_Colleague.h"

#include "ParticleMgr.h"


CCollBullet_Heal::CCollBullet_Heal(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCollBullet_Heal::CCollBullet_Heal(const CCollBullet_Heal & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCollBullet_Heal::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCollBullet_Heal::Ready_GameObject(void * pArg)
{
	// Ÿ�Կ� ���� ���� �Ѿ˰� ���ݿ� �Ѿ˷� ������.
	// ���Ѿ��� �÷��̾�� �ݿ��� �׸��鼭 �ٰ�����, ���� �� HP�� ȸ����Ų��.
	// ���Ѿ��� �÷��̾�� ���� ������ �i�ư���.
	// ���Ѿ� �ӵ��� �����Ѿ˺��ٴ� ���� ���̾�� ��
	// �����Ѿ��� ���Ϳ��� ���������� ���ϸ� ���ǵ尡 �ټ� ���� ���̾�� �Ѵ�.

	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	cout << "Create Heal Bullet!!!!!" << endl;

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_vDir = TARGET_TO_TRANS(m_pTarget)->Get_Axis(AXIS_Z);
	m_fBullet_Speed = 8.f;
	m_dLifeTime = temp.dLifeTime;

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 200.f;

	m_pBulletBody_0 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Colleague_Heal_BulletBody_Hor", nullptr));
	m_pBulletBody_0->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody_0->Set_ParentObject(this);
	m_pBulletBody_0->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody_0, SCENE_STAGE, L"Layer_Effect", nullptr);

	m_pBulletBody_1 = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Colleague_Heal_BulletBody_Ver", nullptr));
	m_pBulletBody_1->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody_1->Set_ParentObject(this);
	m_pBulletBody_1->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody_1, SCENE_STAGE, L"Layer_Effect", nullptr);

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(4); // Red Tail

	return S_OK;
}

_int CCollBullet_Heal::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	_float fPlusHP = 0.f;

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if(nullptr != m_pTarget)
		fPlusHP = (m_pTarget->Get_pTarget_Param()->fHp_Cur / 2.f);

	Enter_Collision();
	Update_Trails(TimeDelta);

	_v3 Temp = V3_NULL;
	_float fLen = 0.f;
	if (nullptr != pPlayer)
	{
		Temp = TARGET_TO_TRANS(pPlayer)->Get_Pos() + _v3(0.f, 1.f, 0.f) - m_pTransformCom->Get_Pos();
		fLen = D3DXVec3Length(&Temp);
		D3DXVec3Normalize(&Temp, &Temp);
	}

	m_pTransformCom->Add_Pos(m_fBullet_Speed * (_float)TimeDelta, Temp);

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &Temp);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &Temp));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_dCurTime += TimeDelta;

	// �÷��̾�� �浹�ߴٸ�
	//if (false == pPlayer->Get_Target_IsHit())
	if (fLen < 0.8f)
	{
		cout << "Heal Bullet Dead : HealAmount - "<< fPlusHP << endl;

		pPlayer->Add_Target_Hp(fPlusHP);

		//���� ����Ʈ
		m_pBulletBody_0->Set_Dead();
		m_pBulletBody_1->Set_Dead();
		m_pTrailEffect->Set_Dead();

		m_bDead = true;
	}
	// ������
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}

		CParticleMgr::Get_Instance()->Create_Effect_Offset(L"Colleague_Heal_BulletBody_Aura", 0.01f, m_pTransformCom->Get_Pos());
	}

	return S_OK;
}

_int CCollBullet_Heal::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollBullet_Heal::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CCollBullet_Heal::Update_Trails(_double TimeDelta)
{
	_mat matWorld = m_pTransformCom->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransformCom->Get_WorldMat()._21, sizeof(_v3));

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * -0.05f, vBegin + vDir * 0.05f);
		// m_pTrailEffect->Update_GameObject(TimeDelta);
	}
}

void CCollBullet_Heal::Update_Collider()
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
}

void CCollBullet_Heal::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
}

void CCollBullet_Heal::Enter_Collision()
{
	Update_Collider();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
}

void CCollBullet_Heal::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bCanAttack)
		return;

	_float fHealHP = 0.f;

	/*CGameObject* pColleague = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);
	if (nullptr != pColleague)
		fHealHP = (pColleague->Get_Target_Hp() / 30.f);*/

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

					/*if (false == m_pTarget->Get_Target_IsHit())
					{
						iter->Add_Target_Hp(fHealHP);
						return;
					}*/
						

					m_dCurTime = 100;	// �ٷ� �����Ű�� ���ؼ� ����ð� 100����

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

HRESULT CCollBullet_Heal::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCollBullet_Heal::SetUp_ConstantTable()
{

	return S_OK;
}

HRESULT CCollBullet_Heal::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CCollBullet_Heal* CCollBullet_Heal::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCollBullet_Heal*	pInstance = new CCollBullet_Heal(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating Colleague Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCollBullet_Heal::Clone_GameObject(void* pArg)
{
	CCollBullet_Heal*	pInstance = new CCollBullet_Heal(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CColleague_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollBullet_Heal::Free()
{
	if (nullptr != m_pBulletBody_0)
		m_pBulletBody_0->Set_Dead();

	if (nullptr != m_pBulletBody_1)
		m_pBulletBody_1->Set_Dead();

	if (nullptr != m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecAttackCol)
	{
		Safe_Release(iter);
	}

	CGameObject::Free();
}
