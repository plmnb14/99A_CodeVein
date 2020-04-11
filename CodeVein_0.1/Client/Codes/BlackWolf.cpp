#include "stdafx.h"
#include "..\Headers\BlackWolf.h"

CBlackWolf::CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBlackWolf::CBlackWolf(const CBlackWolf & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackWolf::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CBlackWolf::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_BoneMatrix();
	Ready_Collider();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE);
	m_pTargetTransform = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE));

	m_eFirstCategory = MONSTER_ANITYPE::IDLE;

	m_tObjParam.fHp_Max = 75.f; //4~5대 사망, 기본공격력 20+-5에서 피감소
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fDamage = 20.f;

	m_tObjParam.bCanHit = true; //맞기 가능
	m_tObjParam.bIsHit = false;	//맞기 진행중 아님
	m_tObjParam.bCanAttack = true; //공격 가능
	m_tObjParam.bIsAttack = false; //공격 진행중 아님
	m_tObjParam.bDodge = false; //첫 생성시 false임

	m_bInRecognitionRange = false; //인지 범위 여부
	m_bInAtkRange = false; //공격 범위 여부
	m_bCanChase = false; //추격 여부
	m_bIsDodge = false; //회피 진행중 여부
	m_bCanCoolDown = false; //쿨타임 여부
	m_bIsCoolDown = false; //쿨타임 진행중 여부

	m_bCanIdleRandom = true;
	m_bCanAtkRandom = true;

	m_fRecognitionRange = 10.f; //인지범위
	m_fAtkRange = 4.f; //공격범위
	m_fCoolDownMax = 1.5f; //쿨타임 맥스값은 유동적
	m_fCoolDownCur = 0.f; //쿨타임 시간을 더함
	m_iRandom = 0; //랜덤 저장 변수
	m_iDodgeCount = 0; //n회 피격시 바로 회피

	m_fSpeedForCollisionPush = 2.f;

	return NOERROR;
}

_int CBlackWolf::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	Check_Hit();
	Check_Dist();
	Set_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_Collision();

	return NOERROR;
}

_int CBlackWolf::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NOERROR;
}

HRESULT CBlackWolf::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul); // * alpha

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

	return NOERROR;
}

HRESULT CBlackWolf::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

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

	return NOERROR;
}

void CBlackWolf::Update_Collider()
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

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return;
}

void CBlackWolf::Render_Collider()
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

void CBlackWolf::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));

	return;
}

void CBlackWolf::Check_CollisionPush()
{
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	for (auto& list_iter : tmpList)
	{
		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			// 지금 속도값 임의로 넣었는데 구해서 넣어줘야함 - 완료
			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSpeedForCollisionPush * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				// 방향 구해주고
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y축 이동은 하지말자
				vDir.y = 0;

				// 네비 메쉬타게 끔 세팅
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}

	return;
}

void CBlackWolf::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bIsAttack)
	{
		return;
	}

	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			if (false == vecIter->Get_Enabled())
				continue;

			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_Dodge())
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

void CBlackWolf::Check_Hit()
{
	//피가 없어 죽음 처리 해야함
	if (MONSTER_ANITYPE::DEAD == m_eFirstCategory)
	{
		return;
	}
	else //죽음 상태 아님
	{
		if (false == m_tObjParam.bCanHit) //피격o
		{
			if (true == m_tObjParam.bIsHit) //피격 진행중o
			{
				if (true == m_tObjParam.bHitAgain) //추가 피격o
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					Check_FBLR();
					m_tObjParam.bHitAgain = false;
					m_pMeshCom->Reset_OldIndx();

					return;
				}
				else //추가 피격x
				{
					return;
				}
			}
			else //피격 진행중x
			{
				if (0 >= m_tObjParam.fHp_Cur) //체력없음
				{
					//Down인 경우 Dead_Strong을 진행
					m_eFirstCategory = MONSTER_ANITYPE::DEAD;
					return;
				}
				else //체력 있음
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					Check_FBLR();
					return;
				}
			}
		}
	}

	return;
}

void CBlackWolf::Check_FBLR()
{
	//피격 앞,뒤 체크
	m_eSecondCategory_HIT = WOLF_HITTYPE::HIT_HIT_F;
	//m_eSecondCategory_HIT = WOLF_HITTYPE::HIT_HIT_B;

	return;
}

