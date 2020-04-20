#include "stdafx.h"
#include "..\Headers\Yeti.h"

CYeti::CYeti(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CYeti::CYeti(const CYeti & rhs)
	:CGameObject(rhs)
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

	Ready_BoneMatrix();
	Ready_Collider();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	m_pTarget->AddRef();
	m_pTargetTransform = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	m_pTargetTransform->AddRef();

	m_eFirstCategory = MONSTER_ANITYPE::IDLE;
	m_tObjParam.fHp_Max = 200.f; //4~5대 사망, 기본공격력 20+-5에서 피감소
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fDamage = 25.f;

	m_tObjParam.bCanHit = true; //맞기 가능
	m_tObjParam.bIsHit = false;	//맞기 진행중 아님
	m_tObjParam.bCanAttack = true; //공격 가능
	m_tObjParam.bIsAttack = false; //공격 진행중 아님
	m_tObjParam.bCanDodge = true; //회피 가능
	m_tObjParam.bIsDodge = false;  //회피 진행중 아님

	m_bInRecognitionRange = false; //인지 범위 여부
	m_bInAtkRange = false; //공격 범위 여부
	m_bCanChase = false; //추격 여부
	m_bCanCoolDown = false; //쿨타임 여부
	m_bIsCoolDown = false; //쿨타임 진행중 여부

	m_bAtkCategory = true; //공격타입 고정용
	m_bIsCombo = false; //콤보 진행중

	m_bCanIdle = true; //일상 가능
	m_bIsIdle = false; //일상 진행중 아님

	m_fRecognitionRange = 20.f; //인지범위
	m_fAtkRange = 5.f; //공격범위
	m_fCoolDownMax = 0.f; //쿨타임 맥스값은 유동적
	m_fCoolDownCur = 0.f; //쿨타임 시간을 더함
	m_fSpeedForCollisionPush = 2.f;
	m_iRandom = 0;

	return S_OK;
}

_int CYeti::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	Check_Hit();
	Check_Dist();
	Check_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_Collision();

	return NO_EVENT;
}

_int CYeti::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CYeti::Render_GameObject()
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
			if (MONSTER_ANITYPE::DEAD != m_eFirstCategory)
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

HRESULT CYeti::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

void CYeti::Update_Collider()
{
	_ulong matrixIdx = 0;

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

void CYeti::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CYeti::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
}

void CYeti::Check_CollisionPush()
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

			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSpeedForCollisionPush * DELTA_60))
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
}

void CYeti::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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
}

void CYeti::Function_RotateBody()
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
}

void CYeti::Function_CoolDown()
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

void CYeti::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CYeti::Function_DecreMoveMent(_float _fMutiply)
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

void CYeti::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_bCanIdle = true;
	m_bIsIdle = false;

	m_tObjParam.bIsAttack = false;

	m_bAtkCategory = true;
	m_bCanInterrupt = true;
	m_bIsCombo = false;

	for (auto& vetor_iter : m_vecAttackCol)
		vetor_iter->Set_Enabled(false);

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CYeti::Check_Hit()
{
	if (MONSTER_ANITYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (true == m_bCanInterrupt)
			{
				if (true == m_tObjParam.bIsHit)
				{
					if (true == m_tObjParam.bHitAgain)
					{
						m_eFirstCategory = MONSTER_ANITYPE::HIT;
						//이떄 특수 공격 관련으로 불값이 참인 경우 cc기로
						/*if(특수 공격)
						else
						데미지 측정 float 혹은 bool*/
						//	m_eFirstCategory = MONSTER_ANITYPE::CC;
						Check_FBLR();
						m_tObjParam.bHitAgain = false;
						m_pMeshCom->Reset_OldIndx();
					}
				}
				else
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					//데미지 측정, 특수 공격 측정
					/*if(특수 공격)
					else
					데미지 측정 float 혹은 bool*/
					//	m_eFirstCategory = MONSTER_ANITYPE::CC;
					Check_FBLR();
				}
			}
		}
	}
	else
		m_eFirstCategory = MONSTER_ANITYPE::DEAD;

}

