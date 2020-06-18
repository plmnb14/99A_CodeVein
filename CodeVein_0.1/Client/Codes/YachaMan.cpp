#include "stdafx.h"
#include "..\Headers\YachaMan.h"

CYachaMan::CYachaMan(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster(pGraphic_Device)
{
}

CYachaMan::CYachaMan(const CYachaMan & rhs)
	: CMonster(rhs)
{
}

HRESULT CYachaMan::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CYachaMan::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_Status(pArg);
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Weapon();
	Ready_Rigid();

	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBone[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	return S_OK;
}

_int CYachaMan::Update_GameObject(_double TimeDelta)
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
	Check_FootSound();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	MONSTER_STATE_TYPE::DEAD != m_eFirstCategory ? Check_CollisionEvent() : Check_DeadEffect(TimeDelta);

	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================


	return NO_EVENT;
}

_int CYachaMan::Late_Update_GameObject(_double TimeDelta)
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

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Late_Update_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CYachaMan::Render_GameObject()
{
	/*IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

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

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Update_GameObject(m_dTimeDelta);

	if (MONSTER_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}
*/
	return S_OK;
}

HRESULT CYachaMan::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

		for (_uint i = 0; i < iNumMeshContainer; ++i)
		{
			_uint iNumSubSet = m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = !m_bDissolve ? m_pMeshCom->Get_MaterialPass(i, j) : 3;

				pShader->Begin_Pass(m_iPass);

				pShader->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				pShader->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				pShader->End_Pass();
			}
		}

	}

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Update_GameObject(m_dTimeDelta);

	if (MONSTER_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}

	return S_OK;
}

HRESULT CYachaMan::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
		_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_float fBloomPower = 0.5f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 실행
	//============================================================================================
	_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		//m_pMeshCom->Update_SkinnedMesh(i);

		_uint iNumSubSet = m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}
	//============================================================================================

	return S_OK;
}

