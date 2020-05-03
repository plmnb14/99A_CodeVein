#include "stdafx.h"
#include "..\Headers\Pet_PoisonButterFly.h"

CPet_PoisonButterFly::CPet_PoisonButterFly(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CPet(pGraphic_Device)
{
}

CPet_PoisonButterFly::CPet_PoisonButterFly(const CPet_PoisonButterFly & rhs)
	:CPet(rhs)
{
}

HRESULT CPet_PoisonButterFly::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPet_PoisonButterFly::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(_v3(0.25f,0.25f,0.25f));

	Ready_Status(pArg);
	Ready_BoneMatrix(pArg);
	Ready_Collider(pArg);
	Ready_Weapon(pArg);

	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBone[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	m_pPlayer = &(*g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	if (nullptr != m_pPlayer)
		Safe_AddRef(m_pPlayer);

	return S_OK;
}

_int CPet_PoisonButterFly::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	//현재 모드랑 쓰던 모드랑 다를경우, 목표 재탐색, 우선순위에 따라 타겟을 재 정렬
	m_pMonsterUI->Update_GameObject(TimeDelta);

	//Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	PET_STATE_TYPE::DEAD != m_eFirstCategory ? Check_CollisionEvent() : Check_DeadEffect(TimeDelta);

	return NO_EVENT;
}

_int CPet_PoisonButterFly::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL(m_pTarget) m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Late_Update_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CPet_PoisonButterFly::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

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
			if (PET_STATE_TYPE::DEAD != m_eFirstCategory)
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Update_GameObject(m_dTimeDelta);

	Update_Collider();
	Render_Collider();

	return S_OK;
}

HRESULT CPet_PoisonButterFly::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
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

void CPet_PoisonButterFly::Update_Collider()
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

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return;
}

void CPet_PoisonButterFly::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CPet_PoisonButterFly::Check_Hit()
{
	if (PET_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	Function_Find_Target();

	if (nullptr == m_pTarget)
	{
		m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
		return;
	}
	else
	{
		switch (m_eTarget)
		{
		case PET_TARGET_TYPE::PET_TARGET_BOSS:
			break;
		case PET_TARGET_TYPE::PET_TARGET_MONSTER:
			break;
		case PET_TARGET_TYPE::PET_TARGET_ITEM:
			break;
		case PET_TARGET_TYPE::PET_TARGET_NONE:
			break;
		}
	}

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (false == m_tObjParam.bSuperArmor)
			{
				if (true == m_tObjParam.bIsHit)
				{
					if (true == m_tObjParam.bHitAgain)
					{
						m_eFirstCategory = PET_STATE_TYPE::HIT;
						Function_FBLR(m_pTarget);
						m_tObjParam.bHitAgain = false;
						m_pMeshCom->Reset_OldIndx();
					}
				}
				else
				{
					m_eFirstCategory = PET_STATE_TYPE::HIT;
					Function_FBLR(m_pTarget);
				}
			}
		}
	}
	else
		m_eFirstCategory = PET_STATE_TYPE::DEAD;

	return;
}