void CYeti::Check_FBLR()
{
	_float angle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos()));

	if (MONSTER_ANITYPE::HIT == m_eFirstCategory)
	{
		m_eSecondCategory_HIT = YETI_HITTYPE::HIT_NORMAL;
		
		//if (-90.f <= angle && 90.f > angle)
		//	m_eFBLR = FBLR::FRONT;
		//else
		//	m_eFBLR = FBLR::BACK;
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

void CYeti::Check_Dist()
{
	if (MONSTER_ANITYPE::HIT == m_eFirstCategory ||
		MONSTER_ANITYPE::CC == m_eFirstCategory ||
		MONSTER_ANITYPE::DEAD == m_eFirstCategory)
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

		m_eFirstCategory = MONSTER_ANITYPE::IDLE;

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
				m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			}
			else
			{
				if (true == m_bInAtkRange)
				{
					if (true == m_tObjParam.bCanAttack)
					{
						if (true == m_bIsCoolDown)
						{
							m_eFirstCategory = MONSTER_ANITYPE::IDLE;
							m_eSecondCategory_IDLE = YETI_IDLETYPE::IDLE_IDLE;
							Function_RotateBody();
						}
						else
						{
							m_bAtkCategory = true;
							m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
							Function_RotateBody();
						}
					}
					else
					{
						m_eFirstCategory = MONSTER_ANITYPE::IDLE;
						m_eSecondCategory_IDLE = YETI_IDLETYPE::IDLE_IDLE;
						Function_RotateBody();
					}
				}
				else
				{
					m_bCanChase = true;
					m_eFirstCategory = MONSTER_ANITYPE::MOVE;
					m_eSecondCategory_MOVE = YETI_MOVETYPE::MOVE_RUN;
					Function_RotateBody();
				}
			}
		}
		else
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;

			if (false == m_bIsIdle)
				m_eSecondCategory_IDLE = YETI_IDLETYPE::IDLE_IDLE;
		}
	}

	return;
}

void CYeti::Check_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_ANITYPE::IDLE:
		Play_Idle();
		break;

	case MONSTER_ANITYPE::MOVE:
		Play_Move();
		break;

	case MONSTER_ANITYPE::ATTACK:
		if (true == m_bAtkCategory)
		{
			m_bAtkCategory = false;

			switch (CALC::Random_Num(YETI_ATKTYPE::ATK_NORMAL, YETI_ATKTYPE::ATK_COMBO))
			{
			case YETI_ATKTYPE::ATK_NORMAL:
				m_eSecondCategory_ATK = YETI_ATKTYPE::ATK_NORMAL;
				Play_RandomAtkNormal();
				break;
			case YETI_ATKTYPE::ATK_COMBO:
				m_eSecondCategory_ATK = YETI_ATKTYPE::ATK_COMBO;
				m_bIsCombo = true;
				Play_RandomAtkCombo();
				break;
			}
		}
		else
		{
			if (YETI_ATKTYPE::ATK_NORMAL == m_eSecondCategory_ATK)
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
			else if (YETI_ATKTYPE::ATK_COMBO == m_eSecondCategory_ATK)
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
		}
		break;

	case MONSTER_ANITYPE::HIT:
		Play_Hit();
		break;

	case MONSTER_ANITYPE::CC:
		Play_CC();
		break;

	case MONSTER_ANITYPE::DEAD:
		Play_Dead();
		break;
}

return;
}

