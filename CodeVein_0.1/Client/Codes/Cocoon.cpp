#include "stdafx.h"
#include "..\Headers\Cocoon.h"
#include "..\Headers\CocoonBullet.h"

CCocoon::CCocoon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster(pGraphic_Device)
{
}

CCocoon::CCocoon(const CCocoon & rhs)
	: CMonster(rhs)
{
}

HRESULT CCocoon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CCocoon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_Status(pArg);
	Ready_BoneMatrix();
	Ready_Collider();

	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBone[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	return S_OK;
}

_int CCocoon::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	m_pMonsterUI->Update_GameObject(TimeDelta);

	Check_PosY();
	Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	MONSTER_STATE_TYPE::DEAD != m_eFirstCategory ? Check_CollisionEvent() : Check_DeadEffect(TimeDelta);

	return S_OK;
}

_int CCocoon::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (!m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	m_dTimeDelta = TimeDelta;

	return S_OK;
}

HRESULT CCocoon::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul); // * alpha

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

			if (m_bDissolve)
				m_iPass = 3;

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	Update_Collider();
	Render_Collider();

	return S_OK;
}

HRESULT CCocoon::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(0.f);

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_bool bMotionBlur = true;
	if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
		return E_FAIL;
	_bool bDecalTarget = false;
	if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
		return E_FAIL;
	m_matLastWVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return S_OK;
}

void CCocoon::Update_Collider()
{
	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[Bone_Head] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(matTemp._41, matTemp._42, matTemp._43);

		vector_iter->Update(ColPos);
	}

	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat = *m_matBone[matrixIdx] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return;
}

void CCocoon::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CCocoon::Check_PosY()
{
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return;
}

void CCocoon::Check_Hit()
{
	if (MONSTER_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (true == m_tObjParam.bIsHit)
			{
				if (true == m_tObjParam.bHitAgain)
				{
					m_eFirstCategory = MONSTER_STATE_TYPE::HIT;
					m_pMeshCom->Reset_OldIndx();
					m_tObjParam.bHitAgain = false;

					if (nullptr == m_pTarget)
						m_eFBLR = FBLR::FRONTLEFT;
					else
						Function_FBLR(m_pTarget);
				}
				else
				{
					m_eFirstCategory = MONSTER_STATE_TYPE::HIT;

					if (nullptr == m_pTarget)
						m_eFBLR = FBLR::FRONTLEFT;
					else
						Function_FBLR(m_pTarget);
				}
			}
		}
	}
	else
		m_eFirstCategory = MONSTER_STATE_TYPE::DEAD;

	return;
}

void CCocoon::Check_Dist()
{
	if (MONSTER_STATE_TYPE::HIT == m_eFirstCategory ||
		MONSTER_STATE_TYPE::CC == m_eFirstCategory ||
		MONSTER_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsHit)
		return;

	Function_Find_Target();

	if (nullptr == m_pTarget)
	{
		Function_ResetAfterAtk();
		m_bInRecognitionRange = false;
		m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

		return;
	}
	else
	{
		_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(m_pTarget)->Get_Pos() - m_pTransformCom->Get_Pos()));

		m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
		m_fShotRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

		if (true == m_bInRecognitionRange)
		{
			if (true == m_bInAtkRange)
			{
				if (true == m_tObjParam.bCanAttack)
				{
					m_eFirstCategory = MONSTER_STATE_TYPE::ATTACK;
					if (m_fAtkRange < fLenth && m_fShotRange > fLenth)
						m_eState = COCOON_ANI::Shot;
					else
						m_eState = COCOON_ANI::Mist;
				}
				else
					m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
			}
			else
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
		}
		else
			m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
	}

	return;
}

void CCocoon::Check_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_STATE_TYPE::IDLE:
		Play_Idle();
		break;

	case MONSTER_STATE_TYPE::ATTACK:
		if(COCOON_ANI::Shot== m_eState)
			Play_Shot();
		else if(COCOON_ANI::Mist == m_eState)
			Play_Mist();
		break;

	case MONSTER_STATE_TYPE::HIT:
		Play_Hit();
		break;

	case MONSTER_STATE_TYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CCocoon::Check_DeadEffect(_double TimeDelta)
{
	m_fDeadEffect_Delay -= _float(TimeDelta);
	if (m_fDeadEffect_Delay > 0.f)
		return;

	m_fDeadEffect_Offset -= _float(TimeDelta);
	if (m_fDeadEffect_Offset > 0.f)
		return;

	m_fDeadEffect_Offset = 0.1f;

	_v3 vPos = m_pTransformCom->Get_Pos();
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head");
	_v3 vHeadPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Hips");
	_v3 vHipPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);

	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, vPos, vHeadPos);
	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, vPos, vHeadPos);

	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHeadPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHipPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vPos);

	return;
}

