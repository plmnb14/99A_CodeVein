#include "stdafx.h"
#include "..\Headers\Wolf.h"

CWolf::CWolf(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster(pGraphic_Device)
{
}

CWolf::CWolf(const CWolf & rhs)
	: CMonster(rhs)
{
}

HRESULT CWolf::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CWolf::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_Status(pArg);
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Rigid();

	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBone[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	return S_OK;
}

_int CWolf::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);
	m_pRigidCom->Update_Component_Self(TimeDelta);

	// 떨어지면 1초 뒤 죽임.
	if (m_pRigidCom->Get_CurTime() > 1.f)
		m_eFirstCategory = MONSTER_STATE_TYPE::DEAD;

	m_pMonsterUI->Update_GameObject(TimeDelta);

	Check_PosY();
	Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	MONSTER_STATE_TYPE::DEAD != m_eFirstCategory ? Check_CollisionEvent() : Check_DeadEffect(TimeDelta);

	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================


	return S_OK;
}

_int CWolf::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (!m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (m_bInFrustum)
	{
		if (false == m_bDissolve)
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;
		}
	}

	m_dTimeDelta = TimeDelta;

	return NO_EVENT;
}

HRESULT CWolf::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul); // * alpha

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
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
	}

	if (MONSTER_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}

	return S_OK;
}

HRESULT CWolf::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

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

				pShader->Begin_Pass(m_iPass);

				pShader->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				pShader->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				pShader->End_Pass();
			}
		}

	}

	if (MONSTER_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}

	return S_OK;
}

HRESULT CWolf::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.5f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 실행
	//============================================================================================
	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pMeshCom->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);
			pShader->Commit_Changes();

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return S_OK;
}

void CWolf::Update_Collider()
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

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));

	return;
}

void CWolf::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CWolf::Check_PosY()
{
	if (m_pRigidCom->Get_IsFall() == false)
		m_pTransformCom->Set_Pos(m_pNavMeshCom->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));
	else
	{
		_float fYSpeed = m_pRigidCom->Set_Fall(m_pTransformCom->Get_Pos(), _float(m_dTimeDelta));

		D3DXVECTOR3 JumpLength = { 0, -fYSpeed, 0 };
		m_pTransformCom->Add_Pos(JumpLength);
	}
	return;
}

void CWolf::Check_Hit()
{
	if (MONSTER_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			++m_iDodgeCount;
			if (m_iDodgeCount >= m_iDodgeCountMax)
			{
				m_iDodgeCount = 0;
				m_tObjParam.bCanDodge = true;
				m_eFirstCategory = MONSTER_STATE_TYPE::MOVE;
				m_eSecondCategory_MOVE = MONSTER_MOVE_TYPE::MOVE_DODGE;
				m_pMeshCom->Reset_OldIndx();
			}
			else
			{
				if (true == m_tObjParam.bIsHit)
				{
					if (true == m_tObjParam.bHitAgain)
					{
						m_bEventTrigger[0] = false;
						m_eFirstCategory = MONSTER_STATE_TYPE::HIT;
						m_tObjParam.bHitAgain = false;
						m_pMeshCom->Reset_OldIndx();

						if (nullptr == m_pAggroTarget)
							m_eFBLR = FBLR::FRONTLEFT;
						else
							Function_FBLR(m_pAggroTarget);
					}
					else
					{
						m_eFirstCategory = MONSTER_STATE_TYPE::HIT;

						if (nullptr == m_pAggroTarget)
							m_eFBLR = FBLR::FRONTLEFT;
						else
							Function_FBLR(m_pAggroTarget);
					}
				}
			}
		}
	}
	else
		m_eFirstCategory = MONSTER_STATE_TYPE::DEAD;

	return;
}