void CYachaMan::Update_Collider()
{
	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[Bone_LeftArm] * m_pTransformCom->Get_WorldMat();

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

void CYachaMan::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CYachaMan::Check_PosY()
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

void CYachaMan::Check_Hit()
{
	if (MONSTER_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (false == m_tObjParam.bSuperArmor)
			{
				++m_iDodgeCount;

				if (m_iDodgeCount >= m_iDodgeCountMax)
				{
					m_bEventTrigger[0] = false;
					m_iDodgeCount = 0;
					m_tObjParam.bCanDodge = true;
					m_eFirstCategory = MONSTER_STATE_TYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVE_TYPE::MOVE_DODGE;
					m_pMeshCom->Reset_OldIndx();
				}
				else
				{
					m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
					m_pBattleAgentCom->Set_RimAlpha(0.5f);
					m_pBattleAgentCom->Set_RimValue(8.f);

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

void CYachaMan::Check_Dist()
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
				m_eState = YACHAMAN_ANI::Hammer_Idle;
				break;
			case MONSTER_IDLE_TYPE::IDLE_CROUCH:
			case MONSTER_IDLE_TYPE::IDLE_EAT:
				m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
				m_eState = YACHAMAN_ANI::NF_Eat;
				break;
			case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
			case MONSTER_IDLE_TYPE::IDLE_LURK:
				m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
				m_eState = YACHAMAN_ANI::NF_Lurk;
				break;
			case MONSTER_IDLE_TYPE::IDLE_STAND:
			case MONSTER_IDLE_TYPE::IDLE_SIT:
				m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
				m_eState = YACHAMAN_ANI::NF_Sit;
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
					{
						m_bCanChooseAtkType = true;
						m_eFirstCategory = MONSTER_STATE_TYPE::ATTACK;
					}
					else
					{
						m_bCanMoveAround = true;
						m_eFirstCategory = MONSTER_STATE_TYPE::MOVE;
						m_eSecondCategory_MOVE = MONSTER_MOVE_TYPE::MOVE_ALERT;
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
					m_eState = YACHAMAN_ANI::Hammer_Idle;
					break;
				case MONSTER_IDLE_TYPE::IDLE_CROUCH:
				case MONSTER_IDLE_TYPE::IDLE_EAT:
					m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
					m_eState = YACHAMAN_ANI::NF_Eat;
					break;
				case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
				case MONSTER_IDLE_TYPE::IDLE_LURK:
					m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
					m_eState = YACHAMAN_ANI::NF_Lurk;
					break;
				case MONSTER_IDLE_TYPE::IDLE_STAND:
				case MONSTER_IDLE_TYPE::IDLE_SIT:
					m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
					m_eState = YACHAMAN_ANI::NF_Sit;
					break;
				}
			}
		}
		return;
	}

	return;
}

void CYachaMan::Check_AniEvent()
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
		if (true == m_bCanChooseAtkType)
		{
			if (m_iPatternCount >= m_iPatternCountMax)
			{
				m_bCanSequencePattern = false;
				m_iPatternCount = 0;
			}

			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;

			m_bCanChooseAtkType = false;

			if (true == m_bCanSequencePattern)
			{
				m_eSecondCategory_ATK = MONSTER_ATK_TYPE::ATK_SEQUNCE;
				Play_SequenceAtk();
			}
			else
			{
				switch (CALC::Random_Num(MONSTER_ATK_TYPE::ATK_NORMAL, MONSTER_ATK_TYPE::ATK_COMBO))
				{
				case MONSTER_ATK_TYPE::ATK_NORMAL:
					m_eSecondCategory_ATK = MONSTER_ATK_TYPE::ATK_NORMAL;
					Play_RandomAtkNormal();
					break;
				case MONSTER_ATK_TYPE::ATK_COMBO:
					m_eSecondCategory_ATK = MONSTER_ATK_TYPE::ATK_COMBO;
					Play_RandomAtkCombo();
					m_bIsCombo = true;
					break;
				}
			}

			return;
		}
		else
		{
			if (MONSTER_ATK_TYPE::ATK_NORMAL == m_eSecondCategory_ATK)
			{
				switch (m_eState)
				{
				case YACHAMAN_ANI::Hammer_N01:
					Play_R();
					break;
				case YACHAMAN_ANI::Hammer_S01:
					Play_Shoulder();
					break;
				case YACHAMAN_ANI::Hammer_Sp01:
					Play_TargetHammering();
					break;
				case YACHAMAN_ANI::Hammer_Sp02:
					Play_WheelWind();
					break;
				}
			}
			else if (MONSTER_ATK_TYPE::ATK_COMBO == m_eSecondCategory_ATK)
			{
				switch (m_eAtkCombo)
				{
				case ATK_COMBO_TYPE::COMBO_R_L:
					Play_Combo_R_L();
					break;
				case ATK_COMBO_TYPE::COMBO_R_HAMMERING:
					Play_Combo_R_Hammering();
					break;
				case ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE:
					Play_Combo_Shoulder_TurnTwice();
					break;
				case ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK:
					Play_Combo_Shoulder_HalfClock();
					break;
				case ATK_COMBO_TYPE::COMBO_RUNHAMMERING:
					Play_Combo_RunHammering();
					break;
				}
			}
			else if (MONSTER_ATK_TYPE::ATK_SEQUNCE == m_eSecondCategory_ATK)
			{
				if (true == m_bIsCombo)
				{
					switch (m_eAtkCombo)
					{
					case ATK_COMBO_TYPE::COMBO_R_L:
						Play_Combo_R_L();
						break;
					case ATK_COMBO_TYPE::COMBO_R_HAMMERING:
						Play_Combo_R_Hammering();
						break;
					case ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE:
						Play_Combo_Shoulder_TurnTwice();
						break;
					case ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK:
						Play_Combo_Shoulder_HalfClock();
						break;
					case ATK_COMBO_TYPE::COMBO_RUNHAMMERING:
						Play_Combo_RunHammering();
						break;
					}
				}
				else
				{
					switch (m_eState)
					{
					case YACHAMAN_ANI::Hammer_N01:
						Play_R();
						break;
					case YACHAMAN_ANI::Hammer_S01:
						Play_Shoulder();
						break;
					case YACHAMAN_ANI::Hammer_Sp01:
						Play_TargetHammering();
						break;
					case YACHAMAN_ANI::Hammer_Sp02:
						Play_WheelWind();
						break;
					}
				}
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

void CYachaMan::Check_DeadEffect(_double TimeDelta)
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

void CYachaMan::Check_FootSound()
{
	if (YACHAMAN_ANI::Walk_F == m_eState ||
		YACHAMAN_ANI::Walk_L == m_eState ||
		YACHAMAN_ANI::Walk_R == m_eState ||
		YACHAMAN_ANI::Run == m_eState)
	{
		m_fFootSound += DELTA_60;

		if (m_fFootSound >= m_fFootSoundMax)
		{
			m_fFootSound = 0.f;

			g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Step);

			switch (CALC::Random_Num(0, 5))
			{
			case 0:
				g_pSoundManager->Play_Sound(L"Step0.ogg", CSoundManager::Yacha_Step, CSoundManager::Effect_Sound);
				break;

			case 1:
				g_pSoundManager->Play_Sound(L"Step1.ogg", CSoundManager::Yacha_Step, CSoundManager::Effect_Sound);
				break;

			case 2:
				g_pSoundManager->Play_Sound(L"Step2.ogg", CSoundManager::Yacha_Step, CSoundManager::Effect_Sound);
				break;

			case 3:
				g_pSoundManager->Play_Sound(L"Step3.ogg", CSoundManager::Yacha_Step, CSoundManager::Effect_Sound);
				break;

			case 4:
				g_pSoundManager->Play_Sound(L"Step4.ogg", CSoundManager::Yacha_Step, CSoundManager::Effect_Sound);
				break;

			case 5:
				g_pSoundManager->Play_Sound(L"Step5.ogg", CSoundManager::Yacha_Step, CSoundManager::Effect_Sound);
				break;
			}
		}
	}

	return;
}

void CYachaMan::Play_SequenceAtk()
{
	switch (m_iPatternCount)
	{
	case 0:
		m_eState = YACHAMAN_ANI::Hammer_N01;
		break;

	case 1:
		m_eState = YACHAMAN_ANI::Hammer_S01;
		break;

	case 2:
		m_eState = YACHAMAN_ANI::Hammer_Sp01;
		break;

	case 3:
		m_eState = YACHAMAN_ANI::Hammer_Sp02;
		break;

	case 4:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RUNHAMMERING;
		m_eState = Hammer_Sp03_Start;
		break;

	case 5:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_L;
		m_eState = Hammer_N01;
		break;

	case 6:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_HAMMERING;
		m_eState = Hammer_N01;
		break;

	case 7:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE;
		m_eState = Hammer_S01;
		break;

	case 8:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK;
		m_eState = Hammer_S01;
		break;

	}

	++m_iPatternCount;

	return;
}

void CYachaMan::Play_RandomAtkNormal()
{
	switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RIGHT, ATK_NORMAL_TYPE::NORMAL_WHEELWIND))
	{
	case ATK_NORMAL_TYPE::NORMAL_RIGHT:
		m_eState = YACHAMAN_ANI::Hammer_N01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_SHOULDER:
		m_eState = YACHAMAN_ANI::Hammer_S01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_TARGETHAMMERING:
		m_eState = YACHAMAN_ANI::Hammer_Sp01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_WHEELWIND:
		m_eState = YACHAMAN_ANI::Hammer_Sp02;
		break;
	}

	return;
}