void CCocoon::Play_Shot()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_mat matBone;
	_v3 vBirth;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else 
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.f;

			return;
		}
		else if (6.500f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				matBone = *m_matBone[Bone_Jaw_Tongue] * m_pTransformCom->Get_WorldMat();
				memcpy(vBirth, &matBone._41, sizeof(_v3));

				if(MONSTER_COLOR_TYPE::WHITE == m_eMonsterColor) //얼음 쏘세요
					CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_CocoonBigBullet", &MONSTER_BULLET_STATUS(
						MONSTER_BULLET_TYPE::BULLET_ICE, vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 6.f, 2.5f));
				else
					CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_CocoonBigBullet", &MONSTER_BULLET_STATUS(
						MONSTER_BULLET_TYPE::BULLET_FIRE, vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 6.f, 2.5f));
					//CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_CocoonBigBullet", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 6.f, 1.5f));
			}
		}
		else if (3.7f <= AniTime && 5.8f >= AniTime)
		{
			matBone = *m_matBone[Bone_Jaw_Tongue] * m_pTransformCom->Get_WorldMat();
			memcpy(vBirth, &matBone._41, sizeof(_v3));

			g_pManagement->Create_Effect_Offset(L"Cocoon_TongueFire", 0.05f, vBirth, nullptr);
			g_pManagement->Create_Effect_Offset(L"FireBoy_FireBullet_Particle_01", 0.1f, vBirth, nullptr);
			g_pManagement->Create_Effect_Offset(L"FireBoy_FireBullet_Particle_02", 0.1f, vBirth, nullptr);
		}
		else if (0.f <= AniTime)
		{
			if (nullptr == m_pTarget)
			{
				Function_Find_Target();

				if (nullptr == m_pTarget)
				{
					Function_ResetAfterAtk();
					m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
					m_bInRecognitionRange = false;
					m_fCoolDownMax = 0.f;
					m_fCoolDownCur = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
				Function_RotateBody(m_pTarget);
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_tObjParam.bSuperArmor = true;
	}

	return;
}

void CCocoon::Play_Mist()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_mat matBone, matBone1, matBone2;
	_v3 vBirth, vMakeDirPoint1, vMakeDirPoint2, vShotDir;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		//3.033 생성(머금기) 3.567 방사 6.900 종료
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.8f;

			return;
		}
		else if (6.900f <= AniTime)
		{
			if (true == m_bEventTrigger[0])
				m_bEventTrigger[0] = false;
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
				m_bEventTrigger[0] = true;
			if (true == m_bEventTrigger[0])
			{	
				matBone1 = *m_matBone[Bone_Jaw_Tongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Neck] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_Jaw_Tongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				if(MONSTER_COLOR_TYPE::WHITE == m_eMonsterColor)
					CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_CocoonBullet", &MONSTER_BULLET_STATUS(
						MONSTER_BULLET_TYPE::BULLET_ICE, vBirth, vShotDir, 4.f, 1.f));
				else
					CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_CocoonBullet", &MONSTER_BULLET_STATUS(
						MONSTER_BULLET_TYPE::BULLET_FIRE, vBirth, vShotDir, 4.f, 1.f));
			}
		}
		else if (0.5f <= AniTime && 2.6f >= AniTime)
		{
			matBone = *m_matBone[Bone_Jaw_Tongue] * m_pTransformCom->Get_WorldMat();
			memcpy(vBirth, &matBone._41, sizeof(_v3));

			g_pManagement->Create_Effect_Offset(L"Cocoon_TongueFire", 0.05f, vBirth, nullptr);
			g_pManagement->Create_Effect_Offset(L"FireBoy_FireBullet_Particle_01", 0.1f, vBirth, nullptr);
			g_pManagement->Create_Effect_Offset(L"FireBoy_FireBullet_Particle_02", 0.1f, vBirth, nullptr);
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_tObjParam.bSuperArmor = true;
	}

	return;
}

void CCocoon::Play_Idle()
{
	if (true == m_bInRecognitionRange)
	{
		m_bIsIdle = false;

		if (true == m_tObjParam.bCanAttack)
		{
			m_eState = COCOON_ANI::Threat;

			if (nullptr == m_pTarget)
			{
				Function_Find_Target();

				if (nullptr == m_pTarget)
				{
					Function_ResetAfterAtk();
					m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
					m_bInRecognitionRange = false;
					m_fCoolDownMax = 0.f;
					m_fCoolDownCur = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
				Function_RotateBody(m_pTarget);
		}
		else
		{
			m_eState = COCOON_ANI::Threat;
			if (nullptr == m_pTarget)
			{
				Function_Find_Target();

				if (nullptr == m_pTarget)
				{
					Function_ResetAfterAtk();
					m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
					m_bInRecognitionRange = false;
					m_fCoolDownMax = 0.f;
					m_fCoolDownCur = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
				Function_RotateBody(m_pTarget);
		}
	}
	else
	{
		m_bIsIdle = true;
		m_eState = COCOON_ANI::Idle;
	}

	return;
}

void CCocoon::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;

		switch (m_eFBLR)
		{
		case Client::CMonster::FRONT:
		case Client::CMonster::FRONTLEFT:
		case Client::CMonster::FRONTRIGHT:
		case Client::CMonster::LEFT:
			m_eState = COCOON_ANI::Dmg_F;
			break;
		case Client::CMonster::BACK:
		case Client::CMonster::BACKLEFT:
		case Client::CMonster::BACKRIGHT:
		case Client::CMonster::RIGHT:
			m_eState = COCOON_ANI::Dmg_B;
			break;
		}
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_tObjParam.bCanHit = true;
			m_tObjParam.bIsHit = false;
			m_bCanCoolDown = true;

			m_fCoolDownMax = 0.5f;

			m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				m_tObjParam.bCanHit = true;
				if (nullptr == m_pTarget)
					m_eFBLR = FBLR::FRONTLEFT;
				else
					Function_FBLR(m_pTarget);
			}
		}
	}

	return;
}