void CWolf::Check_Dist()
{
	if (MONSTER_STATE_TYPE::HIT == m_eFirstCategory ||
		MONSTER_STATE_TYPE::CC == m_eFirstCategory ||
		MONSTER_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_bIsCombo ||
		true == m_bIsMoveAround ||
		true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsDodge ||
		true == m_tObjParam.bIsHit)
		return;

	Function_Find_Target();

	if (nullptr == m_pAggroTarget)
	{
		if (MONSTER_STATE_TYPE::IDLE == m_eFirstCategory)
			return;
		else
		{
			Function_ResetAfterAtk();

			m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

			switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
			{
			case MONSTER_IDLE_TYPE::IDLE_IDLE:
				m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
				m_eState = WOLF_ANI::Idle;
				break;
			case MONSTER_IDLE_TYPE::IDLE_STAND:
			case MONSTER_IDLE_TYPE::IDLE_CROUCH:
			case MONSTER_IDLE_TYPE::IDLE_EAT:
				m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
				m_eState = WOLF_ANI::NF_Eat;
				break;
			case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
			case MONSTER_IDLE_TYPE::IDLE_SIT:
			case MONSTER_IDLE_TYPE::IDLE_LURK:
				m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
				m_eState = WOLF_ANI::NF_Sit;
				break;
			}	

			return;
		}
	}
	else
	{
		_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(m_pAggroTarget)->Get_Pos() - m_pTransformCom->Get_Pos()));

		m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
		m_fAtkRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

		if (true == m_bInRecognitionRange)
		{
			if (true == m_bIsIdle)
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
			else
			{
				if (true == m_bInAtkRange)
				{
					if (true == m_tObjParam.bCanAttack)
						m_eFirstCategory = MONSTER_STATE_TYPE::ATTACK;
					else
					{
						m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
					}
				}
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = MONSTER_STATE_TYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVE_TYPE::MOVE_RUN;
				}
			}
		}
		else
		{
			m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

			if (false == m_bIsIdle)
			{
				switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
				{
				case MONSTER_IDLE_TYPE::IDLE_IDLE:
					m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
					m_eState = WOLF_ANI::Idle;
					break;
				case MONSTER_IDLE_TYPE::IDLE_STAND:
				case MONSTER_IDLE_TYPE::IDLE_CROUCH:
				case MONSTER_IDLE_TYPE::IDLE_EAT:
					m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
					m_eState = WOLF_ANI::NF_Eat;
					break;
				case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
				case MONSTER_IDLE_TYPE::IDLE_SIT:
				case MONSTER_IDLE_TYPE::IDLE_LURK:
					m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
					m_eState = WOLF_ANI::NF_Sit;
					break;
				}
			}
		}
	}

	return;
}

