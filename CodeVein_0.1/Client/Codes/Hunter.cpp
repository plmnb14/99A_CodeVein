#include "stdafx.h"
#include "..\Headers\Hunter.h"
#include "..\Headers\\HunterBullet.h"

CHunter::CHunter(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CHunter::CHunter(const CHunter & rhs)
	:CGameObject(rhs)
{
}

HRESULT CHunter::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CHunter::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_Status(pArg);
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Weapon();

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

_int CHunter::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	// MonsterHP UI
	m_pMonsterUI->Update_GameObject(TimeDelta);

	Check_PosY();
	Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_Collision();

	return NO_EVENT;
}

_int CHunter::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CHunter::Render_GameObject()
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

HRESULT CHunter::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

void CHunter::Update_Collider()
{
	_ulong matrixIdx = Bone_RightForeArm;

	for (auto& vector_iter : m_vecAttackCol)
	{
		//팔꿈치, 무릎, 발끝
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

void CHunter::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CHunter::Enter_Collision()
{
	if (MONSTER_STATETYPE::DEAD != m_eFirstCategory)
	{
		Check_CollisionPush();
		Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	}

	return;
}

void CHunter::Check_CollisionPush()
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

void CHunter::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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

void CHunter::Function_FBLR()
{
	_float angle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos()));

	if (MONSTER_STATETYPE::HIT == m_eFirstCategory)
	{
		m_eSecondCategory_HIT = MONSTER_HITTYPE::HIT_WEAK; //hit01

		if (0.f <= angle && 90.f > angle)
			m_eFBLR = FBLR::FRONTRIGHT;
		else if (90.f <= angle && 180.f > angle)
			m_eFBLR = FBLR::BACKRIGHT;
		else if (-180.f <= angle && -90.f > angle)
			m_eFBLR = FBLR::BACKLEFT;
		else if (-90.f <= angle && 0.f > angle)
			m_eFBLR = FBLR::FRONTLEFT;
	}
	else if (MONSTER_STATETYPE::CC == m_eFirstCategory)
	{
		//m_eSecondCategory_CC = HUNTER_CCTYPE::CC_STUN;
		m_eSecondCategory_CC = MONSTER_CCTYPE::CC_DOWN;
		//자빠짐Down_S, 엎어짐Down_P
		if (0.f <= angle && 90.f > angle)
			m_eFBLR = FBLR::FRONT;
		else if (-90.f <= angle && 0.f > angle)
			m_eFBLR = FBLR::FRONT;
		else if (90.f <= angle && 180.f > angle)
			m_eFBLR = FBLR::BACK;
		else if (-180.f <= angle && -90.f > angle)
			m_eFBLR = FBLR::BACK;
	}

	return;
}

void CHunter::Function_RotateBody()
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

void CHunter::Function_MoveAround(_float _fSpeed, _v3 _vDir)
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

void CHunter::Function_CoolDown()
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

void CHunter::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CHunter::Function_DecreMoveMent(_float _fMutiply)
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

void CHunter::Function_ResetAfterAtk()
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

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Set_Target_CanAttack(false);
	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Set_Enable_Trail(false);

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CHunter::Check_PosY()
{
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return;
}