void CPet_PoisonButterFly::Check_Dist()
{
	if (PET_STATE_TYPE::HIT == m_eFirstCategory ||
		PET_STATE_TYPE::CC == m_eFirstCategory ||
		PET_STATE_TYPE::DEAD == m_eFirstCategory)
		return;

	if (g_pInput_Device->Key_Down(DIK_P))
	{
		if (m_iCount > 2)
			m_iCount = 0;

		++m_iCount;

		switch (m_iCount)
		{
		case 1:
			m_eNowPetMode = PET_MODE_TYPE::PET_MODE_ATK;
			break;
		case 2:
			m_eNowPetMode = PET_MODE_TYPE::PET_MODE_UTILL;
			break;
		}

		if (m_eNowPetMode != m_eOldPetMdoe)
		{
			//모드가 다르고 타겟이 있음
			if (nullptr != m_pTarget)
			{
				m_eOldPetMdoe = m_eNowPetMode;
				Safe_Release(m_pTarget);
				m_pTarget = nullptr;

				Function_ResetAfterAtk();
				m_tObjParam.bCanAttack = true;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;


				Function_Find_Target();
			}
			//모드 바뀌고 타겟 없음
			else
			{
				m_eOldPetMdoe = m_eNowPetMode;
				Function_ResetAfterAtk();
				m_pTarget = nullptr;

				m_tObjParam.bCanAttack = true;
				m_fCoolDownCur = 0.f;
				m_fCoolDownMax = 0.f;

				Function_Find_Target();
			}
		}
	}

	//기존 목표 있음
	if (nullptr != m_pTarget)
	{
		//목표 사망, 공장 초기화
		if (true == m_pTarget->Get_Dead() ||
			false == m_pTarget->Get_Enable())
		{
			Safe_Release(m_pTarget);
			Function_ResetAfterAtk();

			m_pTarget = nullptr;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;

			m_tObjParam.bCanAttack = true;
			m_fCoolDownCur = 0.f;
			m_fCoolDownMax = 0.f;
		}
	}

	if (true == m_bIsCombo ||
		true == m_bIsMoveAround ||
		true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsDodge ||
		true == m_tObjParam.bIsHit)
		return;

	_float fPlayerDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos()));

	m_fLimitRange >= fPlayerDist ? m_bInLimitRange = true : m_bInLimitRange = false; //Player와의 최대 거리
	m_fActiveRange >= fPlayerDist ? m_bInActiveRange = true : m_bInActiveRange = false; //자유 행동 거리

	//최대거리 In
	if (true == m_bInLimitRange)
	{
		//자유 행동 거리 In
		if (true == m_bInActiveRange)
		{
			//기존 목표 있음
			if (nullptr != m_pTarget)
			{
				//죽음
				if (true == m_pTarget->Get_Dead() ||
					false == m_pTarget->Get_Enable())
				{
					Safe_Release(m_pTarget);
					m_pTarget = nullptr;

					Function_Find_Target();
					Check_Action();
				}
				//안 죽음
				else
					Check_Action();
			}
			//기존 목표 없음
			else
			{
				Function_Find_Target();
				Check_Action();
			}
		}
		//자유 행동 거리 out
		else
		{
			//기존 목표 있음
			if (nullptr != m_pTarget)
			{
				//죽음
				if (true == m_pTarget->Get_Dead() ||
					false == m_pTarget->Get_Enable())
				{
					Safe_Release(m_pTarget);
					m_pTarget = nullptr;

					Function_Find_Target();
					Check_Action();
				}
				//안죽음
				else
					Check_Action();
			}
			//기존 목표 없음
			else
			{
				//최대거리 이내에서 달려오다가 특정 거리부터는 감속
				Function_CalcMoveSpeed(m_fActiveRange);
				m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;

				//특정거리보다 가까워진 경우, 속도를 잃고 idle
				if (0.f >= m_fSkillMoveSpeed_Cur)
				{
					m_eFirstCategory = PET_STATE_TYPE::IDLE;
					m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
				}
				//특정거리보다 멀어진 경우, 여전히 달린다
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = PET_STATE_TYPE::MOVE;
					m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
				}
			}
		}
	}
	//최대거리 Out
	else
	{
		//항상 복귀, 타겟 초기화
		if (nullptr != m_pTarget)
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
		}

		m_bCanChase = true;
		m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
		m_eFirstCategory = PET_STATE_TYPE::MOVE;
		m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
	}

	return;
}