void CWolf::Check_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_STATE_TYPE::IDLE:
		Play_Idle();
		break;

	case MONSTER_STATE_TYPE::MOVE:
		Play_Move();
		break;

	case MONSTER_STATE_TYPE::ATTACK:
		if (false == m_tObjParam.bIsAttack)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;

			switch (CALC::Random_Num(WOLF_ANI::Atk_Third, WOLF_ANI::Atk_Jump))
			{
			case WOLF_ANI::Atk_Third:
				m_eState = WOLF_ANI::Atk_Third;
				break;
			case WOLF_ANI::Atk_Jump_R90:
				m_eState = WOLF_ANI::Atk_Jump_R90;
				break;
			case WOLF_ANI::Atk_Jump_L90:
				m_eState = WOLF_ANI::Atk_Jump_L90;
				break;
			case WOLF_ANI::Atk_Jump:
				m_eState = WOLF_ANI::Atk_Jump;
				break;
			}
			
			return;
		}
		else
		{
			switch (m_eState)
			{
			case WOLF_ANI::Atk_Third:
				Play_Bite_LRL();
				break;
			case WOLF_ANI::Atk_Jump_R90:
				Play_RDodgeAtk();
				break;
			case WOLF_ANI::Atk_Jump_L90:
				Play_LDodgeAtk();
				break;
			case WOLF_ANI::Atk_Jump:
				Play_Frisbee();
				break;
			}
		}
		break;
	case MONSTER_STATE_TYPE::HIT:
		Play_Hit();
		break;

	case MONSTER_STATE_TYPE::CC:
		Play_CC();
		break;

	case MONSTER_STATE_TYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CWolf::Check_DeadEffect(_double TimeDelta)
{
	m_fDeadEffect_Delay -= _float(TimeDelta);
	if (m_fDeadEffect_Delay > 0.f)
		return;

	m_fDeadEffect_Offset -= _float(TimeDelta);
	if (m_fDeadEffect_Offset > 0.f)
		return;

	m_fDeadEffect_Offset = 0.15f;

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

void CWolf::Play_Bite_LRL()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

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
			m_fCoolDownMax = 2.f;

			return;
		}
		else if (4.600 <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.200 <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_01);
				g_pSoundManager->Play_Sound(L"Wolf_Bite4.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
			}
		}
		else if (3.367 <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.167 <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_02);
				g_pSoundManager->Play_Sound(L"Wolf_Bite4.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
			}
		}
		else if (2.167 <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.367 <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_01);
				g_pSoundManager->Play_Sound(L"Wolf_Bite4.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
			}
		}

		if (4.433f < AniTime && 4.933f > AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (3.233f < AniTime && 3.567f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.967f < AniTime && 2.467f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CWolf::Play_RDodgeAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

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
			m_fCoolDownMax = 0.8f;

			return;
		}
		else if (2.367f <= AniTime)
		{
			if(false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_01);
				g_pSoundManager->Play_Sound(L"Wolf_Bite3.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
			}
		}

		if (1.533f < AniTime && 2.367 > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.500f < AniTime && 1.233f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CWolf::Play_LDodgeAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

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
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;

			return;
		}
		else if (2.367f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_01);
				g_pSoundManager->Play_Sound(L"Wolf_Bite2.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
			}
		}

		if (1.533f < AniTime && 2.700f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.467f < AniTime && 1.233f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CWolf::Play_Frisbee()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

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
			m_fCoolDownMax = 0.8f;

			return;
		}
		else if (2.200f <= AniTime)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.100f <= AniTime)
		{
			if (m_bEventTrigger[1] == false)
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_01);
				g_pSoundManager->Play_Sound(L"Wolf_Bite1.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
			}
		}

		if(1.700f < AniTime && 2.067f >AniTime)
		{
			if (m_bEventTrigger[2] == false)
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CWolf::Play_Idle()
{
	switch (m_eSecondCategory_IDLE)
	{
	case MONSTER_IDLE_TYPE::IDLE_IDLE:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				m_bIsIdle = false;

				if (true == m_tObjParam.bCanAttack)
					m_eState = WOLF_ANI::Idle;
				else
				{
					if (nullptr == m_pAggroTarget)
					{
						Function_Find_Target();

						if (nullptr == m_pAggroTarget)
						{
							Function_ResetAfterAtk();
							m_fCoolDownMax = 0.f;
							m_fCoolDownCur = 0.f;
							m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

							if (false == m_bIsIdle)
							{
								switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
								{
								case MONSTER_IDLE_TYPE::IDLE_IDLE:
									m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
									break;
								case MONSTER_IDLE_TYPE::IDLE_CROUCH:
								case MONSTER_IDLE_TYPE::IDLE_EAT:
									m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_CROUCH;
									break;
								case MONSTER_IDLE_TYPE::IDLE_SIT:
								case MONSTER_IDLE_TYPE::IDLE_LURK:
									m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
									break;
								case MONSTER_IDLE_TYPE::IDLE_STAND:
								case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
									m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_STAND;
									break;
								}
							}

							Play_Idle();

							return;
						}
						else
							Function_RotateBody(m_pAggroTarget);
					}
					else
						Function_RotateBody(m_pAggroTarget);

					m_eState = WOLF_ANI::Threat;
				}
			}
			else
			{
				m_bIsIdle = true;
				m_eState = WOLF_ANI::Idle;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = WOLF_ANI::Idle;
		}
		break;
	case MONSTER_IDLE_TYPE::IDLE_EAT:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				if (WOLF_ANI::NF_Eat == m_eState)
				{
					m_bIsIdle = true;

					if (m_pMeshCom->Is_Finish_Animation(0.5f))
						m_eState = WOLF_ANI::NF_Eat_End;
				}
				else if (WOLF_ANI::NF_Eat_End == m_eState)
				{
					if (m_pMeshCom->Is_Finish_Animation(0.95f))
					{
						m_bCanIdle = true;
						m_bIsIdle = false;
						m_eState = WOLF_ANI::Threat;
					}
				}
			}
			else
			{
				m_bIsIdle = true;
				m_eState = WOLF_ANI::NF_Eat;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = WOLF_ANI::NF_Eat;
		}
		break;
	case MONSTER_IDLE_TYPE::IDLE_SIT:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				if (WOLF_ANI::NF_Sit == m_eState)
				{
					m_bIsIdle = true;

					if (m_pMeshCom->Is_Finish_Animation(0.5f))
						m_eState = WOLF_ANI::NF_Sit_End;
				}
				else if (WOLF_ANI::NF_Sit_End == m_eState)
				{
					if (m_pMeshCom->Is_Finish_Animation(0.95f))
					{
						m_bCanIdle = true;
						m_bIsIdle = false;
						m_eState = WOLF_ANI::Threat;
					}
				}
			}
			else
			{
				m_bIsIdle = true;
				m_eState = WOLF_ANI::NF_Sit;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = WOLF_ANI::NF_Sit;
		}
		break;
	}

	return;
}