void CHunter::Check_Hit()
{
	if (MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;
	//체력o
	if (0 < m_tObjParam.fHp_Cur)
	{
		//피격o
		if (false == m_tObjParam.bCanHit)
		{
			//콤보 방해가능
			if (false == m_tObjParam.bSuperArmor)
			{
				++m_iDodgeCount;
				//회피 수치 누적o
				if (m_iDodgeCount >= m_iDodgeCountMax)
				{
					m_iDodgeCount = 0;
					m_tObjParam.bCanDodge = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_DODGE;
					m_pMeshCom->Reset_OldIndx();
					Function_RotateBody();
				}
				//회피 수치 누적x
				else
				{
					//맞는 도중이라면
					if (true == m_tObjParam.bIsHit)
					{
						//연속 피격 가능
						if (true == m_tObjParam.bHitAgain)
						{
							m_eFirstCategory = MONSTER_STATETYPE::HIT;
							//이떄 특수 공격 관련으로 불값이 참인 경우 cc기로
							//if(특수 공격)
							//else
							//데미지 측정 float 혹은 bool
							//	m_eFirstCategory = MONSTER_STATETYPE::CC;
							Function_FBLR();
							m_tObjParam.bHitAgain = false;
							m_pMeshCom->Reset_OldIndx();
						}
					}
					//처음 맞음 또는 맞은지 오래됨
					else
					{
						m_eFirstCategory = MONSTER_STATETYPE::HIT;
						//데미지 측정, 특수 공격 측정
						//if(특수 공격)
						//else
						//데미지 측정 float 혹은 bool
						//	m_eFirstCategory = MONSTER_STATETYPE::CC;
							Function_FBLR();
					}
				}
			}
		}
	}
	//체력x
	else
		m_eFirstCategory = MONSTER_STATETYPE::DEAD;

	return;
}

void CHunter::Check_Dist()
{
	if (MONSTER_STATETYPE::HIT == m_eFirstCategory ||
		MONSTER_STATETYPE::CC == m_eFirstCategory ||
		MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_bIsCombo ||
		true == m_bIsMoveAround||
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

		//인지o
		if (true == m_bInRecognitionRange)
		{
			//일상 진행중
			if (true == m_bIsIdle)
			{
				m_eFirstCategory = MONSTER_STATETYPE::IDLE;
			}
			//일상 진행중 아님
			else
			{
				//범위o
				if (true == m_bInAtkRange)
				{
					//공격가능성o, 쿨타임 끝남
					if (true == m_tObjParam.bCanAttack)
					{
						m_bCanChooseAtkType = true;
						m_eFirstCategory = MONSTER_STATETYPE::ATTACK;
					}
					//공격가능성x, 쿨타임 진행중
					else
					{
						m_bCanMoveAround = true;
						m_eFirstCategory = MONSTER_STATETYPE::MOVE;
						m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_WALK;
					}
				}
				//범위x
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_RUN;
				}
			}
		}
		//인지x
		else
		{
			//타겟은 있으나 인지범위에 없음
			m_eFirstCategory = MONSTER_STATETYPE::IDLE;

			if (false == m_bIsIdle)
			{
				switch (CALC::Random_Num(MONSTER_IDLETYPE::IDLE_IDLE, MONSTER_IDLETYPE::IDLE_SIT))
				{
				case MONSTER_IDLETYPE::IDLE_IDLE:
					m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_IDLE;
					break;
				case MONSTER_IDLETYPE::IDLE_STAND:
					m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_STAND;
					break;
				case MONSTER_IDLETYPE::IDLE_CROUCH:
					m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_CROUCH;
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

void CHunter::Check_AniEvent()
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

			m_iRandom = CALC::Random_Num(MONSTER_ATKTYPE::ATK_NORMAL, MONSTER_ATKTYPE::ATK_COMBO);

			if (WEAPON_STATE::WEAPON_Hammer == m_eWeaponState)
				m_iRandom = 0;

			switch (m_iRandom)
			{
			case MONSTER_ATKTYPE::ATK_NORMAL:
				m_eSecondCategory_ATK = MONSTER_ATKTYPE::ATK_NORMAL;
				Play_RandomAtkNormal();
				break;
			case MONSTER_ATKTYPE::ATK_COMBO:
				m_eSecondCategory_ATK = MONSTER_ATKTYPE::ATK_COMBO;
				Play_RandomAtkCombo();
				m_bIsCombo = true;
				break;
			}

			return;
		}
		else
		{
			if (MONSTER_ATKTYPE::ATK_NORMAL == m_eSecondCategory_ATK)
			{
				switch (m_eState)
				{
				case HUNTER_ANI::SSword_Atk_Sp02:
					Play_SSword_CriticalDraw();
					break;
				case HUNTER_ANI::SSword_Atk_Sp01:
					Play_SSword_HelmetBreak();
					break;
				case HUNTER_ANI::SSword_Atk_S01:
					Play_SSword_Elbow();
					break;
				case HUNTER_ANI::SSword_Atk_N05:
					Play_SSword_WoodChop();
					break;
				case HUNTER_ANI::SSword_Atk_N04:
					Play_SSword_Upper_L();
					break;
				case HUNTER_ANI::SSword_Atk_N01:
					Play_SSword_Upper();
					break;
				case HUNTER_ANI::SSword_Atk_RaiseUp:
					Play_SSword_RaiseUp();
					break;
				case HUNTER_ANI::SSword_Atk_Jump:
					Play_SSword_Jump();
					break;
				case HUNTER_ANI::LSword_Atk_Smash:
					Play_LSword_Smash();
					break;
				case HUNTER_ANI::LSword_Atk_S01:
					Play_LSword_RDiagonal();
					break;
				case HUNTER_ANI::LSword_Atk_N01:
					Play_LSword_Right();
					break;
				case HUNTER_ANI::LSword_Atk_Kick:
					Play_LSword_KneeKick();
					break;
				case HUNTER_ANI::Hammer_Atk_TwoUpper:
					Play_Hammer_TwoUpper();
					break;
				case HUNTER_ANI::Hammer_Atk_Smash:
					Play_Hammer_Smash();
					break;
				case HUNTER_ANI::Hammer_Atk_Slash:
					Play_Hammer_Slash();
					break;
				case HUNTER_ANI::Hammer_Atk_N:
					Play_Hammer_Upper();
					break;
				case HUNTER_ANI::Halberd_Atk_TwoUpper:
					Play_Halberd_TwoUpper();
					break;
				case HUNTER_ANI::Halberd_Atk_TwoSlash:
					Play_Halberd_SlashForth();
					break;
				case HUNTER_ANI::Halberd_Atk_Sweep:
					Play_Halberd_Sweap();
					break;
				case HUNTER_ANI::Halberd_Atk_Sp03:
					Play_Halberd_Swing_Jump();
					break;
				case HUNTER_ANI::Halberd_Atk_Sp02:
					Play_Halberd_ClockTwice();
					break;
				case HUNTER_ANI::Halberd_Atk_S01:
					Play_Halberd_DeepPierce();
					break;
				case HUNTER_ANI::Halberd_Atk_N01:
					Play_Halberd_Pierce();
					break;
				case HUNTER_ANI::Halberd_Atk_RaiseUp:
					Play_Halberd_RiseUp();
					break;
				case HUNTER_ANI::Halberd_Atk_Force:
					Play_Halberd_StepPierce();
					break;
				case HUNTER_ANI::Bayonet_Atk_Snipe:
					Play_Gun_Snipe();
					break;
				case HUNTER_ANI::Bayonet_Atk_Shoot01:
					Play_Gun_Shoot();
					break;
				case HUNTER_ANI::Bayonet_Atk_N01:
					Play_Gun_R();
					break;
				case HUNTER_ANI::Bayonet_Atk_Kick:
					Play_Gun_Kick();
					break;
				default:
					break;
				}
			}
			else if (MONSTER_ATKTYPE::ATK_COMBO == m_eSecondCategory_ATK)
			{
				switch (m_eAtkCombo)
				{
				case ATK_COMBO_TYPE::COMBO_GUN_SHOT:
					Play_Gun_Combo_Shot();
					break;
				case ATK_COMBO_TYPE::COMBO_GUN_CQC:
					Play_Gun_Combo_CQC();
					break;
				case ATK_COMBO_TYPE::COMBO_HALBERD_THIRDATK:
					Play_Halberd_Combo_ThirdAtk();
					break;
				case ATK_COMBO_TYPE::COMBO_HALBERD_PIERCETWICE:
					Play_Halberd_Combo_PierceTwice();
					break;
				case ATK_COMBO_TYPE::COMBO_HALBERD_PIERCEWIND:
					Play_Halberd_Combo_PierceWind();
					break;
				case ATK_COMBO_TYPE::COMBO_LSWORD_NORMAL:
					Play_LSword_Combo_Normal();
					break;
				case ATK_COMBO_TYPE::COMBO_LSWORD_STRONG:
					Play_LSword_Combo_Strong();
					break;
				case ATK_COMBO_TYPE::COMBO_SWORD_STEPPIERCE:
					Play_SSword_Combo_StepPierce();
					break;
				case ATK_COMBO_TYPE::COMBO_SWORD_STRONG:
					Play_SSword_Combo_Strong();
					break;
				case ATK_COMBO_TYPE::COMBO_SWORD_Diagonal_L:
					Play_SSword_Combo_Diagonal_L();
					break;
				}
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

void CHunter::Play_RandomAtkNormal()
{		
	switch (m_eWeaponState)
	{
	case WEAPON_STATE::WEAPON_Gun:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_GUN_KICK, ATK_NORMAL_TYPE::NORMAL_GUN_SNIPE))
		{
		case ATK_NORMAL_TYPE::NORMAL_GUN_KICK:
			m_eState = HUNTER_ANI::Bayonet_Atk_Kick;
			break;
		case ATK_NORMAL_TYPE::NORMAL_GUN_R:
			m_eState = HUNTER_ANI::Bayonet_Atk_N01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_GUN_SHOOT:
			m_eState = HUNTER_ANI::Bayonet_Atk_Shoot01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_GUN_SNIPE:
			m_eState = HUNTER_ANI::Bayonet_Atk_Snipe;
			break;
		}
		break;
	case WEAPON_STATE::WEAPON_Halberd:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_HALBERD_STEPPIERCE, ATK_NORMAL_TYPE::NORMAL_HALBERD_TWOUPPER))
		{
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_STEPPIERCE:
			m_eState = HUNTER_ANI::Halberd_Atk_Force;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_RISEUP:
			m_eState = HUNTER_ANI::Halberd_Atk_RaiseUp;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_PIERCE:
			m_eState = HUNTER_ANI::Halberd_Atk_N01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_DEEPPIERCE:
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_CLOCKTWICE:
			m_eState = HUNTER_ANI::Halberd_Atk_Sp02;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_SWING_JUMP:
			m_eState = HUNTER_ANI::Halberd_Atk_Sp03;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_SWEAP:
			m_eState = HUNTER_ANI::Halberd_Atk_Sweep;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_SLASHFORTH:
			m_eState = HUNTER_ANI::Halberd_Atk_TwoSlash;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALBERD_TWOUPPER:
			m_eState = HUNTER_ANI::Halberd_Atk_TwoUpper;
			break;
		}
		break;
	case WEAPON_STATE::WEAPON_Hammer:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_HAMMER_UPPER, ATK_NORMAL_TYPE::NORMAL_HAMMER_TWOUPPER))
		{
		case ATK_NORMAL_TYPE::NORMAL_HAMMER_UPPER:
			m_eState = HUNTER_ANI::Hammer_Atk_N;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HAMMER_SMASH:
			m_eState = HUNTER_ANI::Hammer_Atk_Smash;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HAMMER_SLASH:
			m_eState = HUNTER_ANI::Hammer_Atk_Slash;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HAMMER_TWOUPPER:
			m_eState = HUNTER_ANI::Hammer_Atk_TwoUpper;
			break;
		}
		break;
	case WEAPON_STATE::WEAPON_LSword:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_LSWORD_KNEEKICK, ATK_NORMAL_TYPE::NORMAL_LSWORD_SMASH))
		{
		case ATK_NORMAL_TYPE::NORMAL_LSWORD_KNEEKICK:
			m_eState = HUNTER_ANI::LSword_Atk_Kick;
			break;
		case ATK_NORMAL_TYPE::NORMAL_LSWORD_RIGHT:
			m_eState = HUNTER_ANI::LSword_Atk_N01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_LSWORD_RDIAGONAL:
			m_eState = HUNTER_ANI::LSword_Atk_S01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_LSWORD_SMASH:
			m_eState = HUNTER_ANI::LSword_Atk_Smash;
			break;
		}
		break;
	case WEAPON_STATE::WEAPON_SSword:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_SWORD_JUMP, ATK_NORMAL_TYPE::NORMAL_SWORD_CRITICALDRAW))
		{
		case ATK_NORMAL_TYPE::NORMAL_SWORD_JUMP:
			m_eState = HUNTER_ANI::SSword_Atk_Jump;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_RAISEUP:
			m_eState = HUNTER_ANI::SSword_Atk_RaiseUp;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_UPPER:
			m_eState = HUNTER_ANI::SSword_Atk_N01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_Upper_L:
			m_eState = HUNTER_ANI::SSword_Atk_N04;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_WOODCHOP:
			m_eState = HUNTER_ANI::SSword_Atk_N05;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_ELBOW:
			m_eState = HUNTER_ANI::SSword_Atk_S01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_HELMETBREAK:
			m_eState = HUNTER_ANI::SSword_Atk_Sp01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SWORD_CRITICALDRAW:
			m_eState = HUNTER_ANI::SSword_Atk_Sp02;
			break;
		}
		break;
	}

	return;
}

