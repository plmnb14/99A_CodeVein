#include "stdafx.h"
#include "..\Headers\ThaiMan.h"

CThaiMan::CThaiMan(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CThaiMan::CThaiMan(const CThaiMan & rhs)
	:CGameObject(rhs)
{
}

HRESULT CThaiMan::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CThaiMan::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_Status(pArg);
	Ready_BoneMatrix();
	Ready_Collider();

	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBone[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

	if (nullptr != m_pTarget)
	{
		Safe_AddRef(m_pTarget);

		m_pTargetTransform = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
		Safe_AddRef(m_pTargetTransform);
	}

	return S_OK;
}

_int CThaiMan::Update_GameObject(_double TimeDelta)
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

	MONSTER_STATETYPE::DEAD != m_eFirstCategory ? Enter_Collision() : Check_DeadEffect(TimeDelta);

	return NO_EVENT;
}

_int CThaiMan::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NO_EVENT;
}

HRESULT CThaiMan::Render_GameObject()
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

	Update_Collider();
	Render_Collider();

	return S_OK;
}

HRESULT CThaiMan::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

void CThaiMan::Update_Collider()
{
	_ulong matrixIdx = Bone_LeftHand;

	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[matrixIdx] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(matTemp._41, matTemp._42, matTemp._43);

		vector_iter->Update(ColPos);

		++matrixIdx;
	}

	matrixIdx = Bone_Range;

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

void CThaiMan::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CThaiMan::Enter_Collision()
{
	if (MONSTER_STATETYPE::DEAD != m_eFirstCategory)
	{
		Check_CollisionPush();
		Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	}

	return;
}

void CThaiMan::Check_CollisionPush()
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

void CThaiMan::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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

void CThaiMan::Function_FBLR()
{
	_float angle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos()));

	if (MONSTER_STATETYPE::HIT == m_eFirstCategory)
	{
		m_eSecondCategory_HIT = MONSTER_HITTYPE::HIT_WEAK;

		if (0.f <= angle && 90.f > angle)
			m_eFBLR = FBLR::FRONTRIGHT;
		else if (-90.f <= angle && 0.f > angle)
			m_eFBLR = FBLR::FRONTLEFT;
		else if (90.f <= angle && 180.f > angle)
			m_eFBLR = FBLR::BACKRIGHT;
		else if (-180.f <= angle && -90.f > angle)
			m_eFBLR = FBLR::BACKLEFT;
	}

	return;
}

void CThaiMan::Function_RotateBody()
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos());

	_float fYAngle = m_pTransformCom->Get_Angle().y;

	_v3 vTargetDir = m_pTransformCom->Get_Axis(AXIS_Z);
	V3_NORMAL_SELF(&vTargetDir);

	if (fTargetAngle > 0)
	{
		if (fYAngle < 0)
		{
			if (-D3DXToRadian(90.f) > fYAngle && -D3DXToRadian(180.f) < fYAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= -D3DXToRadian(180.f)) fYAngle = D3DXToRadian(180.f);
			}
			else fYAngle += DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle < fTargetAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}
	else if (fTargetAngle < 0)
	{
		if (fYAngle > 0)
		{
			if (D3DXToRadian(90.f) < fYAngle && D3DXToRadian(180.f) > fYAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= D3DXToRadian(180.f)) fYAngle = -D3DXToRadian(180.f);
			}
			else fYAngle -= DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle > fTargetAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}

	m_pTransformCom->Set_Angle(AXIS_Y, fYAngle);

	return;
}

void CThaiMan::Function_MoveAround(_float _fSpeed, _v3 _vDir)
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos());

	_float fYAngle = m_pTransformCom->Get_Angle().y;

	_v3 vTargetDir = m_pTransformCom->Get_Axis(AXIS_Z);
	V3_NORMAL_SELF(&vTargetDir);

	if (fTargetAngle > 0)
	{
		if (fYAngle < 0)
		{
			if (-D3DXToRadian(90.f) > fYAngle && -D3DXToRadian(180.f) < fYAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= -D3DXToRadian(180.f)) fYAngle = D3DXToRadian(180.f);
			}
			else fYAngle += DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle < fTargetAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}
	else if (fTargetAngle < 0)
	{
		if (fYAngle > 0)
		{
			if (D3DXToRadian(90.f) < fYAngle && D3DXToRadian(180.f) > fYAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= D3DXToRadian(180.f)) fYAngle = -D3DXToRadian(180.f);
			}
			else fYAngle -= DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle > fTargetAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}

	m_pTransformCom->Set_Angle(AXIS_Y, fYAngle);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CThaiMan::Function_CoolDown()
{
	if (true == m_bCanCoolDown)
	{
		m_fCoolDownCur += DELTA_60;

		if (m_fCoolDownCur >= m_fCoolDownMax)
		{
			m_fCoolDownCur = 0.f;
			m_bCanCoolDown = false;
			m_bIsCoolDown = false;
			m_bIsMoveAround = false;
			m_tObjParam.bCanAttack = true;
		}
	}

	return;
}

