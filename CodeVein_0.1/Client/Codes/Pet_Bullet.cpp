#include "stdafx.h"
#include "..\Headers\Pet_Bullet.h"

CPet_Bullet::CPet_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CPet(pGraphic_Device)
{
}

CPet_Bullet::CPet_Bullet(const CPet_Bullet & rhs)
	: CPet(rhs)
{
}

HRESULT CPet_Bullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPet_Bullet::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	Ready_Effect(pArg);

	return S_OK;
}

_int CPet_Bullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_dCurTime += TimeDelta;

	switch (m_eBulletType)
	{
	case PET_BULLET_TYPE::PET_BULLET_POISON:
	{
		m_pTransform->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

		if (m_pTransform->Get_Pos().y <= 0.f)
			m_dCurTime = 1000;

		if (m_dCurTime > m_dLifeTime)
		{
			g_pManagement->Create_Effect(L"ButterFly_VenomShot_DeadSplash", m_pTransform->Get_Pos());
			g_pManagement->Create_Effect(L"ButterFly_VenomShot_DeadMist", m_pTransform->Get_Pos());
			g_pManagement->Create_Effect(L"ButterFly_VenomShot_DeadSmoke", m_pTransform->Get_Pos());
			g_pManagement->Create_ParticleEffect(L"ButterFly_PointParticle", 0.5f, m_pTransform->Get_Pos(), nullptr);
			m_pEffect->Set_Dead();

			m_bDead = true;
		}
		else
		{
			if (true == m_bEffect)
				m_bEffect = false;

			m_fEffectOffset += (_float)TimeDelta;

			if (m_fEffectOffset > 0.1f)
			{
				m_fEffectOffset = 0.f;
				g_pManagement->Create_Effect(L"ButterFly_VenomShot_Body_Sub", m_pTransform->Get_Pos() + m_vDir * 1.9f);
				g_pManagement->Create_Effect(L"ButterFly_VenomShot", m_pTransform->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"ButterFly_VenomShot_SubSmoke", m_pTransform->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"ButterFly_VenomShot_PointParticle", m_pTransform->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"ButterFly_VenomShot_Chunk", m_pTransform->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"ButterFly_VenomShot_Distortion", m_pTransform->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"ButterFly_SoftSmoke_Mist", m_pTransform->Get_Pos(), nullptr);
				g_pManagement->Create_Effect(L"ButterFly_VenomShot_Tail", m_pTransform->Get_Pos(), nullptr);
			}
		}

		Check_CollisionEvent();
	}
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICE:
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICICLEBLADE:
	//{
	//	m_vDir = m_pTarget_AIController->Get_V3Value(L"IceSword_Dir");
	//	_v3 vSelfDir = m_pTarget_AIController->Get_V3Value(L"Self_Dir");
	//	_mat matAttatch = m_pTarget_AIController->Get_MatValue(L"Mat_LeftHandAttach");
	//	_mat matParent = m_pTarget_AIController->Get_MatValue(L"Self_Mat");
	//	m_pTransform->Calc_ParentMat(&(matAttatch * matParent));
	//	m_pTransform->Set_Angle(AXIS_X, D3DXToRadian(m_vAngle.x));
	//	m_pTransform->Set_Angle(AXIS_Y, D3DXToRadian(m_vAngle.y));
	//	m_pTransform->Set_Angle(AXIS_Z, D3DXToRadian(m_vAngle.z));
	//	if (m_dCurTime > m_dLifeTime)
	//	{
	//		m_bDead = true;
	//		m_pEffect->Set_Dead();
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_01", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_02", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_03", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_01", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_01", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransform->Get_Pos() + m_vDir * 0.6f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransform->Get_Pos() + m_vDir * 1.8f, nullptr);
	//	}
	//	else
	//	{
	//		if (m_dCurTime > 0.1f && m_bEffect)
	//		{
	//			m_bEffect = true;
	//			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_01", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Smoke_02", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		}
	//		m_fEffectOffset += _float(TimeDelta);
	//		if (m_fEffectOffset > 0.01f)
	//		{
	//			m_fEffectOffset = 0.f;
	//			CParticleMgr::Get_Instance()->Create_Effect(L"IceSmoke_01", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//			CParticleMgr::Get_Instance()->Create_Effect(L"IceSmoke_02", m_pTransform->Get_Pos() + m_vDir * 1.3f, nullptr);
	//		}
	//	}
	//	Enter_Collision();
	//}
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICICLEBEAM:
	{
		if (m_dCurTime > m_dLifeTime)
		{
			CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_01", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_02", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceCrystal_03", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Bubble_BreakSmoke", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Bubble_BreakSmoke", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Bubble_BreakSmoke", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Particle", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 0.3f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 1.5f, nullptr);
			CParticleMgr::Get_Instance()->Create_Effect(L"IceBlock_Break", m_pTransform->Get_Pos() + _v3(0.f, -0.8f, 0.f) + m_vDir * 2.8f, nullptr);

			m_pEffect->Set_Dead();
			m_bDead = true;
		}
		else
		{
			if (m_dCurTime < 0.1f)
				m_pTransform->Add_Pos(_float(20.f * TimeDelta), m_vDir);

			m_fEffectOffset += _float(TimeDelta);

			if (m_fEffectOffset > 0.3f)
				m_fEffectOffset = 0.f;
		}

		if (m_dCurTime <= 0.3f)
			Check_CollisionEvent();

	}
		break;

	case PET_BULLET_TYPE::PET_BULLET_FIRE:
		break;

	case PET_BULLET_TYPE::PET_BULLET_NONE:
		break;
	}

	return NO_EVENT;
}