void CHunter::Play_RandomAtkCombo()
{
	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	switch (m_eWeaponState)
	{
	case WEAPON_STATE::WEAPON_Gun:
		if (m_fAtkRange < fLenth &&  m_fShotRange > fLenth )
		{
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_GUN_CQC;
			m_eState = HUNTER_ANI::Bayonet_Atk_N01;
		}
		else
		{
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_GUN_SHOT;
			m_eState = HUNTER_ANI::Bayonet_Atk_Shoot01;
		}
		break;
	case WEAPON_STATE::WEAPON_Halberd:
		switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_HALBERD_THIRDATK, ATK_COMBO_TYPE::COMBO_HALBERD_PIERCEWIND))
		{
		case ATK_COMBO_TYPE::COMBO_HALBERD_THIRDATK:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_HALBERD_THIRDATK;
			m_eState = HUNTER_ANI::Halberd_Atk_N01;
			break;
		case ATK_COMBO_TYPE::COMBO_HALBERD_PIERCETWICE:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_HALBERD_PIERCETWICE;
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		case ATK_COMBO_TYPE::COMBO_HALBERD_PIERCEWIND:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_HALBERD_PIERCEWIND;
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		}
		break;
	case WEAPON_STATE::WEAPON_LSword:
		switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_LSWORD_NORMAL, ATK_COMBO_TYPE::COMBO_LSWORD_STRONG))
		{
		case ATK_COMBO_TYPE::COMBO_LSWORD_NORMAL:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_LSWORD_NORMAL;
			m_eState = HUNTER_ANI::Halberd_Atk_N01;
			break;
		case ATK_COMBO_TYPE::COMBO_LSWORD_STRONG:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_LSWORD_STRONG;
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		}
		break;
	case WEAPON_STATE::WEAPON_SSword:
		switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_SWORD_STEPPIERCE, ATK_COMBO_TYPE::COMBO_SWORD_Diagonal_L))
		{
		case ATK_COMBO_TYPE::COMBO_SWORD_STEPPIERCE:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SWORD_STEPPIERCE;
			m_eState = HUNTER_ANI::SSword_Atk_Sp03_Start;
			break;
		case ATK_COMBO_TYPE::COMBO_SWORD_STRONG:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SWORD_STRONG;
			m_eState = HUNTER_ANI::SSword_Atk_S01;
		case ATK_COMBO_TYPE::COMBO_SWORD_Diagonal_L:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SWORD_Diagonal_L;
			m_eState = HUNTER_ANI::SSword_Atk_N02;
			break;
		}
		break;
	}

	return;
}