void CThaiMan::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CThaiMan::Function_DecreMoveMent(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}

	return;
}

void CThaiMan::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_tObjParam.bSuperArmor = false;
	m_tObjParam.bIsAttack = false;

	m_bCanIdle = true;
	m_bIsIdle = false;

	m_bCanMoveAround = true;
	m_bIsMoveAround = false;

	m_bCanChooseAtkType = true;
	m_bIsCombo = false;

	for (auto& vetor_iter : m_vecAttackCol)
		vetor_iter->Set_Enabled(false);

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CThaiMan::Check_PosY()
{
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return;
}

void CThaiMan::Check_Hit()
{
	if (MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (true == m_tObjParam.bSuperArmor)
			{
				++m_iDodgeCount;
				if (m_iDodgeCount >= m_iDodgeCountMax)
				{
					m_iDodgeCount = 0;
					m_tObjParam.bCanDodge = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_DODGE;
					m_pMeshCom->Reset_OldIndx();
				}
				else
				{
					if (true == m_tObjParam.bIsHit)
					{
						if (true == m_tObjParam.bHitAgain)
						{
							m_eFirstCategory = MONSTER_STATETYPE::HIT;
							//이떄 특수 공격 관련으로 불값이 참인 경우 cc기로
							/*if(특수 공격)
							else
							데미지 측정 float 혹은 bool*/
							//	m_eFirstCategory = MONSTER_STATETYPE::CC;
							m_pMeshCom->Reset_OldIndx();
							m_tObjParam.bHitAgain = false;
							Function_FBLR();
						}
					}
					else
					{
						m_eFirstCategory = MONSTER_STATETYPE::HIT;
						//데미지 측정, 특수 공격 측정
						/*if(특수 공격)
						else
						데미지 측정 float 혹은 bool*/
						//	m_eFirstCategory = MONSTER_STATETYPE::CC;
						Function_FBLR();
					}
				}
			}
		}
	}
	else
		m_eFirstCategory = MONSTER_STATETYPE::DEAD;

	return;
}

void CThaiMan::Check_Dist()
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

	if (nullptr == m_pTargetTransform)
	{
		//유저를 잡았거나, 생성되지 않았거나
		//동료, 플레이어 레이어 찾기 또는 일상행동을 반복한다
		Function_ResetAfterAtk();

		m_eFirstCategory = MONSTER_STATETYPE::IDLE;

		return;
	}
	else
	{
		_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

		m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
		m_fAtkRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

		if (true == m_bInRecognitionRange)
		{
			if (true == m_bIsIdle)
			{
				m_eFirstCategory = MONSTER_STATETYPE::IDLE;
			}
			else
			{
				if (true == m_bInAtkRange)
				{
					if (true == m_tObjParam.bCanAttack)
					{
						m_bCanChooseAtkType = true;
						m_eFirstCategory = MONSTER_STATETYPE::ATTACK;
					}
					else
					{
						m_bCanMoveAround = true;
						m_eFirstCategory = MONSTER_STATETYPE::MOVE;
						m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_WALK;
					}
				}
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_RUN;
				}
			}
		}
		else
		{
			m_eFirstCategory = MONSTER_STATETYPE::IDLE;

			if (false == m_bIsIdle)
			{
				switch (CALC::Random_Num(MONSTER_IDLETYPE::IDLE_IDLE, MONSTER_IDLETYPE::IDLE_SIT))
				{
				case MONSTER_IDLETYPE::IDLE_IDLE:
					m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_IDLE;
					break;
				case MONSTER_IDLETYPE::IDLE_SIT:
					m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_SIT;
					break;
				}
			}
		}
	}

	return;
}