_int CPet_Bullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CPet_Bullet::Render_GameObject()
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

void CPet_Bullet::Update_Collider()
{
	_ulong matrixIdx = 0;
	_mat tmpMat;
	_v3 ColPos = V3_NULL, LookPos = V3_NULL;

	switch (m_eBulletType)
	{
	case PET_BULLET_TYPE::PET_BULLET_POISON:
		for (auto& iter : m_vecAttackCol)
		{
			tmpMat = m_pTransform->Get_WorldMat();

			ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

			iter->Update(ColPos);

			++matrixIdx;
		}
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICE:
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICICLEBLADE:
		for (auto& iter : m_vecAttackCol)
		{
			tmpMat = m_pTransform->Get_WorldMat();

			ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

			iter->Update(ColPos + m_vDir * 2.7f);

			++matrixIdx;
		}
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICICLEBEAM:
		for (auto& iter : m_vecAttackCol)
		{
			tmpMat = m_pTransform->Get_WorldMat();

			ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);
			LookPos = *V3_NORMAL(&_v3(), &_v3(tmpMat._31, tmpMat._32, tmpMat._33));

			iter->Update(ColPos + LookPos * 3.f);

			++matrixIdx;
		}
		break;

	case PET_BULLET_TYPE::PET_BULLET_FIRE:
		break;

	case PET_BULLET_TYPE::PET_BULLET_NONE:
		break;
	}


	return;
}

void CPet_Bullet::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CPet_Bullet::Check_CollisionEvent()
{
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
}