void CWolf::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVE_TYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = WOLF_ANI::Run;
			m_fSkillMoveSpeed_Cur = 4.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;
		}

		if (nullptr == m_pAggroTarget)
		{
			Function_Find_Target();

			if (nullptr == m_pAggroTarget)
			{
				Function_ResetAfterAtk();
				m_fCoolDownMax = 0.f;
				m_fCoolDownCur = 0.f;
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

				if (false == m_bIsIdle)
				{
					switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
					{
					case MONSTER_IDLE_TYPE::IDLE_IDLE:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
						break;
					case MONSTER_IDLE_TYPE::IDLE_STAND:
					case MONSTER_IDLE_TYPE::IDLE_CROUCH:
					case MONSTER_IDLE_TYPE::IDLE_EAT:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
						break;
					case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
					case MONSTER_IDLE_TYPE::IDLE_SIT:
					case MONSTER_IDLE_TYPE::IDLE_LURK:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
						break;
					}
				}

				Play_Idle();

				return;
			}
			else
				Function_RotateBody(m_pAggroTarget);
		}
		else
			Function_RotateBody(m_pAggroTarget);

		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(m_fSkillMoveMultiply);
		break;

	case MONSTER_MOVE_TYPE::MOVE_WALK:
		m_eState = WOLF_ANI::Walk;
		if (nullptr == m_pAggroTarget)
		{
			Function_Find_Target();

			if (nullptr == m_pAggroTarget)
			{
				Function_ResetAfterAtk();
				m_fCoolDownMax = 0.f;
				m_fCoolDownCur = 0.f;
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

				if (false == m_bIsIdle)
				{
					switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
					{
					case MONSTER_IDLE_TYPE::IDLE_IDLE:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
						break;
					case MONSTER_IDLE_TYPE::IDLE_STAND:
					case MONSTER_IDLE_TYPE::IDLE_CROUCH:
					case MONSTER_IDLE_TYPE::IDLE_EAT:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
						break;
					case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
					case MONSTER_IDLE_TYPE::IDLE_SIT:
					case MONSTER_IDLE_TYPE::IDLE_LURK:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
						break;
					}
				}

				Play_Idle();

				return;
			}
			else
				Function_RotateBody(m_pAggroTarget);
		}
		else
			Function_RotateBody(m_pAggroTarget);

		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		break;

	case MONSTER_MOVE_TYPE::MOVE_DODGE:
		if (true == m_tObjParam.bCanDodge)
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanDodge = false;
			m_tObjParam.bIsDodge = true;
			m_eState = WOLF_ANI::Dodge;
		}
		else
		{
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
				m_tObjParam.bCanAttack = true;
				Function_ResetAfterAtk();

				return;
			}
			else if (0.900f < AniTime && 1.833f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 10.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));

				Function_DecreMoveMent(m_fSkillMoveMultiply);

			}
		}
		break;
	}

	return;
}