void CThaiMan::Check_AniEvent()
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
		if (true == m_bCanChooseAtkType)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;

			m_bCanChooseAtkType = false;

			switch (CALC::Random_Num(THAIMAN_ANI::Atk_SP03,THAIMAN_ANI::Atk_N01))
			{
			case THAIMAN_ANI::Atk_N01:
				m_eState = Atk_N01;
				break;
			case THAIMAN_ANI::Atk_N02:
				m_eState = Atk_N02;
				break;
			case THAIMAN_ANI::Atk_N03:
				m_eState = Atk_N03;
				break;
			case THAIMAN_ANI::Atk_N04:
				m_eState = Atk_N04;
				break;
			case THAIMAN_ANI::Atk_S01:
				m_eState = Atk_S01;
				break;
			case THAIMAN_ANI::Atk_S02:
				m_eState = Atk_S02;
				break;
			case THAIMAN_ANI::Atk_SP01:
				m_eState = Atk_SP01;
				break;
			case THAIMAN_ANI::Atk_SP02:
				m_eState = Atk_SP02;
				break;
			case THAIMAN_ANI::Atk_SP03:
				m_eState = Atk_SP03;
				break;
			}
		}
		else
		{
			switch (m_eState)
			{
			case THAIMAN_ANI::Atk_N01:
				Play_RDiagonal();
				break;
			case THAIMAN_ANI::Atk_N02:
				Play_L();
				break;
			case THAIMAN_ANI::Atk_N03:
				Play_BackDumpling();
				break;
			case THAIMAN_ANI::Atk_N04:
				Play_DropKick();
				break;
			case THAIMAN_ANI::Atk_S01:
				Play_LkBk();
				break;
			case THAIMAN_ANI::Atk_S02:
				Play_Capoeira();
				break;
			case THAIMAN_ANI::Atk_SP01:
				Play_RDigonalLUpperRAccel();
				break;
			case THAIMAN_ANI::Atk_SP02:
				Play_RkBkFk();
				break;
			case THAIMAN_ANI::Atk_SP03:
				Play_RDigonalRRScrewRchop();
				break;
			}
		}
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

	return;
}

void CThaiMan::Check_DeadEffect(_double TimeDelta)
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

void CThaiMan::Play_RandomAtkNormal()
{
	switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RDIAGONAL, ATK_NORMAL_TYPE::NORMAL_RDigonalRRScrewRchop))
	{
	case ATK_NORMAL_TYPE::NORMAL_RDIAGONAL:
		m_eState = THAIMAN_ANI::Atk_N01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_L:
		m_eState = THAIMAN_ANI::Atk_N02;
		break;
	case ATK_NORMAL_TYPE::NORMAL_BackDumpling:
		m_eState = THAIMAN_ANI::Atk_N03;
		break;
	case ATK_NORMAL_TYPE::NORMAL_DropKick:
		m_eState = THAIMAN_ANI::Atk_N04;
		break;
	case ATK_NORMAL_TYPE::NORMAL_LkBk:
		m_eState = THAIMAN_ANI::Atk_S01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_Capoeira:
		m_eState = THAIMAN_ANI::Atk_S02;
		break;
	case ATK_NORMAL_TYPE::NORMAL_RDigonalLUpperRAccel:
		m_eState = THAIMAN_ANI::Atk_SP01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_LkBkFk:
		m_eState = THAIMAN_ANI::Atk_SP02;
		break;
	case ATK_NORMAL_TYPE::NORMAL_RDigonalRRScrewRchop:
		m_eState = THAIMAN_ANI::Atk_SP03;
		break;
	}

	return;
}

void CThaiMan::Play_RDiagonal()
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.200f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.700f < AniTime && 2.067f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}
			
			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_L()
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.733f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.200f < AniTime && 1.533f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_BackDumpling()
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
			m_fCoolDownMax = 0.3f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[3]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[3]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.300f < AniTime && 1.933f > AniTime)
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

