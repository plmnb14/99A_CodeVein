#include "stdafx.h"
#include "..\Headers\Yeti.h"

CYeti::CYeti(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CMonster(pGraphic_Device)
{
}

CYeti::CYeti(const CYeti & rhs)
	: CMonster(rhs)
{
}

HRESULT CYeti::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CYeti::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
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

_int CYeti::Update_GameObject(_double TimeDelta)
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

_int CYeti::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CYeti::Render_GameObject()
{
	//IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	//IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	//m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	//if (m_bInFrustum)
	//{
	//	if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
	//		return E_FAIL;

	//	m_pShaderCom->Begin_Shader();

	//	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	//	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	//	{
	//		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

	//		m_pMeshCom->Update_SkinnedMesh(i);

	//		for (_uint j = 0; j < iNumSubSet; ++j)
	//		{
	//			m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

	//			if (m_bDissolve)
	//				m_iPass = 3;

	//			m_pShaderCom->Begin_Pass(m_iPass);

	//			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

	//			m_pShaderCom->Commit_Changes();

	//			m_pMeshCom->Render_Mesh(i, j);

	//			m_pShaderCom->End_Pass();
	//		}
	//	}

	//	m_pShaderCom->End_Shader();
	//}

	//if (MONSTER_STATE_TYPE::DEAD != m_eFirstCategory)
	//{
	//	Update_Collider();
	//	Render_Collider();
	//}

	return S_OK;
}

HRESULT CYeti::Render_GameObject_Instancing_SetPass(CShader * pShader)
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

	if (MONSTER_STATE_TYPE::DEAD != m_eFirstCategory)
	{
		Update_Collider();
		Render_Collider();
	}

	return S_OK;
}

HRESULT CYeti::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
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
	for (_uint i = 0; i < m_pMeshCom->Get_NumMeshContainer(); ++i)
	{
		//m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < m_pMeshCom->Get_NumMaterials(i); ++j)
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

void CYeti::Update_Collider()
{
	_ulong matrixIdx = Bone_Body;

	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[matrixIdx] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(matTemp._41, matTemp._42, matTemp._43);

		vector_iter->Update(ColPos);

		++matrixIdx;
	}

	matrixIdx = 0;

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

void CYeti::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
		
	return;
}

void CYeti::Check_PosY()
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

void CYeti::Check_Hit()
{
	if (MONSTER_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (false == m_tObjParam.bSuperArmor)
			{
				m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(0.5f);
				m_pBattleAgentCom->Set_RimValue(8.f);

				if (true == m_bIsMoveAround &&  3 == CALC::Random_Num(0,3))
					Function_ResetAfterAtk();

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
	else
		m_eFirstCategory = MONSTER_STATE_TYPE::DEAD;

	return;
}

void CYeti::Check_Dist()
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

			m_eState = YETI_ANI::Idle;

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
			m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
	}

	return;
}

