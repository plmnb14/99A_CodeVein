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

	m_pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	if (nullptr != m_pPlayer)
		Safe_AddRef(m_pPlayer);

	return S_OK;
}

_int CPet_PoisonButterFly::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	m_pMonsterUI->Update_GameObject(TimeDelta);

	//Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	MONSTER_STATETYPE::DEAD != m_eFirstCategory ? Check_CollisionEvent() : Check_DeadEffect(TimeDelta);

	return NO_EVENT;
}

_int CPet_PoisonButterFly::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

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
			if (MONSTER_STATETYPE::DEAD != m_eFirstCategory)
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

void CPet_PoisonButterFly::Check_CollisionEvent()
{
	Check_CollisionPush();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));

	return;
}

void CPet_PoisonButterFly::Check_CollisionPush()
{
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	for (auto& list_iter : tmpList)
	{
		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSkillMoveSpeed_Cur * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				vDir.y = 0;

				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}

	return;
}

void CPet_PoisonButterFly::Check_CollisionHit(list<CGameObject*> plistGameObject)
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
						iter->Set_Target_CanHit(false);
						iter->Add_Target_Hp(m_tObjParam.fDamage);

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}
					}

					vecIter->Set_Enabled(false);

					g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

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

void CPet_PoisonButterFly::Check_Hit()
{
	if (MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	Function_Find_Target(m_fRecognitionRange);

	if (nullptr == m_pTarget)
	{
		return;
	}
	else
	{
		switch (m_eTarget)
		{
		case TARGET_BOSS:
			break;
		case TARGET_MONSTER:
			break;
		case TARGET_ITEM:
			break;
		case TARGET_NONE:
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
						m_eFirstCategory = MONSTER_STATETYPE::HIT;
						Function_FBLR(m_pTarget);
						m_tObjParam.bHitAgain = false;
						m_pMeshCom->Reset_OldIndx();
					}
				}
				else
				{
					m_eFirstCategory = MONSTER_STATETYPE::HIT;
					Function_FBLR(m_pTarget);
				}
			}
		}
	}
	else
		m_eFirstCategory = MONSTER_STATETYPE::DEAD;

	return;
}

void CPet_PoisonButterFly::Check_Dist()
{
	if (MONSTER_STATETYPE::HIT == m_eFirstCategory ||
		MONSTER_STATETYPE::CC == m_eFirstCategory ||
		MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_bIsCombo ||
		true == m_bIsMoveAround ||
		true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsDodge ||
		true == m_tObjParam.bIsHit)
		return;

	_float fPlayerDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos()));

	//Player와의 최대 거리
	m_fLimitRange >= fPlayerDist ? m_bInLimitRange = true : m_bInLimitRange = false;
	//자유 행동 거리
	m_fActiveRange >= fPlayerDist ? m_bInActiveRange = true : m_bInActiveRange = false;

	//최대거리 In
	if (true == m_bInLimitRange)
	{
		//자유 행동 거리 In
		if (true == m_bInActiveRange)
		{
			//범위 내의 보스,몬스터,아이탬 찾기
			Function_Find_Target(m_fRecognitionRange);

			//무언가를 찾았다면
			if (nullptr != m_pTarget)
			{
				cout << "타겟을 찾았습니다" << endl;
				Check_Target();
			}
			//아무것도 찾지 못했다면
			else
			{
				//자연스러운 속도 유지를 위한 함수
				Function_CalcMoveSpeed(m_fActiveRange);
				//사회적 거리두기
				if (m_fPersonalRange >= fPlayerDist)
				{
					cout << "타겟없음, Player와의 사회적 거리두기를 실천하십시오" << endl;
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_IDLE;
				}
				else
				{
					cout << "타겟없음, player 근처로 걸어갑니다" << endl;
					m_bCanMoveAround = false;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_WALK;
				}
			}
		}
		//자유 행동 거리 Out
		else
		{
			cout << "최대 거리안에 들어왔으나 자유행동거리 밖" << endl;
			m_bCanChase = true;
			m_eFirstCategory = MONSTER_STATETYPE::MOVE;
			m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_RUN;
		}
	}
	//최대거리 Out
	else
	{
		//순간 이동이 필요한 순간, 랜덤으로 플레이어 주변값으로 이동
		cout << "너무멀어서 순간이동이 필요한 순간" << endl;
		m_bCanChase = true;
		m_eFirstCategory = MONSTER_STATETYPE::MOVE;
		m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_RUN;
	}

	return;
}