void CWolf::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;

		switch (m_eFBLR)
		{
		case FBLR::FRONT:
		case FBLR::FRONTLEFT:
		case FBLR::FRONTRIGHT:
		case FBLR::LEFT:
			m_eState = WOLF_ANI::Dmg_F;
			break;
		case FBLR::BACK:
		case FBLR::BACKLEFT:
		case FBLR::BACKRIGHT:
		case FBLR::RIGHT:
			m_eState = WOLF_ANI::Dmg_B;
			break;
		}

		g_pSoundManager->Stop_Sound(CSoundManager::Wolf_SFX_01);
		g_pSoundManager->Play_Sound(L"Wolf_Hit.ogg", CSoundManager::Wolf_SFX_01, CSoundManager::Effect_Sound);
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();

			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.5f;

			m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

			return;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				m_tObjParam.bCanHit = true;
				if (nullptr == m_pAggroTarget)
					m_eFBLR = FBLR::FRONTLEFT;
				else
					Function_FBLR(m_pAggroTarget);
			}
		}
	}

	return;
}

void CWolf::Play_CC()
{
	return;
}

void CWolf::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	
	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();
		m_bCanPlayDead = true;
		m_eState = WOLF_ANI::Death;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bEnable = false;
			m_dAniPlayMul = 0;
		}
		else if (4.267f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				Start_Dissolve(0.9f, false, true, 0.3f);
				m_fDeadEffect_Delay = 0.3f;

				Give_Mana_To_Player(5);
				Check_DropItem(MONSTER_NAMETYPE::M_Wolf);
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.3f));
			}
		}
	}

	return;
}