void CPet_PoisonButterFly::Check_Action()
{
	_float fTargetDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pTarget)->Get_Pos() - m_pTransformCom->Get_Pos()));
	m_fRecognitionRange >= fTargetDist ? m_bInRecognitionRange = true : m_bInRecognitionRange = false; //목표와 인지범위 여부
	m_fShotRange >= fTargetDist ? m_bInShotRange = true : m_bInShotRange = false; //목표와 원거리 공격 범위 여부
	m_fAtkRange >= fTargetDist ? m_bInAtkRange = true : m_bInAtkRange = false; //목표와 근거리 공격범위 여부

	switch (m_eTarget)
	{
	case PET_TARGET_TYPE::PET_TARGET_BOSS:
	case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		//인지 범위 o
		if (true == m_bInRecognitionRange)
		{
				//근거리 범위o
				if (true == m_bInAtkRange)
				{
					//쿨타임x
					if (true == m_tObjParam.bCanAttack)
						m_eFirstCategory = PET_STATE_TYPE::ATTACK;
					//쿨타임o
					else
					{
						m_bCanMoveAround = true;
						m_eFirstCategory = PET_STATE_TYPE::MOVE;
						m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_ALERT;
					}
				}
				//근거리 범위x
				else
				{
					m_eFirstCategory = PET_STATE_TYPE::MOVE;
					m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_WALK;
				}
		}
		//인지 범위x
		else
		{
			//해당 목표로 추적
			m_bCanChase = true;
			m_eFirstCategory = PET_STATE_TYPE::MOVE;
			m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
		}
		break;
	case PET_TARGET_TYPE::PET_TARGET_ITEM:
		if (true == m_bInRecognitionRange)
		{
			if (true == m_bInAtkRange)
			{
				//아이템 획득, 아이템을 공격해서 얻거나 죽이거나 할듯
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
		break;

		break;
	case PET_TARGET_TYPE::PET_TARGET_NONE:
		//50m 이내의 거리, 목표없음, player를 향해 복귀
		//최대거리 이내에서 달려오다가 특정 거리부터는 감속
		Function_CalcMoveSpeed(m_fActiveRange);

		//특정거리보다 가까워진 경우, 속도를 잃고 idle
		if (0.f >= m_fSkillMoveSpeed_Cur)
		{
			m_eFirstCategory = PET_STATE_TYPE::IDLE;
			m_eSecondCategory_IDLE = PET_IDLE_TYPE::IDLE_IDLE;
		}
		//특정거리보다 멀어진 경우, 여전히 달린다
		else
		{
			m_eFirstCategory = PET_STATE_TYPE::MOVE;
			m_eSecondCategory_MOVE = PET_MOVE_TYPE::MOVE_RUN;
		}
		break;
	}

	return;
}

void CPet_PoisonButterFly::Check_AniEvent()
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
				m_iRandom = CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_5SHOT, ATK_NORMAL_TYPE::NORMAL_MIST);
				switch (m_iRandom)
				{
				case ATK_NORMAL_TYPE::NORMAL_5SHOT:
					m_eState = PET_POISIONBUTTERFLY_ANI::Atk_5wayShoot;
					break;
				case ATK_NORMAL_TYPE::NORMAL_MIST:
					m_eState = PET_POISIONBUTTERFLY_ANI::Atk_AllRangeShoot;
					break;
				}
			}
			else
			{
				m_iRandom = CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_MIST, ATK_NORMAL_TYPE::NORMAL_POISONWHEELWIND);

				switch (m_iRandom)
				{
				case ATK_NORMAL_TYPE::NORMAL_MIST:
					m_eState = PET_POISIONBUTTERFLY_ANI::Atk_AllRangeShoot;
					break;
				case ATK_NORMAL_TYPE::NORMAL_POISONWHEELWIND:
					m_eState = PET_POISIONBUTTERFLY_ANI::Atk_PoisonMine;
					break;
				}
			}
			return;
		}
		else
		{
			switch (m_eState)
			{
			case PET_POISIONBUTTERFLY_ANI::Atk_5wayShoot:
				Play_5Shot();
				break;
			case PET_POISIONBUTTERFLY_ANI::Atk_AllRangeShoot:
				Play_Mist();
				break;
			case PET_POISIONBUTTERFLY_ANI::Atk_PoisonMine:
				Play_PoisonWheelWind();
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
}

void CPet_PoisonButterFly::Check_DeadEffect(_double TimeDelta)
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

void CPet_PoisonButterFly::Play_5Shot()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_v3 vBirth, vLook;
	_float fLength = 1.f;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = false;
	}
	else
	{
		//원하는 방향으로 5발을 쏴준다
		if (m_pMeshCom->Is_Finish_Animation(0.955f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.0f;

			return;
		}
		else if (2.900f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_Head] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 5.f));
			}
		}
		else if (0.f <= AniTime)
			if(PET_TARGET_TYPE::PET_TARGET_NONE != m_eTarget)
				Function_RotateBody(m_pTarget);
	}

	return;

}

void CPet_PoisonButterFly::Play_Mist()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_v3 vBirth, vLook;
	_float fLength = 1.f;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = false;
	}
	else
	{
		//전 방향으로 발사
		//기모아서 방출하는 느낌
		if (m_pMeshCom->Is_Finish_Animation(0.955f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.0f;

			return;
		}
		else if (6.800f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 6.f, 5.f));
			}
		}
	}

	return;
}