void CBlackWolf::Check_Dist()
{
	//아래 상태시 체크 안함
	if (MONSTER_ANITYPE::HIT == m_eFirstCategory ||
		MONSTER_ANITYPE::DOWN == m_eFirstCategory ||
		MONSTER_ANITYPE::DEAD == m_eFirstCategory)
		return;

	//회피중, 공격중, 맞는중 체크 안함
	if (true == m_bIsDodge ||
		true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsHit)
		return;

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAtkRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

	if (true == m_bInRecognitionRange) //인지o
	{
		//일상 애니 작동중o
		if (WOLF_ANI::Sit_End == m_eState || WOLF_ANI::Sit == m_eState || WOLF_ANI::Eat_End == m_eState || WOLF_ANI::Eat == m_eState)
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			return;
		}
		else //일상 애니 작동중x
		{
			if (true == m_bInAtkRange) //공격범위 o
			{
				if (true == m_tObjParam.bCanAttack) //공격 가능
				{
					if (true == m_bIsCoolDown) //쿨타임중o
					{
						//경계,응시
						m_eFirstCategory = MONSTER_ANITYPE::IDLE;
						m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_IDLE;
						Function_RotateBody();
						return;
					}
					else //쿨타임중x
					{
						m_bCanAtkRandom = true;
						m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
						Function_RotateBody();
						return;
					}
				}
				else //공격 불가
				{
					//몸을 회전하면서 경계한다
					m_eFirstCategory = MONSTER_ANITYPE::IDLE; //회피 대기 경계
					m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_IDLE;
					Function_RotateBody();
					return;
				}
			}
			else //공격범위x
			{
				m_bCanChase = true;
				m_eFirstCategory = MONSTER_ANITYPE::MOVE;
				m_eSecondCategory_MOVE = WOLF_MOVETYPE::MOVE_RUN;
				return;
			}
		}
	}
	else //인지x
	{
		//일상
		m_bCanChase = false;
		m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		if (true == m_bCanIdleRandom) //일상 랜덤
		{
			switch (CALC::Random_Num(WOLF_IDLETYPE::IDLE_IDLE, WOLF_IDLETYPE::IDLE_SIT))
			{
			case WOLF_IDLETYPE::IDLE_IDLE:
				m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_IDLE;
				break;
			case WOLF_IDLETYPE::IDLE_EAT:
				m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_EAT;
				break;
			case WOLF_IDLETYPE::IDLE_SIT:
				m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_SIT;
				break;
			}
		}

		return;
	}

	return;
}

void CBlackWolf::Set_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_ANITYPE::IDLE:
		if (true == m_bCanIdleRandom)
		{
			m_bCanIdleRandom = false;

			switch (m_eSecondCategory_IDLE)
			{
			case Client::CBlackWolf::IDLE_IDLE:
				m_eState = WOLF_ANI::Idle;
				break;
			case Client::CBlackWolf::IDLE_EAT:
				m_eState = WOLF_ANI::Eat;
				break;
			case Client::CBlackWolf::IDLE_SIT:
				m_eState = WOLF_ANI::Sit;
				break;
			}
		}
		else
		{
			switch (m_eSecondCategory_IDLE)
			{
			case Client::CBlackWolf::IDLE_IDLE:
				Play_Idle();
				break;
			case Client::CBlackWolf::IDLE_EAT:
				Play_Eat();
				break;
			case Client::CBlackWolf::IDLE_SIT:
				Play_Sit();
				break;
			}
		}
		break;

	case MONSTER_ANITYPE::MOVE:
		switch (m_eSecondCategory_MOVE)
		{
		case WOLF_MOVETYPE::MOVE_RUN:
			Play_Run();
			break;
		case WOLF_MOVETYPE::MOVE_WALK:
			Play_Walk();
			break;
		case WOLF_MOVETYPE::MOVE_DODGE:
			Play_Dodge();
			break;
		}
		break;

	case MONSTER_ANITYPE::ATTACK:
		if (true == m_bCanAtkRandom)
		{
			m_bCanAtkRandom = false; //랜덤 공격 불가
			m_tObjParam.bCanAttack = false; //공격불가
			m_tObjParam.bIsAttack = true; //공격중

			m_iRandom = CALC::Random_Num(BiteLRL, Frisbee);

			switch (m_iRandom)
			{
			case WOLF_ANI::BiteLRL:
				m_eState = WOLF_ANI::BiteLRL;
				break;
			case WOLF_ANI::RDodgeAtk:
				m_eState = WOLF_ANI::RDodgeAtk;
				break;
			case WOLF_ANI::LDodgeAtk:
				m_eState = WOLF_ANI::LDodgeAtk;
				break;
			case WOLF_ANI::Frisbee:
				m_eState = WOLF_ANI::Frisbee;
				break;
			}

			return;
		}
		else
		{
			switch (m_eState)
			{
			case Client::CBlackWolf::BiteLRL:
				Play_Bite_LRL();
				break;
			case Client::CBlackWolf::RDodgeAtk:
				Play_RDodgeAtk();
				break;
			case Client::CBlackWolf::LDodgeAtk:
				Play_LDodgeAtk();
				break;
			case Client::CBlackWolf::Frisbee:
				Play_Frisbee();
				break;
			}
		}
		break;

	case MONSTER_ANITYPE::HIT:
		Play_Hit();
		//if (WOLF_HITTYPE::HIT_HIT_F == m_eSecondCategory_IDLE)
		//{
		//	Play_Hit_F();
		//	break;
		//}
		//if (WOLF_HITTYPE::HIT_HIT_B == m_eSecondCategory_IDLE)
		//{
		//	Play_Hit_B();
		//	break;
		//}
		break;

	case MONSTER_ANITYPE::DOWN:
		break;

	case MONSTER_ANITYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CBlackWolf::Function_RotateBody()
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