void CHunter::Play_Gun_Kick()
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
			m_fCoolDownMax = 0.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.133f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.200f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.200f < AniTime && 2.133f > AniTime)
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

void CHunter::Play_Gun_R()
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
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.267f < AniTime && 1.767f > AniTime)
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

void CHunter::Play_Gun_Shoot()
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

				_mat matTemp = *m_matBone[Bone_RightHandAttach] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
		}
		else if (0.f <= AniTime)
			Function_RotateBody();
	}

	return;
}

void CHunter::Play_Gun_Snipe()
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
			m_fCoolDownMax = 0.5f;
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

				_mat matTemp = *m_matBone[Bone_RightHandAttach] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
		}
		else if (0.f <= AniTime)
			Function_RotateBody();
	}

	return;
}

void CHunter::Play_Gun_Combo_Shot()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	_v3 vBirth, vLook;
	_float fLength = 1.f;

	if (HUNTER_ANI::Bayonet_Atk_Shoot01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.7f))
		{
			m_tObjParam.bSuperArmor = false;
			m_eState = HUNTER_ANI::Bayonet_Atk_Shoot02;

			return;
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_RightHandAttach] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
		}
		else if (0.f <= AniTime)
			Function_RotateBody();
	}
	else if (HUNTER_ANI::Bayonet_Atk_Shoot02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.62f))
		{
			m_tObjParam.bSuperArmor = false;
			m_eState = HUNTER_ANI::Bayonet_Atk_Snipe;

			return;
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_RightHandAttach] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
		}
		else if (0.667f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_RightHandAttach] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
			Function_RotateBody();
		}
	}
	else if (HUNTER_ANI::Bayonet_Atk_Snipe == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.333f < AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_tObjParam.bSuperArmor = true;

				_mat matTemp = *m_matBone[Bone_RightHandAttach] * m_pTransformCom->Get_WorldMat(); //뼈위치* 월드

				memcpy(&vBirth, &matTemp._41, sizeof(_v3)); //생성위치
				memcpy(&vLook, &matTemp._21, sizeof(_v3)); //뼈의 룩
				vBirth += (vLook * fLength); //생성위치 = 생성위치 +(룩*길이)

				g_pManagement->Add_GameObject_ToLayer(L"Monster_HunterBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 8.f, 1.5));
			}
		}
		else if (0.f < AniTime)
			Function_RotateBody();
	}

	return;
}

void CHunter::Play_Gun_Combo_CQC()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Bayonet_Atk_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.438f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::Bayonet_Atk_N02;

			return;
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.267f < AniTime && 1.767f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Bayonet_Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.65f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::Bayonet_Atk_N03;

			return;
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.400f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.600f < AniTime && 3.627f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Bayonet_Atk_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.067f < AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.767f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.567f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.367f < AniTime && 1.867f >AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
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

void CHunter::Play_Halberd_StepPierce()
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
			m_fCoolDownMax = 0.7f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (5.067f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.767f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.067f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.100f < AniTime && 3.533f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Halberd_RiseUp()
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
		else if (3.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.300f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.767f < AniTime && 2.367f > AniTime)
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

void CHunter::Play_Halberd_Pierce()
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
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.333f < AniTime && 2.700f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.267f < AniTime && 1.733f > AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Halberd_DeepPierce()
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
		else if (3.867f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.133f < AniTime && 1.800f >AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Halberd_ClockTwice()
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
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.300f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.000f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.767f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (2.567f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.267f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.800f < AniTime && 4.300f > AniTime)
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
		else if (2.767f < AniTime && 3.700f > AniTime)
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
		else if (2.200f < AniTime && 2.667f > AniTime)
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
		else if (1.167f < AniTime && 2.133f > AniTime)
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
	}

	return;
}