void CPet_PoisonButterFly::Play_PoisonWheelWind()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_mat matBone1, matBone2, matBone;
	_v3 vBirth, vShotDir, vMakeDirPoint1, vMakeDirPoint2;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = false;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.0f;

			return;
		}
		else if(4.633f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (4.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (4.233f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (4.100f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.967f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.833f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.433f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.300f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.167f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (3.033f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (2.900f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}
		else if (2.767f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_tObjParam.bSuperArmor = true;
				matBone1 = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();
				matBone2 = *m_matBone[Bone_Tail] * m_pTransformCom->Get_WorldMat();
				matBone = *m_matBone[Bone_TailTongue] * m_pTransformCom->Get_WorldMat();

				memcpy(vMakeDirPoint1, &matBone1._41, sizeof(_v3));
				memcpy(vMakeDirPoint2, &matBone2._41, sizeof(_v3));
				memcpy(vBirth, &matBone._41, sizeof(_v3));
				V3_NORMAL(&vShotDir, &(vMakeDirPoint1 - vMakeDirPoint2));

				CObjectPool_Manager::Get_Instance()->Create_Object(L"Pet_Bullet", &PET_BULLET_STATUS(PET_BULLET_TYPE::PET_BULLET_POISON, vBirth, vShotDir, 8.f, 1.5f));
			}
		}

		if (2.867f < AniTime && 5.367f > AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
}

void CPet_PoisonButterFly::Play_Idle()
{
	switch (m_eTarget)
	{
	case PET_TARGET_TYPE::PET_TARGET_BOSS:
	case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		Function_RotateBody(m_pTarget);
		m_eState = PET_POISIONBUTTERFLY_ANI::Idle;
		break;
	case PET_TARGET_TYPE::PET_TARGET_ITEM:
		Function_RotateBody(m_pTarget);
		m_eState = PET_POISIONBUTTERFLY_ANI::Idle;
		break;
	case PET_TARGET_TYPE::PET_TARGET_NONE:
		Function_RotateBody(m_pPlayer);
		m_eState = PET_POISIONBUTTERFLY_ANI::Idle;
		break;
	}

	return;
}

void CPet_PoisonButterFly::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case PET_MOVE_TYPE::MOVE_WALK:
		m_fSkillMoveSpeed_Cur = 2.f;
		switch (m_eTarget)
		{
		case PET_TARGET_TYPE::PET_TARGET_BOSS:
		case PET_TARGET_TYPE::PET_TARGET_MONSTER:
		case PET_TARGET_TYPE::PET_TARGET_ITEM:
			m_eState = PET_POISIONBUTTERFLY_ANI::Walk_F;

			Function_RotateBody(m_pTarget);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		case PET_TARGET_TYPE::PET_TARGET_NONE:
			m_eState = PET_POISIONBUTTERFLY_ANI::Walk_F;

			Function_RotateBody(m_pPlayer);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		}
		break;
	case PET_MOVE_TYPE::MOVE_ALERT:
		switch (m_eTarget)
		{
		case PET_TARGET_TYPE::PET_TARGET_BOSS:
		case PET_TARGET_TYPE::PET_TARGET_MONSTER:
			if (true == m_bCanMoveAround)
			{
				m_bCanMoveAround = false;
				m_bIsMoveAround = true;

				m_bCanCoolDown = true;
				m_fCoolDownMax = CALC::Random_Num(2, 4) * 1.0f;

				m_fSkillMoveSpeed_Cur = 2.5f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;

				switch (CALC::Random_Num(PET_POISIONBUTTERFLY_ANI::Walk_R, PET_POISIONBUTTERFLY_ANI::Walk_B))
				{
				case PET_POISIONBUTTERFLY_ANI::Walk_R:
				case PET_POISIONBUTTERFLY_ANI::Walk_F:
					m_eState = PET_POISIONBUTTERFLY_ANI::Walk_R;
					break;
				case PET_POISIONBUTTERFLY_ANI::Walk_L:
				case PET_POISIONBUTTERFLY_ANI::Walk_B:
					m_eState = PET_POISIONBUTTERFLY_ANI::Walk_L;
					break;
				}
			}
			else
			{
				if (PET_POISIONBUTTERFLY_ANI::Walk_R == m_eState)
					Function_MoveAround(m_pTarget, m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
				else if (PET_POISIONBUTTERFLY_ANI::Walk_L == m_eState)
					Function_MoveAround(m_pTarget, m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
			}
			break;
		case PET_TARGET_TYPE::PET_TARGET_ITEM:
			break;
		case PET_TARGET_TYPE::PET_TARGET_NONE:
			if (true == m_bCanMoveAround)
			{
				m_bCanMoveAround = false;
				m_bIsMoveAround = true;

				m_bCanCoolDown = true;
				m_fCoolDownMax = CALC::Random_Num(2, 4) * 1.0f;

				m_fSkillMoveSpeed_Cur = 2.5f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;

				switch (CALC::Random_Num(PET_POISIONBUTTERFLY_ANI::Walk_R, PET_POISIONBUTTERFLY_ANI::Walk_B))
				{
				case PET_POISIONBUTTERFLY_ANI::Walk_R:
				case PET_POISIONBUTTERFLY_ANI::Walk_F:
					m_eState = PET_POISIONBUTTERFLY_ANI::Walk_R;
					break;
				case PET_POISIONBUTTERFLY_ANI::Walk_L:
				case PET_POISIONBUTTERFLY_ANI::Walk_B:
					m_eState = PET_POISIONBUTTERFLY_ANI::Walk_L;
					break;
				}
			}
			else
			{
				if (PET_POISIONBUTTERFLY_ANI::Walk_R == m_eState)
					Function_MoveAround(m_pPlayer, m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
				else if (PET_POISIONBUTTERFLY_ANI::Walk_L == m_eState)
					Function_MoveAround(m_pPlayer, m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
			}
			break;
		}
		break;
	case PET_MOVE_TYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = PET_POISIONBUTTERFLY_ANI::Walk_F;
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
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		case PET_TARGET_TYPE::PET_TARGET_NONE:
			Function_RotateBody(m_pPlayer);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		}
		break;
	case MOVE_DODGE:
		if (true == m_tObjParam.bCanDodge)
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanDodge = false;
			m_tObjParam.bIsDodge = true;
			m_eState = PET_POISIONBUTTERFLY_ANI::Dodge;
		}
		else
		{
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_eFirstCategory = PET_STATE_TYPE::IDLE;
				m_tObjParam.bCanAttack = true;
				Function_ResetAfterAtk();

				return;
			}

			if (0.900f < AniTime && 1.300f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 10.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.25f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
		}
		break;
	}

	return;
}

void CPet_PoisonButterFly::Play_Hit()
{
	return;
}

void CPet_PoisonButterFly::Play_CC()
{
	return;
}

void CPet_PoisonButterFly::Play_Dead()
{
	return;
}

HRESULT CPet_PoisonButterFly::Add_Component(void * pArg)
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_PoisonButterFly", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.8f, 0.8f, 0.8f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CPet_PoisonButterFly::SetUp_ConstantTable()
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

HRESULT CPet_PoisonButterFly::Ready_Status(void * pArg)
{
	m_tObjParam.fDamage = -50.f; //힐량,공격량 등등
	m_tObjParam.fHp_Max = 2000.f;
	m_tObjParam.fArmor_Max = 10.f;

	m_fLimitRange = 30.f; //최대거리범위
	m_fActiveRange = 20.f; //자유행동범위

	m_fRecognitionRange = 15.f; //목표인지범위
	m_fShotRange = 10.f; //목표원거리범위
	m_fAtkRange = 5.f; //목표근거리범위
	m_fPersonalRange = 2.f; //사회적 거리두기 범위
	m_iDodgeCountMax = 5; //피격시 회피카운트

	m_eFirstCategory = PET_STATE_TYPE::IDLE;
	m_eNowPetMode = PET_MODE_TYPE::PET_MODE_ATK;
	m_eOldPetMdoe = PET_MODE_TYPE::PET_MODE_END;
	m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
	m_eFBLR = FBLR::FRONT;

	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fArmor_Cur = m_tObjParam.fArmor_Max;

	m_tObjParam.bCanHit = true; //맞기 가능
	m_tObjParam.bIsHit = false;	//맞기 진행중 아님
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

HRESULT CPet_PoisonButterFly::Ready_Weapon(void * pArg)
{
	return S_OK;
}

HRESULT CPet_PoisonButterFly::Ready_Collider(void * pArg)
{
	m_vecPhysicCol.reserve(2);
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
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);
	m_vecPhysicCol.push_back(pCollider);


	return S_OK;
}

HRESULT CPet_PoisonButterFly::Ready_BoneMatrix(void * pArg)
{
 	D3DXFRAME_DERIVED*	pFrame = nullptr;
	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6_Tongue2", 0), E_FAIL);
	m_matBone[Bone_TailTongue] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail4", 0), E_FAIL);
	m_matBone[Bone_Tail] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CPet_PoisonButterFly* CPet_PoisonButterFly::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPet_PoisonButterFly* pInstance = new CPet_PoisonButterFly(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPet_PoisonButterFly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_PoisonButterFly::Clone_GameObject(void * pArg)
{
	CPet_PoisonButterFly* pInstance = new CPet_PoisonButterFly(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPet_PoisonButterFly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_PoisonButterFly::Free()
{
	Safe_Release(m_pMonsterUI);

	IF_NOT_NULL(m_pTarget)
		Safe_Release(m_pTarget);

	IF_NOT_NULL(m_pPlayer)
		Safe_Release(m_pPlayer);

	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CPet::Free();

	return;
}

