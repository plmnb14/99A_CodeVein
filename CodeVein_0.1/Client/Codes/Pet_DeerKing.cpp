#include "stdafx.h"
#include "..\Headers\Pet_DeerKing.h"

CPet_DeerKing::CPet_DeerKing(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CPet(pGraphic_Device)
{
}

CPet_DeerKing::CPet_DeerKing(const CPet_DeerKing & rhs)
	: CPet(rhs)
{
}

HRESULT CPet_DeerKing::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPet_DeerKing::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_Status(pArg);
	Ready_BoneMatrix(pArg);
	Ready_Collider(pArg);
	Ready_Weapon(pArg);

	m_pPlayer = &(*g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	if (nullptr != m_pPlayer)
		Safe_AddRef(m_pPlayer);

	Function_Check_Navi();

	return S_OK;
}

_int CPet_DeerKing::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Play_Deformation();

	CGameObject::Update_GameObject(TimeDelta);

	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMesh->SetUp_Animation(m_eState);

	PET_STATE_TYPE::DEAD != m_eFirstCategory ? Check_CollisionEvent() : Check_DeadEffect(TimeDelta);

	m_bInFrustum = m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), 2.f);

	return NO_EVENT;
}

_int CPet_DeerKing::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	if (!m_bDissolve)
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (m_bInFrustum)
	{
		if (false == m_bDissolve)
		{
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;
		}
	}

	m_dTimeDelta = TimeDelta;

	return NO_EVENT;
}

HRESULT CPet_DeerKing::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMesh, E_FAIL);

	m_pMesh->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (FAILED(SetUp_ConstantTable(m_pShader)))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh->Get_NumMaterials(i);

		m_pMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			if (PET_STATE_TYPE::DEAD != m_eFirstCategory)
				m_iPass = m_pMesh->Get_MaterialPass(i, j);

			m_pShader->Begin_Pass(m_iPass);

			m_pShader->Set_DynamicTexture_Auto(m_pMesh, i, j);

			m_pShader->Commit_Changes();

			m_pMesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	if (PET_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}

	return S_OK;
}

HRESULT CPet_DeerKing::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMesh, E_FAIL);

	m_pMesh->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = _uint(m_pMesh->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMesh->Get_NumMaterials(i);

			m_pMesh->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMesh->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				pShader->Begin_Pass(m_iPass);

				pShader->Set_DynamicTexture_Auto(m_pMesh, i, j);

				pShader->Commit_Changes();

				m_pMesh->Render_Mesh(i, j);

				pShader->End_Pass();
			}
		}
	}

	if (PET_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}

	return S_OK;
}

HRESULT CPet_DeerKing::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMesh, E_FAIL);

	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

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
	_uint iNumMeshContainer = _uint(m_pMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pMesh->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);
			pShader->Commit_Changes();

			pShader->Commit_Changes();

			m_pMesh->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return S_OK;
}

void CPet_DeerKing::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[matrixIdx] * m_pTransform->Get_WorldMat();

		_v3 ColPos = _v3(matTemp._41, matTemp._42, matTemp._43);

		vector_iter->Update(ColPos);

		++matrixIdx;
	}

	m_pCollider->Update(m_pTransform->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return;
}