void CYeti::Play_RandomAtkNormal()
{
	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RUSH, ATK_NORMAL_TYPE::NORMAL_BODYPRESS))
	{
	case ATK_NORMAL_TYPE::NORMAL_RUSH:
		m_eState = YETI_ANI::Atk_Rush;
		break;
	case ATK_NORMAL_TYPE::NORMAL_ROLLINGSLASH:
		m_eState = YETI_ANI::Atk_RollingSlash;
		break;
	case ATK_NORMAL_TYPE::NORMAL_ICETHROW:
		//m_eState = YETI_ANI::Atk_IceThrowing;
		//break;
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (4.200f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		if (3.133f < AniTime && 4.200f > AniTime)
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

void CYeti::Play_Howling()
{
	//미정 버프, 범위네 왜곡와 공격판정
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.533f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (3.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (1.400f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.800f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (4.633f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (3.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (3.267f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.133f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.5f))
			m_bCanInterrupt = false;

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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.300f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (2.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (1.633f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.567f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (0.900f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.400f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (2.300f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (0.867f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (3.333f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}

		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}

		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.5f))
			m_bCanInterrupt = false;

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
	//잠시 보류

	//_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	//if (true == m_tObjParam.bCanAttack)
	//{
	//	m_tObjParam.bCanAttack = false;
	//	m_tObjParam.bIsAttack = true;
	//}
	//else
	//{
	//	//1.500 땅에서 집어들고 잇음
	//	//4.400 투척
	//	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	//	{
	//		m_fCoolDownMax = 0.4f;
	//		m_bCanCoolDown = true;
	//		Function_ResetAfterAtk();
	//		return;
	//	}
	//	else if (4.567f <= AniTime)
	//	{
	//		if (false == m_bEventTrigger[0])
	//		{
	//			m_bEventTrigger[0] = true;
	//			m_vecAttackCol[0]->Set_Enabled(false);
	//		}
	//	}
	//	else if (4.200f <= AniTime)
	//	{
	//		if (false == m_bEventTrigger[1])
	//		{
	//			m_bEventTrigger[1] = true;
	//			m_vecAttackCol[0]->Set_Enabled(true);
	//		}
	//	}
	//	if (3.400f < AniTime && 4.133f > AniTime)
	//	{
	//		if (false == m_bEventTrigger[3])
	//		{
	//			m_bEventTrigger[3] = true;
	//			m_fSkillMoveSpeed_Cur = 6.f;
	//			m_fSkillMoveAccel_Cur = 0.f;
	//			m_fSkillMoveMultiply = 1.f;
	//		}
	//		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
	//		Function_DecreMoveMent(m_fSkillMoveMultiply);
	//	}
	//}

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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.267f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		else if (1.033f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (0.900f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		else if (0.600f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (0.403f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		else if (0.312f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (0.f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		if (1.500f < AniTime && 1.900f > AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
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
			m_fCoolDownMax = 0.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
			}
		}
		else if (0.533f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
			}
		}

		if (0.533f < AniTime && 1.300f > AniTime)
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

void CYeti::Play_Combo_RLRL_Shoulder()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YETI_ANI::Atk_Sp06 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_bCanInterrupt = true;
			m_vecAttackCol[1]->Set_Enabled(false);
			m_vecAttackCol[2]->Set_Enabled(false);
			m_eState = YETI_ANI::Atk_Shoulder;

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.5f))
			m_bCanInterrupt = false;

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
	else if (YETI_ANI::Atk_Shoulder == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.85f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (1.067f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (0.800f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_bCanInterrupt = false;

		if (0.400f < AniTime && 0.967f > AniTime)
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

void CYeti::Play_Combo_RLRL_Smash()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YETI_ANI::Atk_Sp06 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_bCanInterrupt = true;
			m_vecAttackCol[1]->Set_Enabled(false);
			m_vecAttackCol[2]->Set_Enabled(false);
			m_eState = YETI_ANI::Atk_Smash;

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_bCanInterrupt = false;

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
	else if (YETI_ANI::Atk_Smash == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.945f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (2.633f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_bCanInterrupt = false;

		if (1.033f < AniTime && 2.633f > AniTime)
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

void CYeti::Play_Combo_RLRL_Swing()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (YETI_ANI::Atk_Sp06 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_bCanInterrupt = true;
			m_vecAttackCol[1]->Set_Enabled(false);
			m_vecAttackCol[2]->Set_Enabled(false);
			m_eState = YETI_ANI::Atk_Swing;

			return;
		}
		else if (4.600f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (4.500f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (3.633f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[1]->Set_Enabled(false);
			}
		}
		else if (2.733f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_bCanInterrupt = false;

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
	else if (YETI_ANI::Atk_Swing == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.945f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_vecAttackCol[2]->Set_Enabled(false);
			}
		}
		else if (1.833f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_vecAttackCol[2]->Set_Enabled(true);
			}
		}

		if (m_pMeshCom->Is_Finish_Animation(0.3f))
			m_bCanInterrupt = false;

		if (1.267f < AniTime && 1.667f > AniTime)
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

void CYeti::Play_Idle()
{
	if (true == m_bInRecognitionRange)
	{
		m_bIsIdle = false;

		if (true == m_tObjParam.bCanAttack)
		{
			m_eState = YETI_ANI::NF_Threat_Loop;
		}
		else
		{
			Function_RotateBody();
			m_eState = YETI_ANI::NF_Threat_Loop;
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
	case YETI_MOVETYPE::MOVE_RUN:
		m_eState = YETI_ANI::Run;

		Function_RotateBody();
		Function_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(0.1f);

		break;
	case YETI_MOVETYPE::MOVE_WALK:
		m_eState = YETI_ANI::Walk_F;

		Function_RotateBody();
		Function_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(0.1f);

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
			m_eState = YETI_ANI::Dmg_F;
			break;
		case FBLR::BACK:
			m_eState = YETI_ANI::Dmg_B;
			break;
		}
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			Function_ResetAfterAtk();
			m_tObjParam.bCanHit = true;
			m_tObjParam.bIsHit = false;

			m_bCanCoolDown = true;
			m_fCoolDownMax = 0.5f;

			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				m_tObjParam.bCanHit = true;
				Check_FBLR();
			}
		}
	}

	return;
}

void CYeti::Play_CC()
{
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

		if (5.733f < AniTime && 6.467f > AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 1.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (1.567f < AniTime && 1.900f > AniTime)
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

		if (5.433f < AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				Start_Dissolve(0.7f, false, true);
			}

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

HRESULT CYeti::SetUp_ConstantTable()
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
	fRadius = 0.7f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);
	m_vecAttackCol.push_back(pCollider); //뭉개기용

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftHand]->_41, m_matBone[Bone_LeftHand]->_42, m_matBone[Bone_LeftHand]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightHand]->_41, m_matBone[Bone_RightHand]->_42, m_matBone[Bone_RightHand]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.6f;
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

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine2", 0), E_FAIL);
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand", 0), E_FAIL);
	m_matBone[Bone_LeftHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHand", 0), E_FAIL);
	m_matBone[Bone_RightHand] = &pFrame->CombinedTransformationMatrix;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftForeArm", 0), E_FAIL);
	m_matBone[Bone_Shoulder] = &pFrame->CombinedTransformationMatrix;

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
	Safe_Release(m_pTarget);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecPhysicCol)
		Safe_Release(iter);

	for (auto& iter : m_matBone)
		iter = nullptr;

	CGameObject::Free();

	return;
}