void CPet_PoisonButterFly::Check_Target()
{
	//목표와의 거리 비교
	_float fTargetDist = V3_LENGTH(&(TARGET_TO_TRANS(m_pTarget)->Get_Pos() - m_pTransformCom->Get_Pos()));

	//목표와 인지범위 여부
	m_fRecognitionRange >= fTargetDist ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	//목표와 원거리 공격 범위 여부
	m_fShotRange >= fTargetDist ? m_bInShotRange = true : m_bInShotRange = false;
	//목표와 근거리 공격범위 여부
	m_fAtkRange >= fTargetDist ? m_bInAtkRange = true : m_bInAtkRange = false;

	//인지o
	if (true == m_bInRecognitionRange)
	{
		//원거리o
		if (true == m_bInShotRange)
		{
			//근거리o
			if (true == m_bInAtkRange)
			{
				//공격가능,쿨타임x
				if (true == m_tObjParam.bCanAttack)
				{
					m_bCanChooseAtkType = true;

					m_eFirstCategory = MONSTER_STATETYPE::ATTACK;

					m_iRandom = CALC::Random_Num(MONSTER_ATKTYPE::ATK_NORMAL, MONSTER_ATKTYPE::ATK_COMBO);

					m_iRandom = 0;

					switch (m_iRandom)
					{
					case MONSTER_ATKTYPE::ATK_NORMAL:
						m_eSecondCategory_ATK = MONSTER_ATKTYPE::ATK_NORMAL;
						m_eState = PET_POISIONBUTTERFLY_ANI::AtkPoisonShot;
						//Play_RandomAtkNormal();
						break;
					case MONSTER_ATKTYPE::ATK_COMBO:
						m_eSecondCategory_ATK = MONSTER_ATKTYPE::ATK_COMBO;
						Play_RandomAtkCombo();
						m_bIsCombo = true;
						break;
					}
				}
				//공격 불가, 쿨타임중
				else
				{
					m_bCanMoveAround = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_WALK;
				}
			}
		}
		else
		{
			m_bCanChase = true;
			m_eFirstCategory = MONSTER_STATETYPE::MOVE;
			m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_RUN;
		}
	}

	return;
}

void CPet_PoisonButterFly::Check_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_STATETYPE::IDLE:
		Play_Idle();
		break;
	case MONSTER_STATETYPE::MOVE:
		Play_Move();
		break;
	case MONSTER_STATETYPE::ATTACK:
		if (MONSTER_ATKTYPE::ATK_NORMAL == m_eSecondCategory_ATK)
		{
			switch (m_eState)
			{
			case PET_POISIONBUTTERFLY_ANI::AtkPoisonShot:
				Play_Shot();
				break;
			}
		}
		//else if (MONSTER_ATKTYPE::ATK_COMBO == m_eSecondCategory_ATK)
		//{
		//	switch (m_eAtkCombo)
		//	{
		//	}
		//}
		break;
	case MONSTER_STATETYPE::HIT:
		Play_Hit();
		break;
	case MONSTER_STATETYPE::CC:
		Play_CC();
		break;
	case MONSTER_STATETYPE::DEAD:
		Play_Dead();
		break;
	}
}

void CPet_PoisonButterFly::Check_DeadEffect(_double TimeDelta)
{
}

void CPet_PoisonButterFly::Play_RandomAtkNormal()
{
	//switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RIGHT, ATK_NORMAL_TYPE::NORMAL_WHEELWIND))
	//{
	//case ATK_NORMAL_TYPE::NORMAL_RIGHT:
	//	m_eState = PET_POISIONBUTTERFLY_ANI::Hammer_N01;
	//	break;
	//case ATK_NORMAL_TYPE::NORMAL_SHOULDER:
	//	m_eState = PET_POISIONBUTTERFLY_ANI::Hammer_S01;
	//	break;
	//case ATK_NORMAL_TYPE::NORMAL_TARGETHAMMERING:
	//	m_eState = PET_POISIONBUTTERFLY_ANI::Hammer_Sp01;
	//	break;
	//case ATK_NORMAL_TYPE::NORMAL_WHEELWIND:
	//	m_eState = PET_POISIONBUTTERFLY_ANI::Hammer_Sp02;
	//	break;
	//}

	return;
}