void CPet_DeerKing::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CPet_DeerKing::Check_Dist()
{
	if (PET_STATE_TYPE::HIT == m_eFirstCategory ||
		PET_STATE_TYPE::CC == m_eFirstCategory ||
		PET_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	//Function_Change_Mode();

	if (true == m_bIsSummon ||
		true == m_bIsMoveAround ||
		true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsDodge ||
		true == m_tObjParam.bIsHit)
		return;

	_float fPlayerDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransform->Get_Pos()));

	m_fLimitRange >= fPlayerDist ? m_bInLimitRange = true : m_bInLimitRange = false;
	m_fActiveRange >= fPlayerDist ? m_bInActiveRange = true : m_bInActiveRange = false;

	if (true == m_bInLimitRange)
	{
		if (true == m_bInActiveRange)
		{
			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					m_pTarget = nullptr;

					Function_ResetAfterAtk();
					m_tObjParam.bCanAttack = true;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Function_Find_Target();

					if (nullptr != m_pTarget)
						Check_Action();
					else
					{
						Function_CalcMoveSpeed(m_fPersonalRange);

						if (0.f >= m_fSkillMoveSpeed_Cur)
						{
							m_eFirstCategory = PET_STATE_TYPE::IDLE;
							m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
						}
						else
						{
							m_bCanChase = true;
							m_eFirstCategory = PET_STATE_TYPE::MOVE;
							m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
						}
					}
				}
				else
					Check_Action();
			}
			else
			{
				Function_Find_Target();

				if (nullptr != m_pTarget)
				{
					if (false == m_pTarget->Get_Enable() ||
						true == m_pTarget->Get_Dead())
					{
						Safe_Release(m_pTarget);
						m_pTarget = nullptr;

						Function_ResetAfterAtk();
						m_tObjParam.bCanAttack = true;
						m_fCoolDownCur = 0.f;
						m_fCoolDownMax = 0.f;

						Function_Find_Target();

						if (nullptr != m_pTarget)
							Check_Action();
						else
						{
							Function_CalcMoveSpeed(m_fAtkRange);

							if (0.f >= m_fSkillMoveSpeed_Cur)
							{
								m_eFirstCategory = PET_STATE_TYPE::IDLE;
								m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
							}

							else
							{
								m_bCanChase = true;
								m_eFirstCategory = PET_STATE_TYPE::MOVE;
								m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
							}
						}
					}
					else
						Check_Action();
				}
				else
				{
					Function_CalcMoveSpeed(m_fAtkRange);

					if (0.f >= m_fSkillMoveSpeed_Cur)
					{
						m_eFirstCategory = PET_STATE_TYPE::IDLE;
						m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
					}
					else
					{
						m_bCanChase = true;
						m_eFirstCategory = PET_STATE_TYPE::MOVE;
						m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
					}
				}
			}
		}
		else
		{
			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					m_pTarget = nullptr;
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;

					Function_ResetAfterAtk();
					m_tObjParam.bCanAttack = true;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Function_CalcMoveSpeed(m_fAtkRange);

					if (0.f >= m_fSkillMoveSpeed_Cur)
					{
						m_eFirstCategory = PET_STATE_TYPE::IDLE;
						m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
					}
					else
					{
						m_bCanChase = true;
						m_eFirstCategory = PET_STATE_TYPE::MOVE;
						m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
					}
				}
				else
					Check_Action();
			}
			else
			{
				Function_CalcMoveSpeed(m_fPersonalRange);

				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;

				if (0.f >= m_fSkillMoveSpeed_Cur)
				{
					m_eFirstCategory = PET_STATE_TYPE::IDLE;
					m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
				}
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = PET_STATE_TYPE::MOVE;
					m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
				}
			}
		}
	}
	else
	{
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;

			m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
		}

		Function_Teleport_Near_Player();
	}

	return;
}

void CPet_DeerKing::Check_Action()
{
	_float fTargetDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pTarget)->Get_Pos() - m_pTransform->Get_Pos()));

	m_fRecognitionRange >= fTargetDist ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fShotRange >= fTargetDist ? m_bInShotRange = true : m_bInShotRange = false;
	m_fAtkRange >= fTargetDist ? m_bInAtkRange = true : m_bInAtkRange = false;

	switch (m_eTarget)
	{
	case PET_TARGET_TYPE::PET_TARGET_BOSS:
	case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		if (true == m_bInRecognitionRange)
		{
			//근거리 범위o
			if (true == m_bInAtkRange)
			{
				if (true == m_tObjParam.bCanAttack)
					m_eFirstCategory = PET_STATE_TYPE::ATTACK;
				else
				{
					m_eFirstCategory = PET_STATE_TYPE::IDLE;
					m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
				}
			}
			else
			{
				m_eFirstCategory = PET_STATE_TYPE::MOVE;
				m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_WALK;
			}
		}
		else
		{
			m_bCanChase = true;
			m_eFirstCategory = PET_STATE_TYPE::MOVE;
			m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
		}
		break;
	case PET_TARGET_TYPE::PET_TARGET_ITEM:
		if (true == m_bInRecognitionRange)
		{
			Function_CalcMoveSpeed(m_fPersonalRange);

			if (0.f >= m_fSkillMoveSpeed_Cur)
			{
				m_eFirstCategory = PET_STATE_TYPE::IDLE;
				m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;

				if (nullptr != m_pTarget)
				{
					if (false == m_pTarget->Get_Enable() ||
						true == m_pTarget->Get_Dead())
					{
						Safe_Release(m_pTarget);
						m_pTarget = nullptr;
					}
				}
				else
					return;
			}
			else
			{
				m_eFirstCategory = PET_STATE_TYPE::MOVE;
				m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
			}
		}
		break;

	case PET_TARGET_TYPE::PET_TARGET_NONE:

		Function_CalcMoveSpeed(m_fActiveRange);

		if (0.f >= m_fSkillMoveSpeed_Cur)
		{
			m_eFirstCategory = PET_STATE_TYPE::IDLE;
			m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
		}
		else
		{
			m_bCanChase = true;
			m_eFirstCategory = PET_STATE_TYPE::MOVE;
			m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
		}
		break;
	}

	return;
}