void CThaiMan::Play_DropKick()
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
			m_fCoolDownMax = 0.6f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.000f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[3]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[3]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.400f < AniTime && 3.000f > AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.700f < AniTime && 1.167f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CThaiMan::Play_LkBk()
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
			m_fCoolDownMax = 0.6f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.267f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[3]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[3]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.067f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.933f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.100f < AniTime && 2.267f > AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.600f < AniTime && 0.767f > AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_Capoeira()
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
			m_fCoolDownMax = 0.6f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[3]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.933f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[3]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.900f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.733f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.567f < AniTime && 0.967f > AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_RDigonalLUpperRAccel()
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
			m_fCoolDownMax = 0.6f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.867f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.700f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (3.267f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.100f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.567f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (4.100f < AniTime && 4.733f > AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.567f < AniTime && 3.067f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.500f < AniTime && 1.467f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_RkBkFk()
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
			m_fCoolDownMax = 0.6f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.633f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[3]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.667f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[3]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (2.600f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.367f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.200f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[3]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.033f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[3]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.667f < AniTime && 4.633f > AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}
			
			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.233f < AniTime && 2.433f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.667f < AniTime && 1.467f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_RDigonalRRScrewRchop()
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
			m_fCoolDownMax = 0.6f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.767f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.200f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (3.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.800f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.000f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.667f < AniTime && 4.633f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.233f < AniTime && 2.433f > AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.667f < AniTime && 1.467f > AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 2.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CThaiMan::Play_Idle()
{
	switch (m_eSecondCategory_IDLE)
	{
	case MONSTER_IDLETYPE::IDLE_IDLE:
		if (true == m_bInRecognitionRange)
		{
			m_bIsIdle = false;

			if (true == m_tObjParam.bCanAttack)
			{
				m_eState = THAIMAN_ANI::Idle;
			}
			else
			{
				Function_RotateBody();
				m_eState = THAIMAN_ANI::Idle;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = THAIMAN_ANI::Idle;
		}
		break;

	case MONSTER_IDLETYPE::IDLE_SIT:
		if (true == m_bInRecognitionRange)
		{
			if (THAIMAN_ANI::NF_Sit == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = THAIMAN_ANI::NF_Sit_End;
			}
			else if (THAIMAN_ANI::NF_Sit_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					m_eState = THAIMAN_ANI::Idle;
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = THAIMAN_ANI::NF_Sit;
		}
		break;

	case MONSTER_IDLETYPE::IDLE_SCRATCH:
		if (true == m_bInRecognitionRange)
		{
			if (THAIMAN_ANI::NF_Sit == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = THAIMAN_ANI::NF_Sit_End;
			}
			else if (THAIMAN_ANI::NF_Sit_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					m_eState = THAIMAN_ANI::Idle;
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = THAIMAN_ANI::NF_Sit;
		}
		break;

	case MONSTER_IDLETYPE::IDLE_CROUCH:
		if (true == m_bInRecognitionRange)
		{
			if (THAIMAN_ANI::NF_Sit == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = THAIMAN_ANI::NF_Sit_End;
			}
			else if (THAIMAN_ANI::NF_Sit_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					m_eState = THAIMAN_ANI::Idle;
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = THAIMAN_ANI::NF_Sit;
		}
		break;
	}

	return;
}

void CThaiMan::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVETYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = THAIMAN_ANI::Run;
			m_fSkillMoveSpeed_Cur = 4.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;
		}

		Function_RotateBody();
		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(m_fSkillMoveMultiply);
		break;
	case MONSTER_MOVETYPE::MOVE_WALK:
		if (true == m_bCanMoveAround)
		{
			m_bCanMoveAround = false;
			m_bIsMoveAround = true;

			m_bCanCoolDown = true;
			m_fCoolDownMax = 4.f;

			m_fSkillMoveSpeed_Cur = 2.5f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;

			switch (CALC::Random_Num(THAIMAN_ANI::Walk_R, THAIMAN_ANI::Walk_B))
			{
			case THAIMAN_ANI::Walk_R:
			case THAIMAN_ANI::Walk_B:
				m_eState = THAIMAN_ANI::Walk_R;
				break;
			case THAIMAN_ANI::Walk_F:
			case THAIMAN_ANI::Walk_L:
				m_eState = THAIMAN_ANI::Walk_L;
				break;
			}
		}
		else
		{
			if (THAIMAN_ANI::Walk_R == m_eState)
				Function_MoveAround(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
			else if (THAIMAN_ANI::Walk_L == m_eState)
				Function_MoveAround(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
		}
		break;
	case MONSTER_MOVETYPE::MOVE_DODGE:
		if (true == m_tObjParam.bCanDodge)
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanDodge = false;
			m_tObjParam.bIsDodge = true;
			m_eState = THAIMAN_ANI::Dodge;
		}
		else
		{
			switch (m_eState)
			{
			case THAIMAN_ANI::Dodge:
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_tObjParam.bCanAttack = true;
					Function_ResetAfterAtk();

					return;
				}
				else
				{
					if (1.100f < AniTime && 1.500f > AniTime)
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
			case THAIMAN_ANI::Dodge_L:
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_tObjParam.bCanAttack = true;
					Function_ResetAfterAtk();

					return;
				}
				else
				{
					if (0.800f < AniTime && 1.133f > AniTime)
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
			case THAIMAN_ANI::Dodge_R:
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_tObjParam.bCanAttack = true;
					Function_ResetAfterAtk();

					return;
				}
				else
				{
					if (0.867f < AniTime && 1.300f > AniTime)
					{
						if (m_bEventTrigger[0] == false)
						{
							m_bEventTrigger[0] = true;
							m_fSkillMoveSpeed_Cur = 10.f;
							m_fSkillMoveAccel_Cur = 0.f;
							m_fSkillMoveMultiply = 0.5f;
						}

						Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));

						Function_DecreMoveMent(m_fSkillMoveMultiply);
					}
				}
				break;
			}
		}
		break;
	}

	return;
}

void CThaiMan::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;

		switch (m_eFBLR)
		{
		case FBLR::FRONTLEFT:
			m_eState = THAIMAN_ANI::Dmg_W_FL;
			break;
		case FBLR::FRONTRIGHT:
			m_eState = THAIMAN_ANI::Dmg_W_FR;
			break;
		case FBLR::BACKLEFT:
			m_eState = THAIMAN_ANI::Dmg_W_BL;
			break;
		case FBLR::BACKRIGHT:
			m_eState = THAIMAN_ANI::Dmg_W_BR;
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

			m_eFirstCategory = MONSTER_STATETYPE::IDLE;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				m_tObjParam.bCanHit = true;
				Function_FBLR();
			}
		}
	}

	return;
}