void CYeti::Check_AniEvent()
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
					m_bIsCombo = true;
					Play_RandomAtkCombo();
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
				case YETI_ANI::Atk_Rush:
					Play_Rush();
					break;
				case YETI_ANI::Atk_RollingSlash:
					Play_RollingSlash();
					break;
				case YETI_ANI::Atk_IceThrowing:
					Play_IceThrowing();
					break;
				case YETI_ANI::Atk_Sp06:
					Play_RLRL();
					break;
				case YETI_ANI::Atk_Sp05:
					Play_WoodChop();
					break;
				case YETI_ANI::Atk_Sp04:
					Play_FastLR();
					break;
				case YETI_ANI::Atk_Sp03:
					Play_LRSweap();
					break;
				case YETI_ANI::Atk_Sp02:
					Play_RUpperChop();
					break;
				case YETI_ANI::Atk_Sp01:
					Play_SlowLR();
					break;
				case YETI_ANI::Atk_Field:
					//자가 버프 혹은 근처에 일렁거리며 데미지 주기
					Play_Howling();
					break;
				case YETI_ANI::Atk_BodyPress:
					Play_BodyPress();
					break;
				}
			}
			else if (MONSTER_ATK_TYPE::ATK_COMBO == m_eSecondCategory_ATK)
			{
				switch (m_eAtkCombo)
				{
				case ATK_COMBO_TYPE::COMBO_RLRL_SHOULDER:
					Play_Combo_RLRL_Shoulder();
					break;
				case ATK_COMBO_TYPE::COMBO_RLRL_SMASH:
					Play_Combo_RLRL_Smash();
					break;
				case ATK_COMBO_TYPE::COMBO_RLRL_SWING:
					Play_Combo_RLRL_Swing();
					break;
				}
			}
			else if (MONSTER_ATK_TYPE::ATK_SEQUNCE == m_eSecondCategory_ATK)
			{
				if (true == m_bIsCombo)
				{
					switch (m_eAtkCombo)
					{
					case ATK_COMBO_TYPE::COMBO_RLRL_SHOULDER:
						Play_Combo_RLRL_Shoulder();
						break;
					case ATK_COMBO_TYPE::COMBO_RLRL_SMASH:
						Play_Combo_RLRL_Smash();
						break;
					case ATK_COMBO_TYPE::COMBO_RLRL_SWING:
						Play_Combo_RLRL_Swing();
						break;
					}
				}
				else
				{
					switch (m_eState)
					{
					case YETI_ANI::Atk_Rush:
						Play_Rush();
						break;
					case YETI_ANI::Atk_RollingSlash:
						Play_RollingSlash();
						break;
					case YETI_ANI::Atk_IceThrowing:
						Play_IceThrowing();
						break;
					case YETI_ANI::Atk_Sp06:
						Play_RLRL();
						break;
					case YETI_ANI::Atk_Sp05:
						Play_WoodChop();
						break;
					case YETI_ANI::Atk_Sp04:
						Play_FastLR();
						break;
					case YETI_ANI::Atk_Sp03:
						Play_LRSweap();
						break;
					case YETI_ANI::Atk_Sp02:
						Play_RUpperChop();
						break;
					case YETI_ANI::Atk_Sp01:
						Play_SlowLR();
						break;
					case YETI_ANI::Atk_Field:
						//자가 버프 혹은 근처에 일렁거리며 데미지 주기
						Play_Howling();
						break;
					case YETI_ANI::Atk_BodyPress:
						Play_BodyPress();
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

void CYeti::Check_DeadEffect(_double TimeDelta)
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
}

void CYeti::Check_FootSound()
{
	if (YETI_ANI::Walk_F == m_eState ||
		YETI_ANI::Walk_L == m_eState ||
		YETI_ANI::Walk_R == m_eState ||
		YETI_ANI::Run == m_eState)
	{
		m_fFootSound += DELTA_60;

		if (m_fFootSound >= m_fFootSoundMax)
		{
			m_fFootSound = 0.f;

			g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Step);

			switch (CALC::Random_Num(0, 5))
			{
			case 0:
				g_pSoundManager->Play_Sound(L"Step0.ogg", CSoundManager::Yeti_Step, CSoundManager::Effect_Sound);
				break;

			case 1:
				g_pSoundManager->Play_Sound(L"Step1.ogg", CSoundManager::Yeti_Step, CSoundManager::Effect_Sound);
				break;

			case 2:
				g_pSoundManager->Play_Sound(L"Step2.ogg", CSoundManager::Yeti_Step, CSoundManager::Effect_Sound);
				break;

			case 3:
				g_pSoundManager->Play_Sound(L"Step3.ogg", CSoundManager::Yeti_Step, CSoundManager::Effect_Sound);
				break;

			case 4:
				g_pSoundManager->Play_Sound(L"Step4.ogg", CSoundManager::Yeti_Step, CSoundManager::Effect_Sound);
				break;

			case 5:
				g_pSoundManager->Play_Sound(L"Step5.ogg", CSoundManager::Yeti_Step, CSoundManager::Effect_Sound);
				break;
			}
		}
	}

	return;
}

void CYeti::Play_SequenceAtk()
{
	switch (m_iPatternCount)
	{
	case 0:
		m_eState = YETI_ANI::Atk_Rush;
		break;

	case 1:
		m_eState = YETI_ANI::Atk_RollingSlash;
		break;

	case 2:
		m_eState = YETI_ANI::Atk_IceThrowing;
		break;

	case 3:
		m_eState = YETI_ANI::Atk_Sp06;
		break;

	case 4:
		m_eState = YETI_ANI::Atk_Sp05;
		break;

	case 5:
		m_eState = YETI_ANI::Atk_Sp04;
		break;

	case 6:
		m_eState = YETI_ANI::Atk_Sp03;
		break;

	case 7:
		m_eState = YETI_ANI::Atk_Sp02;
		break;

	case 8:
		m_eState = YETI_ANI::Atk_Sp01;
		break;

	case 9:
		m_eState = YETI_ANI::Atk_BodyPress;
		break;

	case 10:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RLRL_SHOULDER;
		m_eState = YETI_ANI::Atk_Sp06;
		break;

	case 11:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RLRL_SMASH;
		m_eState = YETI_ANI::Atk_Sp06;
		break;

	case 12:
		m_bIsCombo = true;
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RLRL_SWING;
		m_eState = YETI_ANI::Atk_Sp06;
		break;
	}

	++m_iPatternCount;

	return;
}

void CYeti::Play_RandomAtkNormal()
{
	switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RUSH, ATK_NORMAL_TYPE::NORMAL_BODYPRESS))
	{
	case ATK_NORMAL_TYPE::NORMAL_RUSH:
		m_eState = YETI_ANI::Atk_Rush;
		break;
	case ATK_NORMAL_TYPE::NORMAL_ROLLINGSLASH:
		m_eState = YETI_ANI::Atk_RollingSlash;
		break;
	case ATK_NORMAL_TYPE::NORMAL_ICETHROW:
		m_eState = YETI_ANI::Atk_IceThrowing;
		break;
	case ATK_NORMAL_TYPE::NORMAL_R_L_R_L:
		m_eState = YETI_ANI::Atk_Sp06;
		break;
	case ATK_NORMAL_TYPE::NORMAL_WOODCHOP:
		m_eState = YETI_ANI::Atk_Sp05;
		break;
	case ATK_NORMAL_TYPE::NORMAL_FASTL_R:
		m_eState = YETI_ANI::Atk_Sp04;
		break;
	case ATK_NORMAL_TYPE::NORMAL_L_R_SWEAP:
		m_eState = YETI_ANI::Atk_Sp03;
		break;
	case ATK_NORMAL_TYPE::NORMAL_R_UPPER_ACCELDOWN:
		m_eState = YETI_ANI::Atk_Sp02;
		break;
	case ATK_NORMAL_TYPE::NORMAL_SLOWL_R:
		m_eState = YETI_ANI::Atk_Sp01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_HOULING:
		//m_eState = YETI_ANI::Atk_Field;
		//break;
	case ATK_NORMAL_TYPE::NORMAL_BODYPRESS:
		m_eState = YETI_ANI::Atk_BodyPress;
		break;
	}

	return;
}

