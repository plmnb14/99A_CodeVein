#include "stdafx.h"
#include "..\Headers\Monkey.h"
#include "..\Headers\MonkeyBullet.h"

CMonkey::CMonkey(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CMonkey::CMonkey(const CMonkey & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMonkey::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CMonkey::Ready_GameObject(void* pArg)
{
	if (FAILED(Add_Component()))
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

_int CMonkey::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	m_pMonsterUI->Update_GameObject(TimeDelta);

	Checkk_PosY();
	Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	MONSTER_STATETYPE::DEAD != m_eFirstCategory ? Enter_Collision() : Check_DeadEffect(TimeDelta);

	return NO_EVENT;
}

_int CMonkey::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CMonkey::Render_GameObject()
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

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Update_GameObject(m_dTimeDelta);

	Update_Collider();
	Render_Collider();

	return S_OK;
}

HRESULT CMonkey::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(0.f);

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_bool bMotionBlur = true;
	if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
		return E_FAIL;
	_bool bDecalTarget = false;
	if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
		return E_FAIL;

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

void CMonkey::Update_Collider()
{
	_ulong matrixIdx = Bone_LeftHand;

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

void CMonkey::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CMonkey::Enter_Collision()
{
	if (MONSTER_STATETYPE::DEAD != m_eFirstCategory)
	{
		Check_CollisionPush();
		Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	}

	return;
}

void CMonkey::Check_CollisionPush()
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

void CMonkey::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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
					{
						break;
					}
				}
			}
		}
	}

	return;
}