void CYachaMan::Play_RandomAtkCombo()
{
	switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_R_L, ATK_COMBO_TYPE::COMBO_RUNHAMMERING))
	{
	case ATK_COMBO_TYPE::COMBO_R_L:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_L;
		m_eState = Hammer_N01;
		break;
	case ATK_COMBO_TYPE::COMBO_R_HAMMERING:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_HAMMERING;
		m_eState = Hammer_N01;
		break;
	case ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE;
		m_eState = Hammer_S01;
		break;
	case ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK;
		m_eState = Hammer_S01;
		break;
	case ATK_COMBO_TYPE::COMBO_RUNHAMMERING:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RUNHAMMERING;
		m_eState = Hammer_Sp03_Start;
		break;
	}

	return;
}

void CYachaMan::Play_R()
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
		else if (4.233f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.933f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.4f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		
		if (2.833f<AniTime && 4.233f> AniTime)
		{
			if (m_bEventTrigger[3] == false)
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_Shoulder()
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
			m_fCoolDownMax = 0.3f;

			return;
		}
		else if (2.500f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.067f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.8f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (4.733f < AniTime && 5.800f >AniTime)
		{
			if (m_bEventTrigger[2] == false)
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 0.2f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.500f < AniTime && 1.800f > AniTime)
		{
			if (m_bEventTrigger[3] == false)
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYachaMan::Play_TargetHammering()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.0f;

			return;
		}
		else if (5.400f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (5.100f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);

				m_tObjParam.bSuperArmor = false;
				g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon->Get_HeadPos());
			}
		}
		else if (4.900f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.8f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (5.267f < AniTime && 6.033f > AniTime)
		{
			if (m_bEventTrigger[3] == false)
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (4.433f < AniTime && 4.933f >AniTime)
		{
			if (m_bEventTrigger[4] == false)
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_WheelWind()
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
			m_fCoolDownMax = 1.5f;

			return;
		}
		else if (6.867f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (6.567f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (6.433f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (6.f <= AniTime)
		{
			if (false == m_bEventTrigger[18])
			{
				m_bEventTrigger[18] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (6.333f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (5.733f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (5.600f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (5.3f <= AniTime)
		{
			if (false == m_bEventTrigger[19])
			{
				m_bEventTrigger[19] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (5.100f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.800f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.633f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.5f <= AniTime)
		{
			if (false == m_bEventTrigger[20])
			{
				m_bEventTrigger[20] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.900f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.600f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[21])
			{
				m_bEventTrigger[21] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (6.267f < AniTime && 6.633f > AniTime)
		{
			if (m_bEventTrigger[12] == false)
			{
				m_bEventTrigger[12] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (5.800f < AniTime && 6.200f > AniTime)
		{
			if (m_bEventTrigger[13] == false)
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (5.400f < AniTime && 5.767f > AniTime)
		{
			if (m_bEventTrigger[14] == false)
			{
				m_bEventTrigger[14] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (4.800f < AniTime && 5.367f > AniTime)
		{
			if (m_bEventTrigger[15] == false)
			{
				m_bEventTrigger[15] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (3.700f < AniTime && 4.367f > AniTime)
		{
			if (m_bEventTrigger[16] == false)
			{
				m_bEventTrigger[16] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (2.867f < AniTime && 3.600f > AniTime)
		{
			if (m_bEventTrigger[17] == false)
			{
				m_bEventTrigger[17] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_Combo_R_L()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YACHAMAN_ANI::Hammer_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.45f))
		{
			m_tObjParam.bSuperArmor = false;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = YACHAMAN_ANI::Hammer_N02;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (4.233f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.933f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_dAniPlayMul = 1;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (2.833f < AniTime && 4.233f > AniTime)
		{
			if (m_bEventTrigger[4] == false)
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}
	else if (YACHAMAN_ANI::Hammer_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.6f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (3.000f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.700f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.400f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_dAniPlayMul = 1;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (0.900f < AniTime && 2.833f > AniTime)
		{
			if (m_bEventTrigger[9] == false)
			{
				m_bEventTrigger[9] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_Combo_R_Hammering()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YACHAMAN_ANI::Hammer_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.45f))
		{
			m_tObjParam.bSuperArmor = false;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = YACHAMAN_ANI::Hammer_N03;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (4.233f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.933f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_dAniPlayMul = 1;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (2.833f < AniTime && 4.233f > AniTime)
		{
			if (m_bEventTrigger[4] == false)
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}
	else if (YACHAMAN_ANI::Hammer_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.2f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (3.933f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);

				m_tObjParam.bSuperArmor = false;
				g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon->Get_HeadPos());
			}
		}
		else if (3.267f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_dAniPlayMul = 1;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (2.167f < AniTime && 3.367f > AniTime)
		{
			if (m_bEventTrigger[9] == false)
			{
				m_bEventTrigger[9] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_Combo_Shoulder_TurnTwice()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YACHAMAN_ANI::Hammer_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.28f))
		{
			m_tObjParam.bSuperArmor = false;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Hammer_S02;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (2.500f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.067f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
				m_dAniPlayMul = 1;
			}
		}
		else if (1.7f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (4.733f < AniTime && 5.800f >AniTime)
		{
			if (m_bEventTrigger[3] == false)
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 0.2f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.500f < AniTime && 1.800f > AniTime)
		{
			if (m_bEventTrigger[4] == false)
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}
	else if (YACHAMAN_ANI::Hammer_S02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.4f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (4.000f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.300f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.600f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.300f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_dAniPlayMul = 1;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.5f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (3.233f < AniTime && 3.733f > AniTime)
		{
			if (m_bEventTrigger[13] == false)
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (2.200f < AniTime && 3.133f >AniTime)
		{
			if (m_bEventTrigger[14] == false)
			{
				m_bEventTrigger[14] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (1.767f < AniTime && 2.100f > AniTime)
		{
			if (m_bEventTrigger[15] == false)
			{
				m_bEventTrigger[15] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_Combo_Shoulder_HalfClock()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YACHAMAN_ANI::Hammer_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.28f))
		{
			m_vecAttackCol[0]->Set_Enabled(false);
			m_tObjParam.bSuperArmor = false;
			m_eState = Hammer_S03;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (2.500f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.067f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
				m_dAniPlayMul = 1;
			}
		}
		else if (1.765f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (4.733f < AniTime && 5.800f >AniTime)
		{
			if (m_bEventTrigger[3] == false)
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 0.2f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.500f < AniTime && 1.800f > AniTime)
		{
			if (m_bEventTrigger[4] == false)
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}
	else if (YACHAMAN_ANI::Hammer_S03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.7f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (2.433f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.133f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.800f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_dAniPlayMul = 1;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.5f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (1.933f < AniTime && 3.000f> AniTime)
		{
			if (m_bEventTrigger[9] == false)
			{
				m_bEventTrigger[9] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
		else if (1.000f < AniTime && 1.833f >AniTime)
		{
			if (m_bEventTrigger[10] == false)
			{
				m_bEventTrigger[10] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}

	return;
}

void CYachaMan::Play_Combo_RunHammering()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YACHAMAN_ANI::Hammer_Sp03_Start == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_tObjParam.bSuperArmor = false;
			m_eState = Hammer_Sp03_Loop;

			m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(1.f);
			m_pBattleAgentCom->Set_RimValue(5.f);

			return;
		}

		if (3.667f < AniTime)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;
				m_fSkillMoveSpeed_Cur = 12.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 2.f;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
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
						case MONSTER_IDLE_TYPE::IDLE_CROUCH:
						case MONSTER_IDLE_TYPE::IDLE_EAT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		}
	}
	else if (YACHAMAN_ANI::Hammer_Sp03_Loop == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_tObjParam.bSuperArmor = false;
			m_eState = Hammer_Sp03_End;

			return;
		}
		if (m_bEventTrigger[1] == false)
		{
			m_bEventTrigger[1] = true;
			m_tObjParam.bSuperArmor = true;
			m_fSkillMoveSpeed_Cur = 12.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 2.f;

			g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

			m_iRandom = CALC::Random_Num(0, 6);

			switch (m_iRandom)
			{
			case 0:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			case 1:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			case 2:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			case 3:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			case 4:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			case 5:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			case 6:
				g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
				break;
			}

			g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
			g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
			g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

			m_iRandom = CALC::Random_Num(0, 3);

			switch (m_iRandom)
			{
			case 0:
				g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
				break;
			case 1:
				g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
				break;
			case 2:
				g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
				break;
			case 3:
				g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
				break;
			}
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
					case MONSTER_IDLE_TYPE::IDLE_CROUCH:
					case MONSTER_IDLE_TYPE::IDLE_EAT:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
						break;
					case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
					case MONSTER_IDLE_TYPE::IDLE_LURK:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
						break;
					case MONSTER_IDLE_TYPE::IDLE_STAND:
					case MONSTER_IDLE_TYPE::IDLE_SIT:
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
	}
	else if (YACHAMAN_ANI::Hammer_Sp03_End == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.4f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (1.767f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.467f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;

				g_pManagement->Create_Effect(L"Weapon_HeavyDust", m_pWeapon->Get_HeadPos());
			}
		}
		else if (0.733f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_SFX_02);
				g_pSoundManager->Play_Sound(L"Monster_HeavyWeapon_Sub.ogg", CSoundManager::Yacha_SFX_02, CSoundManager::Effect_Sound);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing0.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing1.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing2.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Blunt_Swing3.ogg", CSoundManager::Yacha_SFX_01, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (0.7f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice4.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice5.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"YachaMan_Atk_Voice6.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (0.433f > AniTime)
		{
			if (m_bEventTrigger[5] == false)
			{
				m_bEventTrigger[5] = true;
				m_fSkillMoveSpeed_Cur = 12.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 4.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYachaMan::Play_Idle()
{
	switch (m_eSecondCategory_IDLE)
	{
	case MONSTER_IDLE_TYPE::IDLE_IDLE:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				m_bIsIdle = false;

				m_eState = YACHAMAN_ANI::Hammer_Idle;
			}
			else
			{
				m_bIsIdle = true;
				m_eState = YACHAMAN_ANI::Hammer_Idle;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = YACHAMAN_ANI::Hammer_Idle;
		}
		break;
	case MONSTER_IDLE_TYPE::IDLE_EAT:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				if (YACHAMAN_ANI::NF_Eat == m_eState)
				{
					m_bIsIdle = true;

					if (m_pMeshCom->Is_Finish_Animation(0.25f))
						m_eState = YACHAMAN_ANI::NF_Eat_End;
				}
				else if (YACHAMAN_ANI::NF_Eat_End == m_eState)
				{
					if (m_pMeshCom->Is_Finish_Animation(0.95f))
					{
						m_bCanIdle = true;
						m_bIsIdle = false;
						m_eState = YACHAMAN_ANI::Hammer_Idle;
					}
				}
			}
			else
			{
				m_bIsIdle = true;
				m_eState = YACHAMAN_ANI::NF_Eat;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = YACHAMAN_ANI::NF_Eat;
		}
		break;
	case MONSTER_IDLE_TYPE::IDLE_LURK:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				if (YACHAMAN_ANI::NF_Lurk == m_eState)
				{
					m_bIsIdle = true;

					if (m_pMeshCom->Is_Finish_Animation(0.5f))
						m_eState = YACHAMAN_ANI::NF_Lurk_End;
				}
				else if (YACHAMAN_ANI::NF_Lurk_End == m_eState)
				{
					if (m_pMeshCom->Is_Finish_Animation(0.95f))
					{
						m_bCanIdle = true;
						m_bIsIdle = false;
						m_eState = YACHAMAN_ANI::Hammer_Idle;
					}
				}
			}
			else
			{
				m_bIsIdle = true;
				m_eState = YACHAMAN_ANI::NF_Lurk;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = YACHAMAN_ANI::NF_Lurk;
		}
		break;
	case MONSTER_IDLE_TYPE::IDLE_SIT:
		if (nullptr != m_pAggroTarget)
		{
			if (true == m_bInRecognitionRange)
			{
				if (YACHAMAN_ANI::NF_Sit == m_eState)
				{
					m_bIsIdle = true;

					if (m_pMeshCom->Is_Finish_Animation(0.5f))
						m_eState = YACHAMAN_ANI::NF_Sit_End;
				}
				else if (YACHAMAN_ANI::NF_Sit_End == m_eState)
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					m_eState = YACHAMAN_ANI::Hammer_Idle;
				}
			}
			else
			{
				m_bIsIdle = true;
				m_eState = YACHAMAN_ANI::NF_Sit;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = YACHAMAN_ANI::NF_Sit;
		}
		break;
	}

	return;
}

void CYachaMan::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVE_TYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = YACHAMAN_ANI::Run;
			m_fSkillMoveSpeed_Cur = 4.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;

			m_fFootSoundMax = 0.4f;
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
					case MONSTER_IDLE_TYPE::IDLE_CROUCH:
					case MONSTER_IDLE_TYPE::IDLE_EAT:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
						break;
					case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
					case MONSTER_IDLE_TYPE::IDLE_LURK:
						m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
						break;
					case MONSTER_IDLE_TYPE::IDLE_STAND:
					case MONSTER_IDLE_TYPE::IDLE_SIT:
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
		break;

	case MONSTER_MOVE_TYPE::MOVE_ALERT:
		if (true == m_bCanMoveAround)
		{
			m_bCanMoveAround = false;
			m_bIsMoveAround = true;

			m_bCanCoolDown = true;
			m_fCoolDownMax = CALC::Random_Num(1, 3) * 1.0f;

			m_fSkillMoveSpeed_Cur = 2.5f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;

			m_fFootSoundMax = 0.47f;

			switch (CALC::Random_Num(YACHAMAN_ANI::Walk_R, YACHAMAN_ANI::Walk_B))
			{
			case YACHAMAN_ANI::Walk_R:
			case YACHAMAN_ANI::Walk_F:
				m_eState = YACHAMAN_ANI::Walk_R;
				break;
			case YACHAMAN_ANI::Walk_L:
			case YACHAMAN_ANI::Walk_B:
				m_eState = YACHAMAN_ANI::Walk_L;
				break;
			}
		}
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
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
							break;
						case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
						case MONSTER_IDLE_TYPE::IDLE_LURK:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
							break;
						case MONSTER_IDLE_TYPE::IDLE_STAND:
						case MONSTER_IDLE_TYPE::IDLE_SIT:
							m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
							break;
						}
					}

					Play_Idle();

					return;
				}
				else
				{
					if (YACHAMAN_ANI::Walk_R == m_eState)
						Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
					else if (YACHAMAN_ANI::Walk_L == m_eState)
						Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
				}
			}
			else
			{
				if (YACHAMAN_ANI::Walk_R == m_eState)
					Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
				else if (YACHAMAN_ANI::Walk_L == m_eState)
					Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
			}
		}
		break;

	case MONSTER_MOVE_TYPE::MOVE_DODGE:
		if (true == m_tObjParam.bCanDodge)
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanDodge = false;
			m_tObjParam.bIsDodge = true;
			m_eState = YACHAMAN_ANI::Dodge;
		}
		else
		{
			if (m_pMeshCom->Is_Finish_Animation(0.98f))
			{
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
				m_tObjParam.bCanAttack = true;
				Function_ResetAfterAtk();

				return;
			}
			else if (0.600f < AniTime && 1.500f> AniTime)
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

void CYachaMan::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;

		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveSpeed_Cur = 5.f;
		m_fSkillMoveMultiply = 0.5f;

		switch (m_eFBLR)
		{
		case Client::CMonster::FRONTLEFT:
			m_eState = YACHAMAN_ANI::Dmg_W_FL;
			break;
		case Client::CMonster::FRONT:
		case Client::CMonster::FRONTRIGHT:
			m_eState = YACHAMAN_ANI::Dmg_W_FR;
			break;
		case Client::CMonster::BACK:
		case Client::CMonster::BACKLEFT:
			m_eState = YACHAMAN_ANI::Dmg_W_BL;
			break;
		case Client::CMonster::BACKRIGHT:
			m_eState = YACHAMAN_ANI::Dmg_W_BR;
			break;
		}
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
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

				m_iRandom = CALC::Random_Num(0, 3);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"YachaMan_Hit0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"YachaMan_Hit1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"YachaMan_Hit2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"YachaMan_Hit3.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}

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

void CYachaMan::Play_CC()
{
	return;
}

void CYachaMan::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();
		m_bCanPlayDead = true;

		if (YACHAMAN_ANI::Down_S_Start == m_eState ||
			YACHAMAN_ANI::Down_S_End == m_eState)
			m_eState = YACHAMAN_ANI::Death_B;
		else if (YACHAMAN_ANI::Down_P_Start == m_eState ||
			YACHAMAN_ANI::Down_P_Loop == m_eState ||
			YACHAMAN_ANI::Down_P_End == m_eState)
			m_eState = YACHAMAN_ANI::Death_F;
		else
			m_eState = YACHAMAN_ANI::Death;
	}
	else
	{
		switch (m_eState)
		{
		case YACHAMAN_ANI::Death_F:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			else if (m_pMeshCom->Is_Finish_Animation(0.2f))
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

					m_iRandom = CALC::Random_Num(0, 2);

					switch (m_iRandom)
					{
					case 0:
						g_pSoundManager->Play_Sound(L"YachMan_Death0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 1:
						g_pSoundManager->Play_Sound(L"YachMan_Death1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 2:
						g_pSoundManager->Play_Sound(L"YachMan_Death2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					}
				}
			}
			else if (1.433f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					Start_Dissolve(0.7f, false, true, 0.0f);
					m_pWeapon->Start_Dissolve(0.7f, false, true, 0.f);
					m_fDeadEffect_Delay = 0.f;

					CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 0.f));
				}
			}
			else if (1.4f <= AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

					m_iRandom = CALC::Random_Num(0, 2);

					switch (m_iRandom)
					{
					case 0:
						g_pSoundManager->Play_Sound(L"YachMan_Death0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 1:
						g_pSoundManager->Play_Sound(L"YachMan_Death1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 2:
						g_pSoundManager->Play_Sound(L"YachMan_Death2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					}
				}
			}
			break;

		case YACHAMAN_ANI::Death_B:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			else if (m_pMeshCom->Is_Finish_Animation(0.2f))
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

					m_iRandom = CALC::Random_Num(0, 2);

					switch (m_iRandom)
					{
					case 0:
						g_pSoundManager->Play_Sound(L"YachMan_Death0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 1:
						g_pSoundManager->Play_Sound(L"YachMan_Death1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 2:
						g_pSoundManager->Play_Sound(L"YachMan_Death2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					}
				}
			}
			else if (1.867f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					Start_Dissolve(0.7f, false, true, 0.0f);
					m_pWeapon->Start_Dissolve(0.7f, false, true, 0.f);
					m_fDeadEffect_Delay = 0.f;

					CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 0.f));
				}
			}
			else if (1.8f <= AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

					m_iRandom = CALC::Random_Num(0, 2);

					switch (m_iRandom)
					{
					case 0:
						g_pSoundManager->Play_Sound(L"YachMan_Death0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 1:
						g_pSoundManager->Play_Sound(L"YachMan_Death1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 2:
						g_pSoundManager->Play_Sound(L"YachMan_Death2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					}
				}
			}
			break;

		case YACHAMAN_ANI::Death:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}

			if (3.233f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.7f, false, true);
					m_pWeapon->Start_Dissolve(0.5f, false, true);

					Give_Mana_To_Player(5);

					if (false == m_pRigidCom->Get_IsFall())
						Check_DropItem(MONSTER_NAMETYPE::M_YachaMan);

					CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 0.f));
				}
			}
			else if (2.9f <= AniTime)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;

					g_pSoundManager->Stop_Sound(CSoundManager::Yacha_Voice);

					m_iRandom = CALC::Random_Num(0, 2);

					switch (m_iRandom)
					{
					case 0:
						g_pSoundManager->Play_Sound(L"YachMan_Death0.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 1:
						g_pSoundManager->Play_Sound(L"YachMan_Death1.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					case 2:
						g_pSoundManager->Play_Sound(L"YachMan_Death2.ogg", CSoundManager::Yacha_Voice, CSoundManager::Effect_Sound);
						break;
					}
				}
			}

			if (1.30f < AniTime && 2.80f > AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 1.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			break;
		}
	}

	return;
}

HRESULT CYachaMan::Add_Component(void* pArg)
{
	_tchar MeshName[STR_128] = L"";

	MONSTER_STATUS eTemp = *(MONSTER_STATUS*)pArg;

	if (nullptr == pArg)
		lstrcpy(MeshName, L"Mesh_YachaMan_Black");
	else
	{
		switch (eTemp.eMonsterColor)
		{
		case MONSTER_COLOR_TYPE::RED:
			lstrcpy(MeshName, L"Mesh_YachaMan_Red");
			break;
		case MONSTER_COLOR_TYPE::BLUE:
		case MONSTER_COLOR_TYPE::YELLOW:
		case MONSTER_COLOR_TYPE::WHITE:
		case MONSTER_COLOR_TYPE::BLACK:
		case MONSTER_COLOR_TYPE::COLOR_NONE:
			lstrcpy(MeshName, L"Mesh_YachaMan_Black");
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

	m_pColliderCom->Set_Radius(V3_ONE);
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CYachaMan::SetUp_ConstantTable(CShader* pShader)
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

HRESULT CYachaMan::Ready_Status(void * pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Create CYachaMan pArgument nullptr Failed");
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
			Info.eStageIdx == 0 ? L"Navmesh_Training.dat" :
			Info.eStageIdx == 1 ? L"Navmesh_Stage_01.dat" :
			Info.eStageIdx == 2 ? L"Navmesh_Stage_02.dat" :
			Info.eStageIdx == 3 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

		m_pNavMeshCom->Set_Index(-1);
		m_pNavMeshCom->Ready_NaviMesh(m_pGraphic_Dev, szNavData);
		m_pNavMeshCom->Check_OnNavMesh(Info.vPos);
		m_pTransformCom->Set_Pos(Info.vPos);
		m_pTransformCom->Set_Angle(Info.vAngle);
	}

	if (MONSTER_COLOR_TYPE::RED == Info.eMonsterColor)
	{
		m_eMonsterColor = Info.eMonsterColor;
		m_tObjParam.fDamage = 350.f * pow(1.5f, g_eStageIdx_Cur - 1);
		m_tObjParam.fHp_Max = 2500.f * pow(1.5f, g_eStageIdx_Cur - 1);
		m_tObjParam.fArmor_Max = 30.f * pow(1.5f, g_eStageIdx_Cur - 1);

		m_fRecognitionRange = 15.f;
		m_fShotRange = 10.f;
		m_fAtkRange = 5.f;
		m_fPersonalRange = 2.f;
	}
	else
	{
		m_eMonsterColor = MONSTER_COLOR_TYPE::BLACK;
		m_tObjParam.fDamage = 350.f * pow(1.5f, g_eStageIdx_Cur - 1);
		m_tObjParam.fHp_Max = 2500.f * pow(1.5f, g_eStageIdx_Cur - 1);
		m_tObjParam.fArmor_Max = 30.f * pow(1.5f, g_eStageIdx_Cur - 1);

		m_fRecognitionRange = 15.f;
		m_fShotRange = 10.f;
		m_fAtkRange = 5.f;
		m_fPersonalRange = 2.f;
	}

	m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

	switch (CALC::Random_Num(MONSTER_IDLE_TYPE::IDLE_IDLE, MONSTER_IDLE_TYPE::IDLE_STAND))
	{
	case MONSTER_IDLE_TYPE::IDLE_IDLE:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_IDLE;
		m_eState = YACHAMAN_ANI::Hammer_Idle;
		break;

	case MONSTER_IDLE_TYPE::IDLE_CROUCH:
	case MONSTER_IDLE_TYPE::IDLE_EAT:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_EAT;
		m_eState = YACHAMAN_ANI::NF_Eat;
		break;

	case MONSTER_IDLE_TYPE::IDLE_STAND:
	case MONSTER_IDLE_TYPE::IDLE_LURK:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_LURK;
		m_eState = YACHAMAN_ANI::NF_Lurk;
		break;

	case MONSTER_IDLE_TYPE::IDLE_SCRATCH:
	case MONSTER_IDLE_TYPE::IDLE_SIT:
		m_eSecondCategory_IDLE = MONSTER_IDLE_TYPE::IDLE_SIT;
		m_eState = YACHAMAN_ANI::NF_Sit;
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

	m_bCanSequencePattern = true;
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

	m_iDodgeCountMax = CALC::Random_Num(3, 5);
	m_iPatternCount = 0;
	m_iPatternCountMax = 9;

	return S_OK;
}

HRESULT CYachaMan::Ready_Weapon()
{
	m_pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon->Change_WeaponData(Wpn_Hammer_Military);
	m_pWeapon->Set_WeaponDamage(WEAPON_DATA::Wpn_Hammer_Military, 350.f * pow(1.5f, g_eStageIdx_Cur - 1));

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pWeapon->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());


	return S_OK;
}

HRESULT CYachaMan::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(2);

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
	fRadius = 1.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftArm]->_41, m_matBone[Bone_LeftArm]->_42, m_matBone[Bone_LeftArm]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftArm]->_41, m_matBone[Bone_LeftArm]->_42, m_matBone[Bone_LeftArm]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CYachaMan::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftArm", 0), E_FAIL);
	m_matBone[Bone_LeftArm] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CYachaMan::Ready_Rigid()
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

CYachaMan* CYachaMan::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYachaMan* pInstance = new CYachaMan(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CYachaMan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYachaMan::Clone_GameObject(void * pArg)
{
	CYachaMan* pInstance = new CYachaMan(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CYachaMan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYachaMan::Free()
{
	CMonster::Free();

	return;
}