void CPet_DeerKing::Check_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case PET_STATE_TYPE::IDLE:
		Play_Idle();
		break;
	case PET_STATE_TYPE::MOVE:
		Play_Move();
		break;
	case PET_STATE_TYPE::ATTACK:
		if (false == m_tObjParam.bIsAttack)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;

			if (true == m_bInAtkRange)
			{
				switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_CHOP, ATK_NORMAL_TYPE::NORMAL_COMBO))
				{
				case ATK_NORMAL_TYPE::NORMAL_CHOP:
					m_eState = PET_DEERKING_ANI::Atk_N;
					break;
				case ATK_NORMAL_TYPE::NORMAL_SNOWQUAKE:
					m_eState = PET_DEERKING_ANI::Shield_Atk_N04;
					break;
				case ATK_NORMAL_TYPE::NORMAL_COMBO:
					m_eState = PET_DEERKING_ANI::Shield_Atk_N01;
					break;
				}
			}

			return;
		}
		else
		{
			switch (m_eState)
			{
			case PET_DEERKING_ANI::Atk_N:
				Play_Chop();
				break;

			case PET_DEERKING_ANI::Shield_Atk_N04:
				Play_SnowQuake();
				break;

			case PET_DEERKING_ANI::Shield_Atk_N01:
			case PET_DEERKING_ANI::Shield_Atk_N02:
			case PET_DEERKING_ANI::Shield_Atk_N03:
				Play_Combo();
				break;
			}
		}
		break;
	case PET_STATE_TYPE::HIT:
		Play_Hit();
		break;
	case PET_STATE_TYPE::CC:
		Play_CC();
		break;
	case PET_STATE_TYPE::DEAD:
		Play_Dead();
		break;
	}
	
	return;
}

void CPet_DeerKing::Check_DeadEffect(_double TimeDelta)
{
	m_fDeadEffect_Delay -= _float(TimeDelta);
	if (m_fDeadEffect_Delay > 0.f)
		return;

	m_fDeadEffect_Offset -= _float(TimeDelta);
	if (m_fDeadEffect_Offset > 0.f)
		return;

	m_fDeadEffect_Offset = 0.1f;

	_v3 vPos = m_pTransform->Get_Pos();
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMesh->Get_BonInfo("Head");
	_v3 vHeadPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat()).m[3]);
	pFamre = (D3DXFRAME_DERIVED*)m_pMesh->Get_BonInfo("Hips");
	_v3 vHipPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransform->Get_WorldMat()).m[3]);

	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, vPos, vHeadPos);
	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, vPos, vHeadPos);

	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHeadPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHipPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vPos);

	return;
}