void CBlackWolf::Function_CoolDown()
{
	if (true == m_bCanCoolDown) //공격 직후 상태
	{
		m_fCoolDownCur += DELTA_60;

		if (m_fCoolDownCur >= m_fCoolDownMax) //누적시간이 쿨타임보다 크다면
		{
			m_fCoolDownCur = 0.f;
			m_bCanCoolDown = false;
			m_bIsCoolDown = false;
			m_tObjParam.bCanAttack = true;
		}
	}

	return;
}

void CBlackWolf::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	// 네비 미적용
	//m_pTransformCom->Add_Pos(_fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), _vDir);

	// 네비 적용
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CBlackWolf::Function_DecreMoveMent(_float _fMutiply)
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

void CBlackWolf::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true; //피격 가능
	m_tObjParam.bIsHit = false; //피격중 x

	m_tObjParam.bDodge = false;  //회피아님
	m_bIsDodge = false; //회피중 아님

	m_tObjParam.bIsAttack = false; //공격중x
	m_bCanAtkRandom = true;//랜덤 공격 가능

	m_vecAttackCol[0]->Set_Enabled(false);

	LOOP(10)
		m_bEventTrigger[i] = false;

	return;
}

void CBlackWolf::Play_Idle()
{
	if (true == m_bInRecognitionRange) //인지o
	{
		if (true == m_tObjParam.bCanAttack)
		{
			//인지, 공격 가능->대기
			m_eState = WOLF_ANI::Idle;
			return;
		}
		else
		{
			//인지, 공격 불가->경계
			m_eState = WOLF_ANI::Threat;
			return;
		}
	}
	else //인지x
	{
		m_bCanIdleRandom = true;
		m_eState = WOLF_ANI::Idle;
		return;
	}

	return;
}

void CBlackWolf::Play_Eat()
{
	if (true == m_bInRecognitionRange)
	{
		if (WOLF_ANI::Eat == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = WOLF_ANI::Eat_End;

			return;
		}
		else if (WOLF_ANI::Eat_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.98f))
			{
				m_bCanIdleRandom = true;
				m_eState = WOLF_ANI::Idle;
			}

			return;
		}
	}
	else
	{
		m_eState = WOLF_ANI::Eat;

		return;
	}

	return;
}

void CBlackWolf::Play_Sit()
{
	if (true == m_bInRecognitionRange)
	{
		if (WOLF_ANI::Sit == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = WOLF_ANI::Sit_End;

			return;
		}
		else if (WOLF_ANI::Sit_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.98f))
			{
				m_bCanIdleRandom = true;
				m_eState = WOLF_ANI::Idle;
			}

			return;
		}
	}
	else
	{
		m_eState = WOLF_ANI::Sit;

		return;
	}

	return;
}