void CPet_PoisonButterFly::Play_RandomAtkCombo()
{
	//switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_R_L, ATK_COMBO_TYPE::COMBO_RUNHAMMERING))
	//{
	//case ATK_COMBO_TYPE::COMBO_R_L:
	//	m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_L;
	//	m_eState = Hammer_N01;
	//	break;
	//case ATK_COMBO_TYPE::COMBO_R_HAMMERING:
	//	m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_HAMMERING;
	//	m_eState = Hammer_N01;
	//	break;
	//case ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE:
	//	m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE;
	//	m_eState = Hammer_S01;
	//	break;
	//case ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK:
	//	m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK;
	//	m_eState = Hammer_S01;
	//	break;
	//case ATK_COMBO_TYPE::COMBO_RUNHAMMERING:
	//	m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RUNHAMMERING;
	//	m_eState = Hammer_Sp03_Start;
	//	break;
	//}

	return;
}

void CPet_PoisonButterFly::Play_Shot()
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
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 0.3f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_Head] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
		}
		else if (0.f <= AniTime)
			Function_RotateBody(m_pTarget);
	}

	return;
}

void CPet_PoisonButterFly::Play_Mist()
{
}

void CPet_PoisonButterFly::Play_Rush()
{
}

void CPet_PoisonButterFly::Play_Heal()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (nullptr != m_pPlayer)
	{

		if (true == m_tObjParam.bCanAttack)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;
			m_eState = PET_POISIONBUTTERFLY_ANI::Deformation;
		}
		else
		{
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				cout << m_tObjParam.fDamage << " 만큼 힐" << endl;
				m_fCoolDownMax = 2.f;
				m_bCanCoolDown = true;
				m_pPlayer->Add_Target_Hp(-m_tObjParam.fDamage);
				Function_ResetAfterAtk();

				return;
			}
		}

	}

	return;
}

void CPet_PoisonButterFly::Play_Idle()
{
	cout << "사회적 거리두기를 위한 아이들" << endl;
	m_eState = PET_POISIONBUTTERFLY_ANI::Idle;

	return;
}

void CPet_PoisonButterFly::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVETYPE::MOVE_RUN:
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
		case TARGET_BOSS:
			cout << "Boss를 향해 달려간다" << endl;
			Function_RotateBody(m_pTarget);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		case TARGET_MONSTER:
			cout << "Monster를 향해 달려간다" << endl;
			Function_RotateBody(m_pTarget);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		case TARGET_ITEM:
			cout << "Item을 향해 달려간다" << endl;
			Function_RotateBody(m_pTarget);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		case TARGET_NONE:
			cout << "player를 향해 달려간다" << endl;
			Function_RotateBody(m_pPlayer);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			break;
		}
		break;

	case MONSTER_MOVETYPE::MOVE_WALK:
		if (MONSTER_MOVETYPE::MOVE_WALK == m_eSecondCategory_MOVE)
		{
			cout << "player를 향해 걸어간다" << endl;
			m_eState = PET_POISIONBUTTERFLY_ANI::Walk_F;

			Function_RotateBody(m_pPlayer);
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (MONSTER_MOVETYPE::MOVE_ALERT == m_eSecondCategory_MOVE)
		{
			if (true == m_bCanMoveAround)
			{
				m_bCanMoveAround = false;
				m_bIsMoveAround = true;

				m_bCanCoolDown = true;
				m_fCoolDownMax = 4.f;

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
		}
		break;

	case MONSTER_MOVETYPE::MOVE_DODGE:
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
				m_eFirstCategory = MONSTER_STATETYPE::IDLE;
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

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
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

	m_eFirstCategory = MONSTER_STATETYPE::IDLE;
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