HRESULT CWolf::Add_Component(void* pArg)
{
	_tchar MeshName[STR_128] = L"";

	MONSTER_STATUS eTemp = *(MONSTER_STATUS*)pArg;

	if (nullptr == pArg)
		lstrcpy(MeshName, L"Mesh_Wolf_Black");
	else
	{
		switch (eTemp.eMonsterColor)
		{
		case MONSTER_COLOR_TYPE::RED:
		case MONSTER_COLOR_TYPE::BLUE:
		case MONSTER_COLOR_TYPE::YELLOW:
		case MONSTER_COLOR_TYPE::COLOR_NONE:
		case MONSTER_COLOR_TYPE::BLACK:
			lstrcpy(MeshName, L"Mesh_Wolf_Black");
			break;
		case MONSTER_COLOR_TYPE::WHITE:
			lstrcpy(MeshName, L"Mesh_Wolf_White");
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

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMeshCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgentCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Rigidbody", L"Com_Rigidbody", (CComponent**)&m_pRigidCom)))
		return E_FAIL;

	m_pColliderCom->Set_Radius(_v3{ 0.6f, 0.6f, 0.6f });
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CWolf::SetUp_ConstantTable(CShader* pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 0.1f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.2f;	// ID_B	: B채널 ID 값 , 1이 최대

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 림라이트 값들을 쉐이더에 등록시킴
	//=============================================================================================
	m_pBattleAgentCom->Update_RimParam_OnShader(pShader);
	//=============================================================================================

	return S_OK;
}

HRESULT CWolf::Ready_Status(void * pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Create CWolf pArgument nullptr Failed");
		return E_FAIL;
	}

	MONSTER_STATUS Info = *(MONSTER_STATUS*)pArg;

	m_pBattleAgentCom->Set_RimAlpha(0.5f);
	m_pBattleAgentCom->Set_RimValue(8.f);
	m_pBattleAgentCom->Set_OriginRimAlpha(0.5f);
	m_pBattleAgentCom->Set_OriginRimValue(8.f);

	if (true == Info.bSpawnOnTrigger)
	{
		_tchar szNavData[STR_128] = L"";

		lstrcpy(szNavData, (
			Info.sStageIdx == 0 ? L"Navmesh_Training.dat" :
			Info.sStageIdx == 1 ? L"Navmesh_Stage_01.dat" :
			Info.sStageIdx == 2 ? L"Navmesh_Stage_02.dat" :
			Info.sStageIdx == 3 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

		m_pNavMeshCom->Set_Index(-1);
		m_pNavMeshCom->Ready_NaviMesh(m_pGraphic_Dev, szNavData);
		m_pNavMeshCom->Check_OnNavMesh(Info.vPos);
		m_pTransformCom->Set_Pos(Info.vPos);
		m_pTransformCom->Set_Angle(Info.vAngle);

		if (MONSTER_COLOR_TYPE::WHITE == Info.eMonsterColor)
		{
			m_eMonsterColor = Info.eMonsterColor;
			m_tObjParam.fDamage = 120.f * pow(1.5f, g_sStageIdx_Cur - 1);
			m_tObjParam.fHp_Max = 1200.f * pow(1.5f, g_sStageIdx_Cur - 1);
			m_tObjParam.fArmor_Max = 30.f * pow(1.5f, g_sStageIdx_Cur - 1);

			m_fRecognitionRange = 15.f;
			m_fShotRange = 10.f;
			m_fAtkRange = 5.f;
			m_fPersonalRange = 2.f;
			m_iDodgeCountMax = 5;
		}
		else
		{
			m_eMonsterColor = MONSTER_COLOR_TYPE::BLACK;
			m_tObjParam.fDamage = 120.f * pow(1.5f, g_sStageIdx_Cur - 1);
			m_tObjParam.fHp_Max = 1200.f * pow(1.5f, g_sStageIdx_Cur - 1);
			m_tObjParam.fArmor_Max = 30.f * pow(1.5f, g_sStageIdx_Cur - 1);

			m_fRecognitionRange = 15.f;
			m_fShotRange = 10.f;
			m_fAtkRange = 5.f;
			m_fPersonalRange = 2.f;
			m_iDodgeCountMax = 5;
		}
	}

	m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

	switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
	{
	case MONSTER_IDLE_TYPE::IDLE_STAND:
	case MONSTER_IDLE_TYPE::IDLE_IDLE:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
		m_eState = WOLF_ANI::Idle;
		break;

	case MONSTER_IDLE_TYPE::IDLE_CROUCH:
	case MONSTER_IDLE_TYPE::IDLE_EAT:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
		m_eState = WOLF_ANI::NF_Eat;
		break;

	case MONSTER_IDLE_TYPE::IDLE_LURK:
	case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
	case MONSTER_IDLE_TYPE::IDLE_SIT:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
		m_eState = WOLF_ANI::NF_Sit;
		break;
	}

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

HRESULT CWolf::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(2);

	_float fRadius;
	CCollider* pCollider = nullptr;

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

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CWolf::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine1", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CWolf::Ready_Rigid()
{
	if (nullptr != m_pRigidCom)
	{
		m_pRigidCom->Set_UseGravity(true);							// 중력의 영향 유무

		m_pRigidCom->Set_IsFall(false);								// 낙하중인지 체크

		m_pRigidCom->Set_fPower(2.f);								// 점프 파워

		m_pRigidCom->Set_Speed({ 10.f , 10.f , 10.f });				// 각 축에 해당하는 속도
		m_pRigidCom->Set_Accel({ 1.f, 0.f, 0.f });					// 각 축에 해당하는 Accel 값
		m_pRigidCom->Set_MaxAccel({ 2.f , 4.f , 2.f });				// 각 축에 해당하는 MaxAccel 값
	}

	return S_OK;
}

CWolf* CWolf::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CWolf* pInstance = new CWolf(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWolf::Clone_GameObject(void * pArg)
{
	CWolf* pInstance = new CWolf(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWolf::Free()
{
	CMonster::Free();

	return;
}