void CBlackWolf::Play_Walk()
{
	m_eState = WOLF_ANI::Walk;

	Function_RotateBody();

	Function_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CBlackWolf::Play_Run()
{
	m_eState = WOLF_ANI::Run;

	Function_RotateBody();

	Function_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CBlackWolf::Play_Dodge()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	//회피는 변수가 좀 이상하니 일단 제외
	if (false == m_tObjParam.bDodge) //회피아님?
	{
		m_eState = WOLF_ANI::Dodge;
		m_tObjParam.bDodge = true; //회피임
		m_bIsDodge = true; //회피중
	}
	else //회피임
	{
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			m_bIsDodge = false;
			m_tObjParam.bDodge = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (0.8f < AniTime && 2.f> AniTime)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 2.f;
			}

			Function_RotateBody(); //바라보면서 회피하고 회피하면서도 응시해야함

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));

			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CBlackWolf::Play_Bite_LRL()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack) //공격가능
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bCanCoolDown = true;
			m_fCoolDownMax = 2.f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.8f < AniTime && 2.2f> AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true); //공격콜라이더 작동
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.2f <= AniTime &&  2.8 >= AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (2.8f < AniTime && 3.8f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (3.8f <= AniTime && 4.4f >= AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
				Function_RotateBody();
			}
			else if (4.4f < AniTime && 4.9f > AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (4.9f <= AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}

	return;
}

void CBlackWolf::Play_RDodgeAtk()
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
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.5f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.2f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.6f < AniTime && 2.8f > AniTime) //1.6~2.8까치 충돌판정
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.8f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}
}

void CBlackWolf::Play_LDodgeAtk()
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
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.5f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.2f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (AniTime > 1.6f && AniTime < 2.8f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.8f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}

	return;
}

void CBlackWolf::Play_Frisbee()
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
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.3f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.5f < AniTime && 2.8f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.8f <= AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}

	return;
}

void CBlackWolf::Play_Hit()
{
	//최초 피격
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;
		m_eState = WOLF_ANI::Dmg_F;
		//if (WOLF_HITTYPE::HIT_HIT_F == m_eSecondCategory_HIT)
		//{
		//}
		//else
		//{
		//}
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_tObjParam.bCanHit = true;
			m_tObjParam.bIsHit = false;
			m_bCanCoolDown = true;

			m_fCoolDownMax = 0.5f;

			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit) //추가 피격인 경우
			{
				m_tObjParam.bCanHit = true;
				Check_FBLR();
			}
		}
	}

	return;
}

void CBlackWolf::Play_Down_Strong()
{
	return;
}

void CBlackWolf::Play_Down_Weak()
{
	return;
}

void CBlackWolf::Play_Dead()
{
	if (false == m_bCanDead)
	{
		m_bCanDead = true;
		m_bCanDissolve = true;
		m_eState = WOLF_ANI::Dead;
	}
	else
	{
		if (true == m_bCanDissolve && m_pMeshCom->Is_Finish_Animation(0.35f))
		{
			Start_Dissolve(0.5f, false, true);
			m_bCanDissolve = false;
		}

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bEnable = false;
			m_dAniPlayMul = 0;
		}
	}

	return;
}

void CBlackWolf::Play_Dead_Strong()
{
	m_eState = WOLF_ANI::Dead_Strong;

	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	{
		m_bIsDead = true;
		m_dAniPlayMul = 0.f;
	}

	return;
}

HRESULT CBlackWolf::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Wolf_Black", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return NOERROR;
}

HRESULT CBlackWolf::SetUp_ConstantTable()
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

	return NOERROR;
}

HRESULT CBlackWolf::Ready_Collider()
{
	m_vecPhysicCol.reserve(3); //2칸의 공간 할당
	m_vecAttackCol.reserve(2); //2칸의 공간 할당

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	_float fRadius = 1.2f;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	//공격 구 넣기
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.5f; //크기 확인

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CBlackWolf::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine1", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CBlackWolf* CBlackWolf::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlackWolf* pInstance = new CBlackWolf(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackWolf::Clone_GameObject(void * pArg)
{
	CBlackWolf* pInstance = new CBlackWolf(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackWolf::Free()
{
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (auto& vecter_iter : m_vecPhysicCol)
	{
		Safe_Release(vecter_iter);
	}

	for (auto& vecter_iter : m_vecAttackCol)
	{
		Safe_Release(vecter_iter);
	}

	for (auto& iter : m_matBone)
	{
		iter = nullptr;
	}

	CGameObject::Free();
}