void CPet_DeerKing::Play_Combo()
{
	_double AniTime = m_pMesh->Get_TrackInfo().Position;

	if (PET_DEERKING_ANI::Shield_Atk_N01 == m_eState)
	{
		if (m_pMesh->Is_Finish_Animation(0.6f))
		{
			m_tObjParam.bSuperArmor = false;
			m_eState = Shield_Atk_N02;
			return;
		}
		else if (2.000f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (1.733f < AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		if (0.500f < AniTime && 1.800f> AniTime)
		{
			if (m_bEventTrigger[2] == false)
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (PET_DEERKING_ANI::Shield_Atk_N02 == m_eState)
	{
		if (m_pMesh->Is_Finish_Animation(0.45f))
		{
			m_tObjParam.bSuperArmor = false;
			m_eState = Shield_Atk_N03;
			return;
		}
		else if (1.633f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		if (1.300f < AniTime && 1.567f > AniTime)
		{
			if (m_bEventTrigger[5] == false)
			{
				m_bEventTrigger[5] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (PET_DEERKING_ANI::Shield_Atk_N03 == m_eState)
	{
		if (m_pMesh->Is_Finish_Animation(0.9f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.5f;

			return;
		}
		else if (2.500f <= AniTime)
		{
			if (m_bEventTrigger[6] == false)
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (2.267f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}
		else if (2.100f <= AniTime)
		{
			if (m_bEventTrigger[8] == false)
			{
				m_bEventTrigger[8] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (1.915f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}
		else if (1.713f <= AniTime)
		{
			if (m_bEventTrigger[10] == false)
			{
				m_bEventTrigger[10] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (1.500f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		if (2.333f < AniTime && 2.833f > AniTime)
		{
			if (m_bEventTrigger[12] == false)
			{
				m_bEventTrigger[12] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.733f < AniTime && 2.200f > AniTime)
		{
			if (m_bEventTrigger[13] == false)
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.000f < AniTime && 1.633f > AniTime)
		{
			if (m_bEventTrigger[14] == false)
			{
				m_bEventTrigger[14] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CPet_DeerKing::Play_Chop()
{
	_double AniTime = m_pMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMesh->Is_Finish_Animation(0.92f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.0f;

			return;
		}
		else if (2.000f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (1.733f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = false;
			}
		}

		if (1.067f < AniTime && 1.767f> AniTime)
		{
			if (m_bEventTrigger[2] == false)
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CPet_DeerKing::Play_SnowQuake()
{
	_double AniTime = m_pMesh->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMesh->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.2f;

			return;
		}
		else if (1.633f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (1.333f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = false;
			}
		}

		if (0.300f < AniTime && 0.867f > AniTime)
		{
			if (m_bEventTrigger[2] == false)
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			if (nullptr != m_pTarget)
			{
				if (false == m_pTarget->Get_Enable() ||
					true == m_pTarget->Get_Dead())
				{
					Safe_Release(m_pTarget);
					Function_ResetAfterAtk();
					m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
					m_tObjParam.bCanAttack = true;
					m_bCanCoolDown = false;
					m_bIsCoolDown = false;
					m_fCoolDownCur = 0.f;
					m_fCoolDownMax = 0.f;

					Play_Idle();

					return;
				}
				else
					Function_RotateBody(m_pTarget);
			}
			else
			{
				Function_ResetAfterAtk();
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
				m_tObjParam.bCanAttack = true;
				m_bCanCoolDown = false;
				m_bIsCoolDown = false;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Play_Idle();

				return;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CPet_DeerKing::Play_Deformation()
{
	if (false == m_bCanSummon)
	{
		m_bCanSummon = true;
		m_bIsSummon = true;
		m_pMesh->Reset_OldIndx();
		m_eState = PET_DEERKING_ANI::Shield_Appearance_End;

		return;
	}
	else
	{
		if (m_pMesh->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bIsSummon = false;
			m_eState = PET_DEERKING_ANI::Idle;
			return;
		}
	}

	return;
}

void CPet_DeerKing::Play_Idle()
{
	m_eState = PET_DEERKING_ANI::Idle;

	switch (m_eTarget)
	{
	case PET_TARGET_TYPE::PET_TARGET_BOSS:
	case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		Function_RotateBody(m_pTarget);
		break;

	case PET_TARGET_TYPE::PET_TARGET_ITEM:
		Function_RotateBody(m_pTarget);
		break;

	case PET_TARGET_TYPE::PET_TARGET_NONE:
		Function_RotateBody(m_pPlayer);
		break;
	}

	return;
}

void CPet_DeerKing::Play_Move()
{
	switch (m_eSecondCategory_MOVE)
	{
	case PET_MOVE_TYPE::MOVE_WALK:
		m_fSkillMoveSpeed_Cur = 2.f;
		switch (m_eTarget)
		{
		case PET_TARGET_TYPE::PET_TARGET_BOSS:
		case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		case PET_TARGET_TYPE::PET_TARGET_ITEM:
			m_eState = PET_DEERKING_ANI::Walk_F;

			Function_RotateBody(m_pTarget);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			break;
		case PET_TARGET_TYPE::PET_TARGET_NONE:
			m_eState = PET_DEERKING_ANI::Walk_F;

			Function_RotateBody(m_pPlayer);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			break;
		}
		break;

	case PET_MOVE_TYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = PET_DEERKING_ANI::Run;
			m_fSkillMoveSpeed_Cur = 8.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;
		}
		switch (m_eTarget)
		{
		case PET_TARGET_TYPE::PET_TARGET_BOSS:
		case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		case PET_TARGET_TYPE::PET_TARGET_ITEM:
			Function_RotateBody(m_pTarget);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			break;
		case PET_TARGET_TYPE::PET_TARGET_NONE:
			Function_RotateBody(m_pPlayer);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransform->Get_Axis(AXIS_Z));
			break;
		}
		break;
	}

	return;
}

void CPet_DeerKing::Play_Hit()
{
	return;
}

void CPet_DeerKing::Play_CC()
{
	return;
}

void CPet_DeerKing::Play_Dead()
{
	return;
}

HRESULT CPet_DeerKing::Add_Component(void * pArg)
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DeerKing", L"Com_Mesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimization)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	m_pTransform->Set_Scale(_v3{ 0.25f, 0.25f, 0.25f });

	m_pCollider->Set_Radius(_v3{ 0.8f, 0.8f, 0.8f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CPet_DeerKing::SetUp_ConstantTable(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);

	// 기본 메트릭스
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	// 디졸브용 상수
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	// 쉐이더 재질정보 수치 입력
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

	// 림라이트 값들을 쉐이더에 등록시킴
	m_pBattleAgent->Update_RimParam_OnShader(pShader);

	return S_OK;
}

HRESULT CPet_DeerKing::Ready_Status(void * pArg)
{
	m_tObjParam.fDamage = 300.f; //힐량,공격량 등등
	m_tObjParam.fHp_Max = 2000.f;
	m_tObjParam.fArmor_Max = 10.f;

	m_fLimitRange = 30.f; //최대거리범위
	m_fActiveRange = 20.f; //자유행동범위

	m_fRecognitionRange = 12.f; //목표 인지범위
	m_fShotRange = 8.f; //목표 원거리범위
	m_fAtkRange = 4.f; //목표 근거리범위
	m_fPersonalRange = 2.f; //사회적 거리두기 범위

	m_eType = PET_TYPE::PET_DEERKING;
	m_eFirstCategory = PET_STATE_TYPE::IDLE;
	m_eNowPetMode = PET_MODE_TYPE::PET_MODE_ATK;
	m_eOldPetMdoe = PET_MODE_TYPE::PET_MODE_END;
	m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
	m_eFBLR = FBLR::FRONT;

	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fArmor_Cur = m_tObjParam.fArmor_Max;

	m_tObjParam.bCanHit = true; //피격 가능
	m_tObjParam.bIsHit = false;	//피격 진행중 아님
	m_tObjParam.bCanAttack = true; //공격 가능
	m_tObjParam.bIsAttack = false; //공격 진행중 아님
	m_tObjParam.bCanDodge = true; //회피 가능
	m_tObjParam.bIsDodge = false;  //회피 진행중 아님

	m_bCanPlayDead = false; //죽음 애니 진행시 true;
	m_bInRecognitionRange = false; //인지 범위 여부
	m_bInShotRange = false; //원거리 공격범위 여부
	m_bInAtkRange = false; //공격 범위 여부
	m_bCanChase = false; //추격 여부
	m_bCanCoolDown = false; //쿨타임 여부
	m_bIsCoolDown = false; //쿨타임 진행중 여부
	m_bCanChooseAtkType = true; //공격타입 고정용
	m_bIsCombo = false; //콤보 진행중
	m_bCanMoveAround = true; //경계 여부
	m_bIsMoveAround = false; //경게 진행중
	m_bCanIdle = true; //일상 가능
	m_bIsIdle = false; //일상 진행중 아님

	m_bEnable = false;

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

HRESULT CPet_DeerKing::Ready_Weapon(void * pArg)
{
	return S_OK;
}

HRESULT CPet_DeerKing::Ready_Collider(void * pArg)
{
	m_vecAttackCol.reserve(2);

	CCollider* pCollider = nullptr;
	_float fRadius = 0.f;

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Enabled(true);
	pCollider->Set_Type(COL_SPHERE);

	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightHand]->_41, m_matBone[Bone_RightHand]->_42, m_matBone[Bone_RightHand]->_43));
	m_vecAttackCol.push_back(pCollider);

	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftHand]->_41, m_matBone[Bone_LeftHand]->_42, m_matBone[Bone_LeftHand]->_43));
	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CPet_DeerKing::Ready_BoneMatrix(void * pArg)
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMesh->Get_BonInfo("RightHandAttach", 0), E_FAIL);
	m_matBone[Bone_RightHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMesh->Get_BonInfo("LeftHandAttach", 0), E_FAIL);
	m_matBone[Bone_LeftHand] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CPet_DeerKing * CPet_DeerKing::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPet_DeerKing* pInstance = new CPet_DeerKing(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_DeerKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_DeerKing::Clone_GameObject(void * pArg)
{
	CPet_DeerKing* pInstance = new CPet_DeerKing(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_DeerKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_DeerKing::Free()
{
	CPet::Free();

	return;
}