void CMonkey::Function_FBLR()
{
	_float angle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos()));

	if (MONSTER_STATETYPE::HIT == m_eFirstCategory)
	{
		m_eSecondCategory_HIT = MONSTER_HITTYPE::HIT_NORMAL;

		if (0.f <= angle && 90.f > angle)
			m_eFBLR = FBLR::FRONT;
		else if (-90.f <= angle && 0.f > angle)
			m_eFBLR = FBLR::FRONT;
		else if (90.f <= angle && 180.f > angle)
			m_eFBLR = FBLR::BACK;
		else if (-180.f <= angle && -90.f > angle)
			m_eFBLR = FBLR::BACK;
	}
	else if (MONSTER_STATETYPE::CC == m_eFirstCategory)
	{
		m_eSecondCategory_CC = MONSTER_CCTYPE::CC_DOWN;

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

void CMonkey::Function_RotateBody()
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

void CMonkey::Function_MoveAround()
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

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &m_pTransformCom->Get_Axis(AXIS_X), m_fSkillMoveSpeed_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CMonkey::Function_CoolDown()
{
	if (true == m_bCanCoolDown)
	{
		m_fCoolDownCur += DELTA_60;

		if (m_fCoolDownCur >= m_fCoolDownMax)
		{
			m_fCoolDownCur = 0.f;
			m_bCanCoolDown = false;
			m_bIsCoolDown = false;
			m_tObjParam.bCanAttack = true;
		}
	}

	return;
}

void CMonkey::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CMonkey::Function_DecreMoveMent(_float _fMutiply)
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

void CMonkey::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_bCanIdle = true;
	m_bIsIdle = false;

	m_tObjParam.bSuperArmor = false;
	m_tObjParam.bIsAttack = false;

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

void CMonkey::Checkk_PosY()
{
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return;
}

void CMonkey::Check_Hit()
{
	if (MONSTER_STATETYPE::DEAD == m_eFirstCategory)
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
					m_iDodgeCount = 0;
					m_tObjParam.bCanDodge = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_DODGE;
					m_pMeshCom->Reset_OldIndx();
					Function_RotateBody();
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
							Function_FBLR();
							m_tObjParam.bHitAgain = false;
							m_pMeshCom->Reset_OldIndx();
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

void CMonkey::Check_Dist()
{
	if (MONSTER_STATETYPE::HIT == m_eFirstCategory ||
		MONSTER_STATETYPE::CC == m_eFirstCategory ||
		MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_bIsCombo ||
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
						if (true == m_bIsCoolDown)
						{
							m_eFirstCategory = MONSTER_STATETYPE::IDLE;
							m_eSecondCategory_IDLE = MONSTER_IDLETYPE::IDLE_IDLE;
							Function_MoveAround();
						}
						else
						{
							m_bCanChooseAtkType = true;
							m_eFirstCategory = MONSTER_STATETYPE::ATTACK;
							Function_RotateBody();
						}
					}
					else
					{
						m_eFirstCategory = MONSTER_STATETYPE::MOVE;
						m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_WALK;
					}
				}
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = MONSTER_STATETYPE::MOVE;
					m_eSecondCategory_MOVE = MONSTER_MOVETYPE::MOVE_RUN;
					Function_RotateBody();
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

void CMonkey::Check_AniEvent()
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
				case MONKEY_ANI::Atk_N02:
					Play_Atk_RotBody();
					break;
				case MONKEY_ANI::Atk_N01:
					Play_RDiagonal();
					break;
				case MONKEY_ANI::Atk_Jump03:
					Play_JumpDown();
					break;
				case MONKEY_ANI::Atk_Jump02:
					Play_JumpLHand();
					break;
				case MONKEY_ANI::Atk_Jump01:
					Play_Jump_RotBody();
					break;
				case MONKEY_ANI::Atk_FangShoot:
					Play_FangShot();
					break;
				}
			}
			else if (MONSTER_ATKTYPE::ATK_COMBO == m_eSecondCategory_ATK)
			{
				switch (m_eAtkCombo)
				{
				case ATK_COMBO_TYPE::COMBO_NORMAL:
					Play_Combo_Normal();
					break;
				case ATK_COMBO_TYPE::COMBO_JUMP_CLOCK:
					Play_Combo_Jump_Clock();
					break;
				case ATK_COMBO_TYPE::COMBO_RUNATK:
					Play_Combo_RunAtk();
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

void CMonkey::Check_DeadEffect(_double TimeDelta)
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

void CMonkey::Play_RandomAtkNormal()
{
	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	if (m_fAtkRange < fLenth && m_fShotRange >= fLenth) 
		m_iRandom = ATK_NORMAL_TYPE::NORMAL_FANGSHOOT;
	else
		m_iRandom = CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_ATK_ROTBODY, ATK_NORMAL_TYPE::NORMAL_JUMP_ROTBODY);

	switch (m_iRandom)
	{
	case ATK_NORMAL_TYPE::NORMAL_ATK_ROTBODY:
		m_eState = MONKEY_ANI::Atk_N02;
		break;
	case ATK_NORMAL_TYPE::NORMAL_RDIGONAL:
		m_eState = MONKEY_ANI::Atk_N01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_JUMPDOWN:
		m_eState = MONKEY_ANI::Atk_Jump03;
		break;
	case ATK_NORMAL_TYPE::NORMAL_JUMPLHAND:
		m_eState = MONKEY_ANI::Atk_Jump02;
		break;
	case ATK_NORMAL_TYPE::NORMAL_JUMP_ROTBODY:
		m_eState = MONKEY_ANI::Atk_Jump01;
		break;
	case ATK_NORMAL_TYPE::NORMAL_FANGSHOOT:
		m_eState = MONKEY_ANI::Atk_FangShoot;
		break;
	}

	return;
}

void CMonkey::Play_RandomAtkCombo()
{
	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_JUMP_CLOCK, ATK_COMBO_TYPE::COMBO_RUNATK))
	{
	case ATK_COMBO_TYPE::COMBO_JUMP_CLOCK:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_JUMP_CLOCK;
		m_eState = MONKEY_ANI::Atk_Jump01;
		break;
	case ATK_COMBO_TYPE::COMBO_NORMAL:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_NORMAL;
		m_eState = MONKEY_ANI::Atk_N01;
		break;
	case ATK_COMBO_TYPE::COMBO_RUNATK:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RUNATK;
		m_eState = MONKEY_ANI::Atk_Sp_Start;
		break;
	}

	return;
}

void CMonkey::Play_FangShot()
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
			m_fCoolDownMax = 0.3f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.133f <= AniTime && 3.287 > AniTime)
		{
			matBone = *m_matBone[Bone_LeftHand] * m_pTransformCom->Get_WorldMat();
			memcpy(vBirth, &matBone._41, sizeof(_v3));
			//g_pManagement->Add_GameObject_ToLayer(L"Monster_MonkeyBullet", SCENE_STAGE, L"Layer_MonsterProjectile", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 15.f, 1.5));
			CObjectPool_Manager::Get_Instance()->Create_Object(L"Monster_MonkeyBullet", &BULLET_INFO(vBirth, m_pTransformCom->Get_Axis(AXIS_Z), 15.f, 1.5));
		}
		else if (0.f <= AniTime)
			Function_RotateBody();
	}

	return;
}