void CCocoon::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();
		m_bCanPlayDead = true;
		m_eState = COCOON_ANI::Dead;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bEnable = false;
			m_dAniPlayMul = 0;
		}
		else if (3.733f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				Start_Dissolve(0.8f, false, true, 0.0f);
				m_fDeadEffect_Delay = 0.f;
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.f));
			}
		}
	}

	return;
}

HRESULT CCocoon::Add_Component(void* pArg)
{
	_tchar MeshName[STR_128] = L"";

	MONSTER_STATUS eTemp = *(MONSTER_STATUS*)pArg;

	if (nullptr == pArg)
		lstrcpy(MeshName, L"Mesh_Cocoon_Black");
	else
	{
		switch (eTemp.eMonsterColor)
		{
		case MONSTER_COLOR_TYPE::WHITE:
			lstrcpy(MeshName, L"Mesh_Cocoon_White");
			break;
		case MONSTER_COLOR_TYPE::RED:
		case MONSTER_COLOR_TYPE::BLUE:
		case MONSTER_COLOR_TYPE::YELLOW:
		case MONSTER_COLOR_TYPE::COLOR_NONE:
		case MONSTER_COLOR_TYPE::BLACK:
			lstrcpy(MeshName, L"Mesh_Cocoon_Black");
			break;
		}
	}
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, MeshName, L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.6f, 0.6f, 0.6f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CCocoon::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);

	CManagement* pManagement = CManagement::Get_Instance();
	IF_NULL_VALUE_RETURN(pManagement, E_FAIL);

	Safe_AddRef(pManagement);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CCocoon::Ready_Status(void * pArg)
{
	if (nullptr != pArg)
	{
		MONSTER_STATUS info = *(MONSTER_STATUS*)pArg;

		if (MONSTER_COLOR_TYPE::WHITE == info.eMonsterColor)
			m_eMonsterColor = info.eMonsterColor;
		else
			m_eMonsterColor = MONSTER_COLOR_TYPE::COLOR_NONE;
	}
	else
		m_eMonsterColor = MONSTER_COLOR_TYPE::COLOR_NONE;

	m_tObjParam.fDamage = 250.f;
	m_tObjParam.fHp_Max = 750.f;
	m_tObjParam.fArmor_Max = 100.f;

	m_fRecognitionRange = 30.f;
	m_fShotRange = 20.f;
	m_fAtkRange = 5.f;
	m_fPersonalRange = 2.f;

	m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fArmor_Cur = m_tObjParam.fArmor_Max;

	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;
	m_tObjParam.bCanAttack = true;
	m_tObjParam.bIsAttack = false;
	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;
	m_bCanPlayDead = false;
	m_bInRecognitionRange = false;
	m_bInAtkRange = false;
	m_bCanChase = false;
	m_bCanCoolDown = false;
	m_bIsCoolDown = false;
	m_bCanChooseAtkType = true;
	m_bIsCombo = false;
	m_bCanIdle = true;
	m_bIsIdle = false;
	m_bCanMoveAround = true;
	m_bIsMoveAround = false;

	m_dTimeDelta = 0;
	m_dAniPlayMul = 1;

	m_fSkillMoveSpeed_Cur = 0.f;
	m_fSkillMoveSpeed_Max = 0.f;
	m_fSkillMoveAccel_Cur = 0.5f;
	m_fSkillMoveAccel_Max = 0.f;
	m_fSkillMoveMultiply = 1.f;

	m_fCoolDownMax = 0.f;
	m_fCoolDownCur = 0.f;

	return S_OK;
}

HRESULT CCocoon::Ready_Collider()
{
	m_vecPhysicCol.reserve(3); 

	CCollider* pCollider = nullptr;
	_float fRadius;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 1.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	return S_OK;
}

HRESULT CCocoon::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Jaw_Tongue", 0), E_FAIL);
	m_matBone[Bone_Jaw_Tongue] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Neck", 0), E_FAIL);
	m_matBone[Bone_Neck] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CCocoon* CCocoon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCocoon* pInstance = new CCocoon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCocoon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCocoon::Clone_GameObject(void * pArg)
{
	CCocoon* pInstance = new CCocoon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CCocoon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCocoon::Free()
{
	Safe_Release(m_pMonsterUI);

	IF_NOT_NULL(m_pTarget)
		Safe_Release(m_pTarget);

	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}