void CThaiMan::Play_CC()
{
	return;
}

void CThaiMan::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();

		m_bCanPlayDead = true;

		if (THAIMAN_ANI::Down_P_End == m_eState ||
			THAIMAN_ANI::Down_P_Loop == m_eState ||
			THAIMAN_ANI::DmgStrike_P == m_eState ||
			THAIMAN_ANI::DmgBlow_B == m_eState)
			m_eState = THAIMAN_ANI::Death_B;
		else if (THAIMAN_ANI::Down_S_End == m_eState ||
				THAIMAN_ANI::Down_S_Loop == m_eState ||
				THAIMAN_ANI::DmgStrike_S == m_eState||
				THAIMAN_ANI::DmgBlow_F == m_eState)
			m_eState = THAIMAN_ANI::Death_F;
		else
			m_eState = THAIMAN_ANI::Death;
	}
	else
	{
		switch (m_eState)
		{
		case THAIMAN_ANI::Death:
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
				}
			}
			break;

		case THAIMAN_ANI::Death_F:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			if (3.167f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.7f, false, true);
				}
			}
			break;

		case THAIMAN_ANI::Death_B:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			if (2.867f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.7f, false, true);
				}
			}
			break;
		}
	}

	return;
}

HRESULT CThaiMan::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_ThaiMan", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CThaiMan::SetUp_ConstantTable()
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

HRESULT CThaiMan::Ready_Status(void * pArg)
{
	m_tObjParam.fDamage = -450.f;
	m_tObjParam.fHp_Max = 2400.f;
	m_tObjParam.fArmor_Max = 10.f;

	m_fRecognitionRange = 15.f;
	m_fShotRange = 10.f;
	m_fAtkRange = 5.f;
	m_fPersonalRange = 2.f;
	m_iDodgeCountMax = 5;

	m_eFirstCategory = MONSTER_STATETYPE::IDLE;
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

HRESULT CThaiMan::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(5);

	_float fRadius;
	CCollider* pCollider = nullptr;

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

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftHand]->_41, m_matBone[Bone_LeftHand]->_42, m_matBone[Bone_LeftHand]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightHand]->_41, m_matBone[Bone_RightHand]->_42, m_matBone[Bone_RightHand]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftToeBase]->_41, m_matBone[Bone_LeftToeBase]->_42, m_matBone[Bone_LeftToeBase]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightToeBase]->_41, m_matBone[Bone_RightToeBase]->_42, m_matBone[Bone_RightToeBase]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CThaiMan::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand", 0), E_FAIL);
	m_matBone[Bone_LeftHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHand", 0), E_FAIL);
	m_matBone[Bone_RightHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftToeBase", 0), E_FAIL);
	m_matBone[Bone_LeftToeBase] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightToeBase", 0), E_FAIL);
	m_matBone[Bone_RightToeBase] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CThaiMan* CThaiMan::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CThaiMan* pInstance = new CThaiMan(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CThaiMan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThaiMan::Clone_GameObject(void * pArg)
{
	CThaiMan* pInstance = new CThaiMan(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CThaiMan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThaiMan::Free()
{
	Safe_Release(m_pMonsterUI);

	Safe_Release(m_pTarget);
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}