void CMonkey::Play_Jump_RotBody()
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
			m_fCoolDownMax = 1.4f;
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
		else if (1.833f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.967f < AniTime && 1.733f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CMonkey::Play_JumpLHand()
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
		else if (3.467f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (3.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.433f < AniTime && 3.233f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CMonkey::Play_JumpDown()
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

		if (1.533f < AniTime && 2.633f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
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

void CMonkey::Play_RDiagonal()
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
			m_fCoolDownMax = 0.9f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.933f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.633f <= AniTime)
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

		if (0.767f < AniTime && 1.133f > AniTime)
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

void CMonkey::Play_Atk_RotBody()
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
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.000f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}

		if (0.833f < AniTime && 1.300f > AniTime)
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

void CMonkey::Play_Combo_Normal()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (MONKEY_ANI::Atk_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.85f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = MONKEY_ANI::Atk_N02;

			return;
		}
		else if (1.933f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.633f <= AniTime)
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

		if (0.767f < AniTime && 1.133f > AniTime)
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
	else if (MONKEY_ANI::Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.000f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.833f < AniTime && 1.300f > AniTime)
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

	return;
}

void CMonkey::Play_Combo_Jump_Clock()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (MONKEY_ANI::Atk_Jump01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.85f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = MONKEY_ANI::Atk_N02;

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
		else if (1.833f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.967f < AniTime && 1.733f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 8.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 1.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (MONKEY_ANI::Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.000f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.833f < AniTime && 1.300f > AniTime)
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

	return;
}

void CMonkey::Play_Combo_RunAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (MONKEY_ANI::Atk_Sp_Start == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.8f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = MONKEY_ANI::Atk_Sp_Loop;

			return;
		}

		if (0.600f < AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 6.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (MONKEY_ANI::Atk_Sp_Loop == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_tObjParam.bSuperArmor = true;
			m_eState = MONKEY_ANI::Atk_Sp_End;

			return;
		}

		if (false == m_bEventTrigger[5])
		{
			m_bEventTrigger[5] = true;
			m_fSkillMoveSpeed_Cur = 6.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.0f;
		}

		Function_RotateBody();
		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(m_fSkillMoveMultiply);
	}
	else if (MONKEY_ANI::Atk_Sp_End == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.92f))
		{
			m_fCoolDownMax = 1.f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.167f < AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (0.867f < AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.467f < AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (0.267f < AniTime && 0.800f > AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.2f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CMonkey::Play_Idle()
{
	switch (m_eSecondCategory_IDLE)
	{
	case MONSTER_IDLETYPE::IDLE_IDLE:
		if (true == m_bInRecognitionRange)
		{
			m_bIsIdle = false;

			if (true == m_tObjParam.bCanAttack)
			{
				m_eState = MONKEY_ANI::Idle;
			}
			else
			{
				Function_RotateBody();
				m_eState = MONKEY_ANI::Idle;
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = MONKEY_ANI::Idle;
		}
		break;
	case MONSTER_IDLETYPE::IDLE_SIT:
		if (true == m_bInRecognitionRange)
		{
			if (MONKEY_ANI::NF_Sit == m_eState)
			{
				m_bIsIdle = true;

				if (m_pMeshCom->Is_Finish_Animation(0.5f))
					m_eState = MONKEY_ANI::NF_Sit_End;
			}
			else if (MONKEY_ANI::NF_Sit_End == m_eState)
			{
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_bCanIdle = true;
					m_bIsIdle = false;
					m_eState = MONKEY_ANI::Idle;
				}
			}
		}
		else
		{
			m_bIsIdle = true;
			m_eState = MONKEY_ANI::NF_Sit;
		}

		break;
	}

	return;
}

void CMonkey::Play_Move()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	switch (m_eSecondCategory_MOVE)
	{
	case MONSTER_MOVETYPE::MOVE_RUN:
		m_eState = MONKEY_ANI::Run;

		Function_RotateBody();

		Function_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

		Function_DecreMoveMent(0.1f);
		break;
	case MONSTER_MOVETYPE::MOVE_WALK:
		m_eState = MONKEY_ANI::Walk; 
		if (false == m_tObjParam.bCanAttack)
		{
			Function_MoveAround();
		}
		else
		{
			Function_RotateBody();

			Function_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));

			Function_DecreMoveMent(0.1f);
		}
		break;
	case MONSTER_MOVETYPE::MOVE_DODGE:
		if (true == m_tObjParam.bCanDodge)
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanDodge = false;
			m_tObjParam.bIsDodge = true;
			m_eState = MONKEY_ANI::Dodge;
		}
		else
		{
			switch (m_eState)
			{
			case MONKEY_ANI::Dodge:
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_tObjParam.bCanAttack = true;
					Function_ResetAfterAtk();

					return;
				}

				if (0.800f < AniTime && 1.433f > AniTime)
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
				break;
			case MONKEY_ANI::Dodge_L:
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_tObjParam.bCanAttack = true;
					Function_ResetAfterAtk();

					return;
				}

				if (0.800f < AniTime && 1.433f > AniTime)
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
				break;
			case MONKEY_ANI::Dodge_R:
				if (m_pMeshCom->Is_Finish_Animation(0.95f))
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					m_tObjParam.bCanAttack = true;
					Function_ResetAfterAtk();

					return;
				}

				if (0.800f < AniTime && 1.433f > AniTime)
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
				break;
			}
		}
		break;
	}

	return;
}