void CYeti::Play_RandomAtkCombo()
{
	switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_RLRL_SHOULDER, ATK_COMBO_TYPE::COMBO_RLRL_SWING))
	{
	case ATK_COMBO_TYPE::COMBO_RLRL_SHOULDER:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RLRL_SHOULDER;
		m_eState = YETI_ANI::Atk_Sp06;
		break;
	case ATK_COMBO_TYPE::COMBO_RLRL_SMASH:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RLRL_SMASH;
		m_eState = YETI_ANI::Atk_Sp06;
		break;
	case ATK_COMBO_TYPE::COMBO_RLRL_SWING:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RLRL_SWING;
		m_eState = YETI_ANI::Atk_Sp06;
		break;
	}

	return;
}

void CYeti::Play_BodyPress()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (4.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.200f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}

			}
		}
		else if (4.f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (3.133f < AniTime && 4.200f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_Howling()
{
	//미정, 버프, 범위 왜곡와 공격판정
	return;
}

void CYeti::Play_SlowLR()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (3.533f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.400f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.2f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (2.133f < AniTime && 3.000f > AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.433f < AniTime && 0.900f > AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_RUpperChop()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (4.800f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.633f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.1f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.267f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (3.233f < AniTime && 4.400f > AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.200f < AniTime && 2.733f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.533f < AniTime && 0.967f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_LRSweap()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (2.300f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.1f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.633f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.567f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.3f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.900f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (0.5f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (1.833f < AniTime && 1.933f > AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.367f < AniTime && 1.833f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.500f < AniTime && 0.800f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_FastLR()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (2.400f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.300f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.1f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.867f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (0.4f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (1.433f < AniTime && 2.000f > AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.567f < AniTime && 0.800f > AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_WoodChop()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
	}

	return;
}

void CYeti::Play_RLRL()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.2f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.3f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.4f <= AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.3f <= AniTime)
		{
			if (false == m_bEventTrigger[16])
			{
				m_bEventTrigger[16] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (4.333f < AniTime && 4.567f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (3.467f < AniTime && 3.900f > AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.633f < AniTime && 2.900f > AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.533f < AniTime && 2.133f > AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.767f < AniTime && 1.333f > AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_IceThrowing()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_v3 vBirth, vLook;
	_float fLength = 1.f;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		//1.500 땅에서 집어들고 잇음 //4.400 투척
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.6f;
			return;
		}
		else if (4.400f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_RightHand] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_YetiBullet", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 16.f, 1.5));
			}
		}
		else if (4.f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (3.400f < AniTime && 4.133f > AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
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

void CYeti::Play_RollingSlash()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (2.267f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.1f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.033f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.900f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.600f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.403f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.312f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (1.500f < AniTime && 1.900f > AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_Rush()
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
			m_fCoolDownMax = 0.4f;

			return;
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.533f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.3f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				switch (CALC::Random_Num(0, 6))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (0.533f < AniTime && 1.300f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CYeti::Play_Combo_RLRL_Shoulder()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YETI_ANI::Atk_Sp06 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = YETI_ANI::Atk_Shoulder;

			m_vecAttackCol[1]->Set_Enabled(false);
			m_vecAttackCol[2]->Set_Enabled(false);

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.2f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.3f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.3f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.3f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				
				m_dAniPlayMul = 1;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);
			}
		}

		if (4.333f < AniTime && 4.567f > AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (3.467f < AniTime && 3.900f > AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.633f < AniTime && 2.900f > AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.533f < AniTime && 2.133f > AniTime)
		{
			if (false == m_bEventTrigger[16])
			{
				m_bEventTrigger[16] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.767f < AniTime && 1.333f > AniTime)
		{
			if (false == m_bEventTrigger[17])
			{
				m_bEventTrigger[17] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (YETI_ANI::Atk_Shoulder == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.85f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.8f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (1.067f <= AniTime)
		{
			if (false == m_bEventTrigger[18])
			{
				m_bEventTrigger[18] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.800f <= AniTime)
		{
			if (false == m_bEventTrigger[19])
			{
				m_bEventTrigger[19] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
				m_dAniPlayMul = 1;
			}
		}
		else if (0.5f <= AniTime)
		{
			if (false == m_bEventTrigger[20])
			{
				m_bEventTrigger[20] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);


				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (0.400f < AniTime && 0.967f > AniTime)
		{
			if (false == m_bEventTrigger[21])
			{
				m_bEventTrigger[21] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
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

void CYeti::Play_Combo_RLRL_Smash()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YETI_ANI::Atk_Sp06 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = YETI_ANI::Atk_Smash;

			m_vecAttackCol[1]->Set_Enabled(false);
			m_vecAttackCol[2]->Set_Enabled(false);

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.3f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.4f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.4f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.4f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;

				m_dAniPlayMul = 1;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);
			}
		}

		if (4.333f < AniTime && 4.567f > AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (3.467f < AniTime && 3.900f > AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.633f < AniTime && 2.900f > AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.533f < AniTime && 2.133f > AniTime)
		{
			if (false == m_bEventTrigger[16])
			{
				m_bEventTrigger[16] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.767f < AniTime && 1.333f > AniTime)
		{
			if (false == m_bEventTrigger[17])
			{
				m_bEventTrigger[17] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (YETI_ANI::Atk_Smash == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.945f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.8f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[18])
			{
				m_bEventTrigger[18] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.633f <= AniTime)
		{
			if (false == m_bEventTrigger[19])
			{
				m_bEventTrigger[19] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.3f <= AniTime)
		{
			if (false == m_bEventTrigger[20])
			{
				m_bEventTrigger[20] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.3f <= AniTime)
		{
			if (false == m_bEventTrigger[21])
			{
				m_bEventTrigger[21] = true;

				m_dAniPlayMul = 1;
			}
		}
		else if (1.f <= AniTime)
		{
			if (false == m_bEventTrigger[22])
			{
				m_bEventTrigger[22] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);
			}
		}

		if (1.033f < AniTime && 2.633f > AniTime)
		{
			if (false == m_bEventTrigger[22])
			{
				m_bEventTrigger[22] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
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

void CYeti::Play_Combo_RLRL_Swing()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YETI_ANI::Atk_Sp06 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_tObjParam.bSuperArmor = true;
			m_vecAttackCol[1]->Set_Enabled(false);
			m_vecAttackCol[2]->Set_Enabled(false);
			m_eState = YETI_ANI::Atk_Swing;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (4.3f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (3.4f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (2.4f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.4f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;

				m_dAniPlayMul = 1;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);
			}
		}

		if (4.333f < AniTime && 4.567f > AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (3.467f < AniTime && 3.900f > AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.633f < AniTime && 2.900f > AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.533f < AniTime && 2.133f > AniTime)
		{
			if (false == m_bEventTrigger[16])
			{
				m_bEventTrigger[16] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.767f < AniTime && 1.333f > AniTime)
		{
			if (false == m_bEventTrigger[17])
			{
				m_bEventTrigger[17] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (YETI_ANI::Atk_Swing == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.945f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.8f;

			m_pBattleAgentCom->Set_RimColor(_v4(0.f, 0.f, 0.f, 0.f));
			m_pBattleAgentCom->Set_RimAlpha(0.5f);
			m_pBattleAgentCom->Set_RimValue(8.f);

			return;
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[18])
			{
				m_bEventTrigger[18] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.833f <= AniTime)
		{
			if (false == m_bEventTrigger[19])
			{
				m_bEventTrigger[19] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_01);
				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_SFX_02);

				switch (CALC::Random_Num(0, 3))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub0.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub1.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub2.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_BladeWeapon_Sub3.ogg", CSoundManager::Yeti_SFX_01, CSoundManager::Effect_Sound);
					break;
				}

				switch (CALC::Random_Num(0, 4))
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub0.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub1.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub2.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub3.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Monster_Swing_Sub4.ogg", CSoundManager::Yeti_SFX_02, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.5f <= AniTime)
		{
			if (false == m_bEventTrigger[20])
			{
				m_bEventTrigger[20] = true;

				m_dAniPlayMul = 1;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 6);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 3:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice3.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 4:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice4.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 5:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice5.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 6:
					g_pSoundManager->Play_Sound(L"Yeti_Atk_Voice6.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}
		else if (1.f <= AniTime)
		{
			if (false == m_bEventTrigger[21])
			{
				m_bEventTrigger[21] = true;

				m_dAniPlayMul = 0.5;

				m_pBattleAgentCom->Set_RimColor(_v4(1.f, 0.f, 0.f, 0.f));
				m_pBattleAgentCom->Set_RimAlpha(1.f);
				m_pBattleAgentCom->Set_RimValue(5.f);
			}
		}

		if (1.267f < AniTime && 1.667f > AniTime)
		{
			if (false == m_bEventTrigger[22])
			{
				m_bEventTrigger[22] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
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

void CYeti::Play_Idle()
{
	if (nullptr != m_pAggroTarget)
	{
		if (true == m_bInRecognitionRange)
		{
			m_bIsIdle = false;

			if (true == m_tObjParam.bCanAttack)
			{
				m_eState = YETI_ANI::NF_Threat_Loop;
				if (nullptr != m_pAggroTarget)
					Function_RotateBody(m_pAggroTarget);
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
						m_bIsIdle = true;

						m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
						m_eState = YETI_ANI::Idle;

						return;
					}
					else
						Function_RotateBody(m_pAggroTarget);
				}
				else
					Function_RotateBody(m_pAggroTarget);

				m_eState = YETI_ANI::NF_Threat_Loop;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = YETI_ANI::Idle;
		}
	}
	else
	{
		m_bIsIdle = true;
		m_eState = YETI_ANI::Idle;
	}

	return;
}

void CYeti::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVE_TYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = YETI_ANI::Run;
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

			m_fFootSoundMax = 0.46f;

			switch (CALC::Random_Num(YETI_ANI::Walk_R, YETI_ANI::Walk_B))
			{
			case YETI_ANI::Walk_R:
			case YETI_ANI::Walk_FR:
			case YETI_ANI::Walk_F:
			case YETI_ANI::Walk_FL_RFoot:
			case YETI_ANI::Walk_BR_RFoot:
				m_eState = YETI_ANI::Walk_R;
				break;
			case YETI_ANI::Walk_B:
			case YETI_ANI::Walk_L:
			case YETI_ANI::Walk_BR_LFoot:
			case YETI_ANI::Walk_FL_LFoot:
			case YETI_ANI::Walk_BL:
				m_eState = YETI_ANI::Walk_L;
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

					Play_Idle();

					return;
				}
				else
				{
					if (YETI_ANI::Walk_R == m_eState)
						Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
					else if (YETI_ANI::Walk_L == m_eState)
						Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
				}
			}
			else
			{
				if (YETI_ANI::Walk_R == m_eState)
					Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
				else if (YETI_ANI::Walk_L == m_eState)
					Function_MoveAround(m_pAggroTarget, m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
			}
		}
		break;

	case MONSTER_MOVE_TYPE::MOVE_WALK:
		m_eState = YETI_ANI::Walk_F;
		m_fFootSoundMax = 0.41f;

		if (nullptr == m_pAggroTarget)
		{
			Function_Find_Target();

			if (nullptr == m_pAggroTarget)
			{
				Function_ResetAfterAtk();
				m_fCoolDownMax = 0.f;
				m_fCoolDownCur = 0.f;
				m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;

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
	}

	return;
}

void CYeti::Play_Hit()
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
			m_eState = YETI_ANI::Dmg_F;
			break;
		case FBLR::BACK:
		case FBLR::BACKLEFT:
		case FBLR::BACKRIGHT:
		case FBLR::RIGHT:
			m_eState = YETI_ANI::Dmg_B;
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

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 2);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Hit0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Hit1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Hit2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
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

void CYeti::Play_CC()
{
	return;
}

void CYeti::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();

		m_bCanPlayDead = true;
		m_eState = YETI_ANI::Death;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bEnable = false;
			m_dAniPlayMul = 0;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;

				g_pSoundManager->Stop_Sound(CSoundManager::Yeti_Voice);

				m_iRandom = CALC::Random_Num(0, 2);

				switch (m_iRandom)
				{
				case 0:
					g_pSoundManager->Play_Sound(L"Yeti_Death0.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 1:
					g_pSoundManager->Play_Sound(L"Yeti_Death1.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				case 2:
					g_pSoundManager->Play_Sound(L"Yeti_Death2.ogg", CSoundManager::Yeti_Voice, CSoundManager::Effect_Sound);
					break;
				}
			}
		}

		if (5.433f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;

				Start_Dissolve(0.7f, false, true, 0.0f);
				m_fDeadEffect_Delay = 0.f;

				Give_Mana_To_Player(5);

				if (false == m_pRigidCom->Get_IsFall())
					Check_DropItem(MONSTER_NAMETYPE::M_Yeti);

				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 0.f));
			}
		}

		if (5.733f < AniTime && 6.467f > AniTime)
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
		else if (1.567f < AniTime && 1.900f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 1.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

HRESULT CYeti::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Yeti", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

	m_pColliderCom->Set_Radius(_v3{1.2f, 1.2f, 1.2f});
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CYeti::SetUp_ConstantTable(CShader* pShader)
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

HRESULT CYeti::Ready_Status(void * pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("Create CYeti pArgument nullptr Failed");
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

	m_tObjParam.fDamage = 220.f * pow(1.5f, g_eStageIdx_Cur - 1);
	m_tObjParam.fHp_Max = 2000.f * pow(1.5f, g_eStageIdx_Cur - 1);
	m_tObjParam.fArmor_Max = 80.f* pow(1.5f, g_eStageIdx_Cur - 1);;

	m_fRecognitionRange = 15.f;
	m_fShotRange = 10.f;
	m_fAtkRange = 5.f;
	m_fPersonalRange = 2.f;
	m_iDodgeCountMax = 5;

	m_eFirstCategory = MONSTER_STATE_TYPE::IDLE;
	m_eState = YETI_ANI::Idle;

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

	m_iPatternCount = 0;
	m_iPatternCountMax = 13;

	return S_OK;
}

HRESULT CYeti::Ready_Collider()
{
	m_vecPhysicCol.reserve(2);
	m_vecAttackCol.reserve(4);
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
	fRadius = 2.f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 2.f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider); //뭉개기용

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 1.2f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftHand]->_41, m_matBone[Bone_LeftHand]->_42, m_matBone[Bone_LeftHand]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 1.2f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightHand]->_41, m_matBone[Bone_RightHand]->_42, m_matBone[Bone_RightHand]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 1.2f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Shoulder]->_41, m_matBone[Bone_Shoulder]->_42, m_matBone[Bone_Shoulder]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CYeti::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Hips", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach", 0), E_FAIL);
	m_matBone[Bone_LeftHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach", 0), E_FAIL);
	m_matBone[Bone_RightHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftForeArm", 0), E_FAIL);
	m_matBone[Bone_Shoulder] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CYeti::Ready_Rigid()
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

CYeti* CYeti::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CYeti* pInstance = new CYeti(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CYeti");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CYeti::Clone_GameObject(void * pArg)
{
	CYeti* pInstance = new CYeti(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CYeti");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYeti::Free()
{
	CMonster::Free();

	return;
}