void CPet_Bullet::Check_CollisionHit(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bIsAttack)
		return;

	_bool bFirst = true;

	for (auto& iter : plistGameObject)
	{
		if (false == iter->Get_Target_CanHit())
			continue;

		for (auto& vecIter : m_vecAttackCol)
		{
			if (false == vecIter->Get_Enabled())
				continue;

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
						//iter->Set_Target_CanHit(false);

						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						iter->Add_Target_Hp(m_tObjParam.fDamage);
					}

					vecIter->Set_Enabled(false);

					switch (m_eBulletType)
					{
					case Client::CPet::PET_BULLET_POISON:
						break;
					case Client::CPet::PET_BULLET_ICE:
						break;
					case Client::CPet::PET_BULLET_ICICLEBLADE:
						break;
					case Client::CPet::PET_BULLET_ICICLEBEAM:
						break;
					case Client::CPet::PET_BULLET_FIRE:
						break;
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

void CPet_Bullet::Play_Deformation()
{
	return;
}

HRESULT CPet_Bullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPet_Bullet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CPet_Bullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);
	
	_float fRadius = 0.f;
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	switch (m_eBulletType)
	{
	case PET_BULLET_TYPE::PET_BULLET_POISON:
		fRadius = 0.7f;
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICE:
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICICLEBLADE:
		fRadius = 1.7f;
		break;

	case PET_BULLET_TYPE::PET_BULLET_ICICLEBEAM:
		fRadius = 1.5f;
		break;

	case PET_BULLET_TYPE::PET_BULLET_FIRE:
		break;

	case PET_BULLET_TYPE::PET_BULLET_NONE:
		break;
	}

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransform->Get_WorldMat().m[3][0], m_pTransform->Get_WorldMat().m[3][1], m_pTransform->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CPet_Bullet::Ready_Effect(void * pArg)
{
	PET_BULLET_STATUS temp = *(PET_BULLET_STATUS*)(pArg);

	switch (temp.eType)
	{
	case PET_BULLET_TYPE::PET_BULLET_POISON:
	{
		m_pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"ButterFly_VenomShot_Body", nullptr));
		m_pEffect->Set_Desc(_v3(0, 0, 0), m_pTransform);
		m_pEffect->Set_Loop(true);
		m_pEffect->Reset_Init();
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);

		m_eBulletType = temp.eType;

		m_vDir = temp.vDir;
		m_fSpeed = temp.fSpeed;
		m_dLifeTime = temp.dLifeTime;

		m_pTransform->Set_Pos(temp.vCreatePos);
		m_pTransform->Set_Scale(V3_ONE);

		m_tObjParam.bCanAttack = true;
		m_tObjParam.fDamage = 4.f;

		m_dCurTime = 0;
		m_bDead = false;
		m_bEffect = true;
		m_fEffectOffset = 0.f;
	}
		break;
	case PET_BULLET_TYPE::PET_BULLET_ICE:
		break;
	case PET_BULLET_TYPE::PET_BULLET_ICICLEBLADE:
	//{
	//	m_dLifeTime = temp.dLifeTime;

	//	m_dCurTime = 0;
	//	m_bDead = false;

	//	m_pTransform->Set_Pos(V3_NULL);
	//	m_pTransform->Set_Scale(V3_ONE);
	//	m_vAngle = V3_NULL;

	//	// »ç½¿¿Õ ºí·¢º¸µå¿¡¼­ °¡Á®¿È.
	//	m_vDir = m_pTarget_AIController->Get_V3Value(L"IceSword_Dir");

	//	//_mat matAttatch = m_pTarget_AIController->Get_MatValue(L"Bone_LeftHandAttach");
	//	//_mat matParent = m_pTarget_AIController->Get_MatValue(L"Self_Mat");
	//	//m_pTransform->Calc_ParentMat(&(matAttatch * matParent));

	//	//m_pTransform->Set_Pos(m_pTarget_AIController->Get_V3Value(L"IceSword_Pos"));
	//	//m_pTransform->Set_Scale(_v3(1.f, 1.f, 1.f));

	//	m_tObjParam.bCanAttack = true;
	//	m_tObjParam.fDamage = 20.f;

	//	m_pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"DeerKing_IceSword", nullptr));
	//	m_pEffect->Set_Desc(V3_NULL, nullptr);
	//	m_pEffect->Set_Angle(_v3(0.f, 0.f, 90.f));
	//	m_pEffect->Set_ParentObject(this);
	//	m_pEffect->Set_ZWrite();
	//	m_pEffect->Reset_Init();
	//	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);
	//}
		break;
	case PET_BULLET_TYPE::PET_BULLET_ICICLEBEAM:
	{
		m_vDir = temp.vDir;
		m_dLifeTime = 1;

		m_dCurTime = 0;
		m_bDead = false;
		m_fEffectOffset = 0.f;

		m_pTransform->Set_Pos(temp.vCreatePos);
		m_pTransform->Set_Scale(V3_ONE);

		m_tObjParam.bCanAttack = true;
		m_tObjParam.fDamage = 600.f;

		m_vDir += _v3(0.f, _float(CCalculater::Random_Num_Double(0.3, 0.45)), 0.f);

		_v3	vRight = *D3DXVec3Cross(&vRight, &WORLD_UP, &m_vDir);
		V3_NORMAL_SELF(&vRight);
		_float	fDot = acosf(D3DXVec3Dot(&WORLD_LOOK, &m_vDir));
		if (vRight.z > 0)
			fDot *= -1.f;
		m_pTransform->Set_Angle(_v3(0.f, fDot, 0.f));

		m_pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"DeerKing_IceBlock_0", nullptr));
		m_pEffect->Set_Desc(V3_NULL, nullptr);
		m_pEffect->Set_ParentObject(this);
		m_pEffect->Set_ZWrite();
		m_pEffect->Reset_Init();
		g_pManagement->Add_GameOject_ToLayer_NoClone(m_pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);

		for (_int i = 0; i < 5; i++)
			CParticleMgr::Get_Instance()->Create_Effect_Delay(L"DeerKing_Snow_Up_LongLine_1", 0.15f, m_pTransform->Get_Pos() + WORLD_UP + m_vDir * 0.3f, nullptr);

	}
		break;
	case PET_BULLET_TYPE::PET_BULLET_FIRE:
		break;
	case PET_BULLET_TYPE::PET_BULLET_NONE:
		break;
	}

	return S_OK;
}

CPet_Bullet* CPet_Bullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPet_Bullet* pInstance = new CPet_Bullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Bullet::Clone_GameObject(void * pArg)
{
	CPet_Bullet* pInstance = new CPet_Bullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Bullet::Free()
{
	Safe_Release(m_pEffect);

	CPet::Free();

	return;
}