void CMonkey::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;

		switch (m_eFBLR)
		{
		case FBLR::FRONT:
			m_eState = MONKEY_ANI::Dmg_F;
			break;
		case FBLR::BACK:
			m_eState = MONKEY_ANI::Dmg_B;
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

void CMonkey::Play_CC()
{
	return;
}

void CMonkey::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();

		m_bCanPlayDead = true;

		if (MONKEY_ANI::Down_P_End == m_eState ||
			MONKEY_ANI::Down_P_Loop == m_eState ||
			MONKEY_ANI::Down_P_Start == m_eState)
			m_eState = MONKEY_ANI::Death_B;
		else if (MONKEY_ANI::Down_S_End == m_eState ||
				 MONKEY_ANI::Down_S_Loop == m_eState ||
				 MONKEY_ANI::Down_S_Start == m_eState)
			m_eState = MONKEY_ANI::Death_F;
		else
			m_eState = MONKEY_ANI::Death;
	}
	else
	{
		switch (m_eState)
		{
		case MONKEY_ANI::Death:
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

					Start_Dissolve(0.7f, false, true, 0.0f);
					m_pWeapon->Start_Dissolve(0.7f, false, true, 0.f);
					m_fDeadEffect_Delay = 0.f;
					CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.f));
				}
			}
			break;

		case MONKEY_ANI::Death_F:
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

					Start_Dissolve(0.7f, false, true, 0.0f);
					m_pWeapon->Start_Dissolve(0.7f, false, true, 0.f);
					m_fDeadEffect_Delay = 0.f;
					CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.f));
				}
			}
			break;

		case MONKEY_ANI::Death_B:
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

					Start_Dissolve(0.7f, false, true, 0.0f);
					m_pWeapon->Start_Dissolve(0.7f, false, true, 0.f);
					m_fDeadEffect_Delay = 0.f;
					CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.f));
				}
			}
			break;
		}
	}

	return;
}

HRESULT CMonkey::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Monkey", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CMonkey::SetUp_ConstantTable()
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

HRESULT CMonkey::Ready_Status(void * pArg)
{
	m_tObjParam.fDamage = -250.f;
	m_tObjParam.fHp_Max = 1200.f;
	m_tObjParam.fArmor_Max = 100.f;

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

HRESULT CMonkey::Ready_Weapon()
{
	m_pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon->Change_WeaponData(CWeapon::Wpn_SSword_Slave);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pWeapon->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CMonkey::Ready_Collider()
{
	m_vecPhysicCol.reserve(2);
	m_vecAttackCol.reserve(1);

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
	fRadius = 0.6f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftHand]->_41, m_matBone[Bone_LeftHand]->_42, m_matBone[Bone_LeftHand]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CMonkey::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand", 0), E_FAIL);
	m_matBone[Bone_LeftHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CMonkey* CMonkey::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonkey* pInstance = new CMonkey(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMonkey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonkey::Clone_GameObject(void * pArg)
{
	CMonkey* pInstance = new CMonkey(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMonkey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonkey::Free()
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