void CHunter::Play_Halberd_Swing_Jump()
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
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (5.367f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (5.067f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.900f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (2.133f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.833f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.400f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.600f < AniTime && 4.067f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.667f < AniTime && 2.500f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);

		}
		else if (0.767f < AniTime && 1.200f > AniTime)
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
	}

	return;
}

void CHunter::Play_Halberd_Sweap()
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
			m_fCoolDownMax = 1.f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (6.500f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (6.200f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (6.067f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (6.000f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (5.700f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (5.500f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (5.167f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.867f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.667f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (3.833f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.533f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.400f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (6.367f < AniTime && 6.633f > AniTime)
		{
			if (false == m_bEventTrigger[18])
			{
				m_bEventTrigger[18] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (5.367f < AniTime && 5.767f > AniTime)
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
		else if (3.200f < AniTime && 3.667f > AniTime)
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
		else if (1.367f < AniTime && 1.767f > AniTime)
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
	}

	return;
}

void CHunter::Play_Halberd_SlashForth()
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
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (5.700f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (5.400f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (5.267f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (4.933f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.633f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.467f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (4.200f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.900f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.033f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (4.400f < AniTime && 5.500f > AniTime)
		{
			if (false == m_bEventTrigger[18])
			{
				m_bEventTrigger[18] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (3.867f < AniTime && 4.200f > AniTime)
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
		else if (2.933f < AniTime && 3.367f > AniTime)
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
		else if (1.867f < AniTime && 2.667f > AniTime)
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
	}

	return;
}

void CHunter::Play_Halberd_TwoUpper()
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
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.200f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.900f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.733f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (2.533f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.233f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.733f < AniTime && 4.567f > AniTime)
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
		else if (2.900f < AniTime && 3.633f > AniTime)
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
		else if (2.167f < AniTime && 2.800f > AniTime)
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
		else if (0.967f < AniTime && 2.000f > AniTime)
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
	}

	return;
}

void CHunter::Play_Halberd_Combo_ThirdAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Halberd_Atk_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.6f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::Halberd_Atk_N02;

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
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.333f < AniTime && 2.700f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (2.267f < AniTime && 1.733f > AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Halberd_Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.6f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::Halberd_Atk_N03;

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
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.833f < AniTime && 1.900f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Halberd_Atk_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.935f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.767f < AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.167f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.633f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.133f < AniTime && 2.433f >AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
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

void CHunter::Play_Halberd_Combo_PierceTwice()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Halberd_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.625f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::Halberd_Atk_S03;

			return;
		}
		else if (3.867f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.133f < AniTime && 1.800f >AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Halberd_Atk_S03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.93f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.433f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}

		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.333f < AniTime && 1.700f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Halberd_Combo_PierceWind()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Halberd_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.625f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::Halberd_Atk_S02;

			return;
		}
		else if (3.867f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.133f < AniTime && 1.800f >AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Halberd_Atk_S02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.945f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.167f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.667f < AniTime && 1.833f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
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

void CHunter::Play_Hammer_Upper()
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
		else if (2.467f <= AniTime)
		{ 
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.167f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.133f < AniTime && 1.800f > AniTime)
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

void CHunter::Play_Hammer_Slash()
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
		else if (2.667f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.367f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.233f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.200f < AniTime && 2.600f > AniTime)
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
		else if (1.067f < AniTime && 1.967f > AniTime)
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

void CHunter::Play_Hammer_Smash()
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
		else if (4.667f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.367f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.300f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.733f < AniTime && 4.400f > AniTime)
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
		else if (0.367f < AniTime && 1.167f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
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

void CHunter::Play_Hammer_TwoUpper()
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
		else if (3.433f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.133f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.033f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (2.067f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.767f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.000f < AniTime && 3.467f > AniTime)
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
		else if (0.933f < AniTime && 1.533f > AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
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

void CHunter::Play_LSword_KneeKick()
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
			m_fCoolDownMax = 0.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.133f < AniTime && 2.433f > AniTime)
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
		else if (0.567f < AniTime && 1.000f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 7.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_LSword_Right()
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
		else if (2.000f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.700f < AniTime && 1.233f > AniTime)
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

void CHunter::Play_LSword_RDiagonal()
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
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.533f < AniTime && 1.300f > AniTime)
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

void CHunter::Play_LSword_Smash()
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
		else if (3.133f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.833f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.067f < AniTime && 2.900f > AniTime)
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
		else if (1.133f < AniTime && 1.933f > AniTime)
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
		else if (0.533f < AniTime && 1.033f > AniTime)
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

void CHunter::Play_LSword_Combo_Normal()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::LSword_Atk_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.6f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::LSword_Atk_N02;

			return;
		}
		else if (2.000f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.700f < AniTime && 1.233f > AniTime)
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
	else if (HUNTER_ANI::LSword_Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.55f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::LSword_Atk_N03;

			return;
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.733f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.433f < AniTime && 1.333f > AniTime)
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
	}
	else if (HUNTER_ANI::LSword_Atk_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.935f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.200f < AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.900f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.733f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.633f < AniTime && 2.500f > AniTime)
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
		else if (0.533f < AniTime && 1.467f > AniTime)
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

void CHunter::Play_LSword_Combo_Strong()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::LSword_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.45f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::LSword_Atk_S02;

			return;
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.533f < AniTime && 1.300f > AniTime)
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
	else if (HUNTER_ANI::LSword_Atk_S02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.45f))
		{
			m_tObjParam.bSuperArmor = true;
			m_pWeapon->Set_Target_CanAttack(false);
			m_pWeapon->Set_Enable_Trail(false);
			m_eState = HUNTER_ANI::LSword_Atk_S03;

			return;
		}
		else if (2.200f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.900f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.767f < AniTime && 1.500f > AniTime)
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
	}
	else if (HUNTER_ANI::LSword_Atk_S03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.945f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.067f < AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.767f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.700f < AniTime && 2.367f > AniTime)
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
		else if (0.333f < AniTime && 1.200f > AniTime)
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

void CHunter::Play_SSword_Jump()
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
			m_fCoolDownMax = 1.f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.100f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.633f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.767f < AniTime && 2.767f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.300f < AniTime && 0.600f > AniTime)
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

void CHunter::Play_SSword_RaiseUp()
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
		else if (2.867f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.567f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.400f < AniTime && 1.800f > AniTime)
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

void CHunter::Play_SSword_Upper()
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
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.167f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.633f < AniTime && 1.000f > AniTime)
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

void CHunter::Play_SSword_Upper_L()
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
		else if (2.333f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.900f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.800f < AniTime && 1.300f > AniTime)
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

void CHunter::Play_SSword_WoodChop()
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
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.500f < AniTime && 1.867f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CHunter::Play_SSword_Elbow()
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
			m_fCoolDownMax = 0.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.333f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.667f < AniTime && 1.267f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 5.f;
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

void CHunter::Play_SSword_HelmetBreak()
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
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (4.467f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (4.333f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (4.167f < AniTime && 4.400f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CHunter::Play_SSword_CriticalDraw()
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
		else if (3.067f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (2.767f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.633f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.567f < AniTime && 1.100f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CHunter::Play_SSword_Combo_StepPierce()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::SSword_Atk_Sp03_Start == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.935f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = HUNTER_ANI::SSword_Atk_Sp03_Loop;

			return;
		}
		else if (4.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (3.967f < AniTime && 4.367f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 7.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::SSword_Atk_Sp03_Loop == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_eState = HUNTER_ANI::SSword_Atk_Sp03_End;

			return;
		}

		if (false == m_bEventTrigger[4])
		{
			m_bEventTrigger[4] = true;
			m_fSkillMoveSpeed_Cur = 20.f;
			m_fSkillMoveAccel_Cur = 2.f;
			m_fSkillMoveMultiply = 0.5f;
		}

		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(m_fSkillMoveMultiply);
	}
	else if (HUNTER_ANI::SSword_Atk_Sp03_End == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.915f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.733f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.421f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.211f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.103f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.904f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.857f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.665f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.526f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.381f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.233f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.f <= AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.267f > AniTime)
		{
			if (false == m_bEventTrigger[16])
			{
				m_bEventTrigger[16] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_SSword_Combo_Strong()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::SSword_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.400f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = HUNTER_ANI::SSword_Atk_S02;

			return;
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.333f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.667f < AniTime && 1.267f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 5.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::SSword_Atk_S02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.65f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = HUNTER_ANI::SSword_Atk_S03;

			return;
		}
		else if (3.900f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.600f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.533f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.900f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.500f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.733f < AniTime && 3.500f > AniTime)
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
		else if (0.533f < AniTime && 1.300f > AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::SSword_Atk_S03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.433f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.133f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (2.867f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
	}

	return;
}

void CHunter::Play_SSword_Combo_Diagonal_L()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::SSword_Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = HUNTER_ANI::SSword_Atk_N03;

			return;
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.367f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.900f < AniTime && 1.333f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.5f;
			}
			Function_RotateBody();
			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::SSword_Atk_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.915f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.400f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.000f < AniTime && 1.567f > AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
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

void CHunter::Play_Idle()
{
	switch (m_eSecondCategory_IDLE)
	{
	case MONSTER_IDLETYPE::IDLE_IDLE:
		if (true == m_bInRecognitionRange)
		{
			m_bIsIdle = false;

			if (true == m_tObjParam.bCanAttack)
			{
				//인지, 공격 가능->대기
				switch (m_eWeaponState)
				{
				case WEAPON_STATE::WEAPON_Gun:
					m_eState = HUNTER_ANI::Bayonet_Idle;
					break;
				case WEAPON_STATE::WEAPON_Halberd:
					m_eState = HUNTER_ANI::Halberd_Idle;
					break;
				case WEAPON_STATE::WEAPON_Hammer:
					m_eState = HUNTER_ANI::Hammer_Idle;
					break;
				case WEAPON_STATE::WEAPON_LSword:
					m_eState = HUNTER_ANI::LSword_Idle;
					break;
				case WEAPON_STATE::WEAPON_SSword:
					m_eState = HUNTER_ANI::Sword_Idle;
					break;
				}
			}
			else
			{
				//인지, 공격 불가->경계
				Function_RotateBody();
				switch (m_eWeaponState)
				{
				case WEAPON_STATE::WEAPON_Gun:
					m_eState = HUNTER_ANI::Bayonet_Idle;
					break;
				case WEAPON_STATE::WEAPON_Halberd:
					m_eState = HUNTER_ANI::Halberd_Idle;
					break;
				case WEAPON_STATE::WEAPON_Hammer:
					m_eState = HUNTER_ANI::Hammer_Idle;
					break;
				case WEAPON_STATE::WEAPON_LSword:
					m_eState = HUNTER_ANI::LSword_Idle;
					break;
				case WEAPON_STATE::WEAPON_SSword:
					m_eState = HUNTER_ANI::Sword_Idle;
					break;
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = HUNTER_ANI::Idle;
		}
		break;
	case MONSTER_IDLETYPE::IDLE_STAND:
		if (true == m_bInRecognitionRange)
		{
			if (HUNTER_ANI::Stand == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = HUNTER_ANI::Stand_End;
			}
			else if (HUNTER_ANI::Stand_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					switch (m_eWeaponState)
					{
					case WEAPON_STATE::WEAPON_Gun:
						m_eState = HUNTER_ANI::Bayonet_Idle;
						break;
					case WEAPON_STATE::WEAPON_Halberd:
						m_eState = HUNTER_ANI::Halberd_Idle;
						break;
					case WEAPON_STATE::WEAPON_Hammer:
						m_eState = HUNTER_ANI::Hammer_Idle;
						break;
					case WEAPON_STATE::WEAPON_LSword:
						m_eState = HUNTER_ANI::LSword_Idle;
						break;
					case WEAPON_STATE::WEAPON_SSword:
						m_eState = HUNTER_ANI::Sword_Idle;
						break;
					}
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = HUNTER_ANI::Stand;
		}
		break;
	case MONSTER_IDLETYPE::IDLE_CROUCH:
		if (true == m_bInRecognitionRange)
		{
			if (HUNTER_ANI::Crouch == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = HUNTER_ANI::Crouch_End;
			}
			else if (HUNTER_ANI::Crouch_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					switch (m_eWeaponState)
					{
					case WEAPON_STATE::WEAPON_Gun:
						m_eState = HUNTER_ANI::Bayonet_Idle;
						break;
					case WEAPON_STATE::WEAPON_Halberd:
						m_eState = HUNTER_ANI::Halberd_Idle;
						break;
					case WEAPON_STATE::WEAPON_Hammer:
						m_eState = HUNTER_ANI::Hammer_Idle;
						break;
					case WEAPON_STATE::WEAPON_LSword:
						m_eState = HUNTER_ANI::LSword_Idle;
						break;
					case WEAPON_STATE::WEAPON_SSword:
						m_eState = HUNTER_ANI::Sword_Idle;
						break;
					}
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = HUNTER_ANI::Crouch;
		}
		break;
	case MONSTER_IDLETYPE::IDLE_SIT:
		if (true == m_bInRecognitionRange)
		{
			if (HUNTER_ANI::Sit == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = HUNTER_ANI::Sit_End;
			}
			else if (HUNTER_ANI::Sit_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					switch (m_eWeaponState)
					{
					case WEAPON_STATE::WEAPON_Gun:
						m_eState = HUNTER_ANI::Bayonet_Idle;
						break;
					case WEAPON_STATE::WEAPON_Halberd:
						m_eState = HUNTER_ANI::Halberd_Idle;
						break;
					case WEAPON_STATE::WEAPON_Hammer:
						m_eState = HUNTER_ANI::Hammer_Idle;
						break;
					case WEAPON_STATE::WEAPON_LSword:
						m_eState = HUNTER_ANI::LSword_Idle;
						break;
					case WEAPON_STATE::WEAPON_SSword:
						m_eState = HUNTER_ANI::Sword_Idle;
						break;
					}
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = HUNTER_ANI::Sit;
		}
		break;
	}

	return;
}

void CHunter::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVETYPE::MOVE_RUN:
		if (true == m_bCanChase)
		{
			m_bCanChase = false;
			m_eState = HUNTER_ANI::Run;
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

			m_fSkillMoveSpeed_Cur = 3.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;

			switch (CALC::Random_Num(HUNTER_ANI::Walk_R, HUNTER_ANI::Walk_B))
			{
			case HUNTER_ANI::Walk_R:
			case HUNTER_ANI::Walk_FR:
			case HUNTER_ANI::Walk_F:
			case HUNTER_ANI::Walk_BR_RFoot:
			case HUNTER_ANI::Walk_B:
				m_eState = HUNTER_ANI::Walk_R;
				break;
			case HUNTER_ANI::Walk_L:
			case HUNTER_ANI::Walk_FL_L:
			case HUNTER_ANI::Walk_FL_R:
			case HUNTER_ANI::Walk_BR_LFoot:
			case HUNTER_ANI::Walk_BL:
				m_eState = HUNTER_ANI::Walk_L;
				break;
			}
		}
		else
		{
			if(HUNTER_ANI::Walk_R == m_eState)
				Function_MoveAround(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_X));
			else if(HUNTER_ANI::Walk_L == m_eState)
				Function_MoveAround(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_X));
		}
		break;
	case MONSTER_MOVETYPE::MOVE_DODGE:
		if (true == m_tObjParam.bCanDodge)
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanDodge = false;
			m_tObjParam.bIsDodge = true;
			m_eState = HUNTER_ANI::Dodge;
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

void CHunter::Play_Hit()
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
		case FBLR::FRONTLEFT:
			m_eState = HUNTER_ANI::Dmg01_FL;
			break;
		case FBLR::FRONTRIGHT:
			m_eState = HUNTER_ANI::Dmg01_FR;
			break;
		case FBLR::BACKLEFT:
			m_eState = HUNTER_ANI::Dmg01_BL;
			break;
		case FBLR::BACKRIGHT:
			m_eState = HUNTER_ANI::Dmg01_BR;
			break;
		}

	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			Function_ResetAfterAtk();
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

		Function_Movement(m_fSkillMoveSpeed_Cur, m_tObjParam.vHitDir);
		Function_DecreMoveMent(m_fSkillMoveMultiply);
	}

	return;
}

void CHunter::Play_CC()
{
	return;
}

void CHunter::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();

		m_bCanPlayDead = true;

		if (HUNTER_ANI::Down_S_Start == m_eState ||
			HUNTER_ANI::Down_S_Loop == m_eState ||
			HUNTER_ANI::Down_S_End == m_eState)
			m_eState = HUNTER_ANI::Death_B;
		else if (HUNTER_ANI::Down_P_Start == m_eState ||
				HUNTER_ANI::Down_P_Loop == m_eState ||
				HUNTER_ANI::Down_P_End == m_eState)
			m_eState = HUNTER_ANI::Death_F;
		else
			m_eState = HUNTER_ANI::Death;
	}
	else
	{
		switch (m_eState)
		{
		case HUNTER_ANI::Death_F:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			if (1.967f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.7f, false, true);
					m_pWeapon->Start_Dissolve(0.7f, false, true);
				}
			}
			break;

		case HUNTER_ANI::Death_B:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			if (4.400f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.7f, false, true);
					m_pWeapon->Start_Dissolve(0.7f, false, true);
				}
			}
			break;

		case HUNTER_ANI::Death:
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bEnable = false;
				m_dAniPlayMul = 0;
			}
			if (4.233f <= AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.7f, false, true);
					m_pWeapon->Start_Dissolve(0.7f, false, true);
				}
			}
			break;
		}
	}

	return;
}

HRESULT CHunter::Add_Component(void* pArg)
{
	_tchar MeshName[MAX_STR] = L"";

	MONSTER_STATUS eTemp = *(MONSTER_STATUS*)pArg;

	if (nullptr == pArg)
		lstrcpy(MeshName, L"Mesh_Hunter_Black");
	else
	{
		switch (eTemp.eMonsterColor)
		{
		case MONSTER_COLORTYPE::YELLOW:
			lstrcpy(MeshName, L"Mesh_Hunter_Black"); //황금 사냥꾼으로 바꿀예정
			break;
		case MONSTER_COLORTYPE::RED:
		case MONSTER_COLORTYPE::BLUE:
		case MONSTER_COLORTYPE::WHITE:
		case MONSTER_COLORTYPE::COLOR_NONE:
		case MONSTER_COLORTYPE::BLACK:
			lstrcpy(MeshName, L"Mesh_Hunter_Black");
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

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return S_OK;
}

HRESULT CHunter::SetUp_ConstantTable()
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

HRESULT CHunter::Ready_Status(void* pArg)
{
	if (nullptr == pArg)
	{
		m_tObjParam.fDamage = 500.f;
		m_tObjParam.fHp_Max = 2000.f;
		m_tObjParam.fArmor_Max = 10.f;

		m_fRecognitionRange = 15.f;
		m_fShotRange = 10.f;
		m_fAtkRange = 5.f;
		m_fPersonalRange = 2.f;
		m_iDodgeCountMax = 5;
		m_eWeaponState = WEAPON_STATE::WEAPON_SSword;
	}
	else
	{
		MONSTER_STATUS Info = *(MONSTER_STATUS*)pArg;
		m_eWeaponState = Info.eUseWhatWeapon;

		if (MONSTER_COLORTYPE::YELLOW == Info.eMonsterColor)
		{
			m_tObjParam.fDamage = 550.f;
			m_tObjParam.fHp_Max = 3000.f;
			m_tObjParam.fArmor_Max = 10.f;

			m_fRecognitionRange = 15.f;
			m_fShotRange = 10.f;
			m_fAtkRange = 5.f;
			m_fPersonalRange = 2.f;
			m_iDodgeCountMax = 5;
		}
		else
		{
			m_tObjParam.fDamage = 500.f;
			m_tObjParam.fHp_Max = 2000.f;
			m_tObjParam.fArmor_Max = 10.f;

			m_fRecognitionRange = 15.f;
			m_fShotRange = 10.f;
			m_fAtkRange = 5.f;
			m_fPersonalRange = 2.f;
			m_iDodgeCountMax = 5;
		}
	}

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

HRESULT CHunter::Ready_Weapon()
{
	m_pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));

	switch (m_eWeaponState)
	{
	case WEAPON_STATE::WEAPON_Gun:
		m_pWeapon->Change_WeaponData(CWeapon::Wpn_Gun_Black);
		break;
	case WEAPON_STATE::WEAPON_Halberd:
		m_pWeapon->Change_WeaponData(CWeapon::Wpn_Halberd_Black);
		break;
	case WEAPON_STATE::WEAPON_Hammer:
		m_pWeapon->Change_WeaponData(CWeapon::Wpn_Hammer_Black);
		break;
	case WEAPON_STATE::WEAPON_LSword:
		m_pWeapon->Change_WeaponData(CWeapon::Wpn_LSword_Black);
		break;
	case WEAPON_STATE::WEAPON_SSword:
		m_pWeapon->Change_WeaponData(CWeapon::Wpn_SSword_Black);
		break;
	}

	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pWeapon->Set_AttachBoneMartix(&pFrame->CombinedTransformationMatrix);
	m_pWeapon->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CHunter::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(3);

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

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightForeArm]->_41, m_matBone[Bone_RightForeArm]->_42, m_matBone[Bone_RightForeArm]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftKnee]->_41, m_matBone[Bone_LeftKnee]->_42, m_matBone[Bone_LeftKnee]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftToe]->_41, m_matBone[Bone_LeftToe]->_42, m_matBone[Bone_LeftToe]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CHunter::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;
	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightForeArm", 0), E_FAIL);
	m_matBone[Bone_RightForeArm] = &pFrame->CombinedTransformationMatrix; //팔꿈치

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftLeg", 0), E_FAIL);
	m_matBone[Bone_LeftKnee] = &pFrame->CombinedTransformationMatrix; //무릎

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftToeBase", 0), E_FAIL);
	m_matBone[Bone_LeftToe] = &pFrame->CombinedTransformationMatrix; // 발끝

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach", 0), E_FAIL);
	m_matBone[Bone_RightHandAttach] = &pFrame->CombinedTransformationMatrix; // 투사체 발사용 손뼈

	return S_OK;
}

CHunter* CHunter::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHunter* pInstance = new CHunter(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CHunter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHunter::Clone_GameObject(void * pArg)
{
	CHunter* pInstance = new CHunter(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CHunter");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHunter::Free()
{
	Safe_Release(m_pMonsterUI);

	Safe_Release(m_pTarget);
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pWeapon);
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}
