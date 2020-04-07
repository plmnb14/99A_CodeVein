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

	m_tObjParam.fHp_Max = 100.f; //4~5대 사망, 기본공격력 20+-5에서 피감소
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;

	m_eFirstIdentify = MONSTER_ANITYPE::IDLE;

	m_tObjParam.bCanHit = true; //맞기 가능
	m_tObjParam.bIsHit = false;	//맞기 진행중 아님
	m_tObjParam.bCanAttack = true; //공격 가능
	m_tObjParam.bIsAttack = false; //공격 진행중 아님
	m_tObjParam.bDodge = false; //첫 생성시 회피 비활성

	return NOERROR;
}

_int CBlackWolf::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);
	
	if (m_bIsDead)
		return DEAD_OBJ;

	Check_Hit();
	Check_Dist();
	Set_AniEvent();
	Skill_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_CollisionEvent();

	return NOERROR;
}

_int CBlackWolf::Late_Update_GameObject(_double TimeDelta)
{
	CGameObject::Late_Update_GameObject(TimeDelta);

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

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
			m_pShaderCom->Begin_Pass(0);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

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

void CBlackWolf::Enter_CollisionEvent()
{
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));
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
			if(false == vecIter->Get_Enabled())
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

					//회피 중이 아니라면
					if (false == m_bIsDodge)
					{
						iter->Set_Target_CanHit(false);
						iter->Add_Target_Hp(m_tObjParam.fDamage);
					}

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
}

void CBlackWolf::Check_Hit()
{
	if (DEAD == m_eFirstIdentify)
		return;

	//맞고 있다
	if (true == m_tObjParam.bIsHit)
	{
		//200406 수정
		if (3 <= m_iDodgeCount)
		{
			m_iDodgeCount = 0;
			m_tObjParam.bIsHit = false; //3대 맞으면 강제로 회피 작동
			m_tObjParam.bDodge = true;
			return;
		}

		return;
	}

	//맞고 있지 않다
	else
	{
		//맞을 수 있는 상태
		if(false == m_tObjParam.bCanHit)
		{
			//체력 없음
			if (0 >= m_tObjParam.fHp_Cur)
			{
				m_eFirstIdentify = DEAD;
				return;
			}
			//체력있음
			else
			{
				m_eFirstIdentify = HIT;
				return;
			}
		}
	}
}

void CBlackWolf::Check_Dist()
{
	if (HIT == m_eFirstIdentify ||
		DOWN == m_eFirstIdentify ||
		DEAD == m_eFirstIdentify)
		return;

	if (true == m_bIsDodge)
	{
		return;
	}
	
	if (true == m_tObjParam.bIsAttack)
	{
		return;
	}

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAttackRange >= fLenth ? m_bInAttackRange = true : m_bInAttackRange = false;

	//인지 범위 IN
	if (true == m_bInRecognitionRange)
	{
		//일상 애니들 끊내기 위해서
		if (WOLF_ANI::Sit_End == m_eState ||
			WOLF_ANI::Sit == m_eState ||
			WOLF_ANI::Eat_End == m_eState||
			WOLF_ANI::Eat == m_eState)
		{
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
			return;
		}

		//공격 가능 범위 IN
		if (true == m_bInAttackRange)
		{
			if (true == m_tObjParam.bDodge)
			{
				m_eFirstIdentify = MONSTER_ANITYPE::MOVE;
				return;
			}
			else
			{
				if (true == m_tObjParam.bCanAttack)
				{
					if (true == m_bIsCoolDown)
					{
						m_eState = Threat;
						return;
					}
					else
					{
						m_eFirstIdentify = MONSTER_ANITYPE::ATTACK;
						m_iAttackRandomNumber = CALC::Random_Num(BiteLRL, Frisbee);
						return;
					}
				}
				else
				{
					m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
					return;
				}
			}
		}
		//공격 가능 범위 Out
		else
		{
			m_bCanChase = true; //추적
			m_eFirstIdentify = MONSTER_ANITYPE::MOVE;
			return;
		}
	}
	//인지 범위 Out
	else
	{
		m_bCanChase = false;
		m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
		return;
	}
}

void CBlackWolf::Set_AniEvent()
{
	switch (m_eFirstIdentify)
	{
	case MONSTER_ANITYPE::IDLE:
		Play_Idle();
		/*	if (true == m_bInRecognitionRange)
			{
				Play_Idle();
			}
			else if(false == m_bInRecognitionRange)
			{
				if (true == m_bIdleRandom)
				{
					m_iIdleRandomNumber = CALC::Random_Num(Idle, Eat);
					switch (m_iIdleRandomNumber)
					{
					case WOLF_ANI::Idle:
						Play_Idle();
						break;
					case WOLF_ANI::Eat:
					case WOLF_ANI::Eat_End:
						Play_Eat();
						break;
					case WOLF_ANI::Sit:
					case WOLF_ANI::Sit_End:
						Play_Sit();
						break;
					}
				}
				if (false == m_bIdleRandom)
				{
					Play_Idle();
				}
			}*/
		break;

	case MONSTER_ANITYPE::MOVE:
		if (true == m_bCanChase)
			Play_Run();
		if(true == m_tObjParam.bDodge) //200406수정
			Play_Dodge();
		break;

	case MONSTER_ANITYPE::ATTACK:
		Play_RandomAtk();
		break;

	case MONSTER_ANITYPE::HIT:
		Play_Hit();
		break;

	case MONSTER_ANITYPE::DOWN:
		break;

	case MONSTER_ANITYPE::DEAD:
		Play_Dead();
		break;
	}
}

void CBlackWolf::Skill_RotateBody()
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

void CBlackWolf::Skill_CoolDown()
{
	if (false == m_tObjParam.bIsAttack && false == m_tObjParam.bCanAttack)
	{
		m_fCoolDown += DELTA_60;
		m_bIsCoolDown = true;

		if (1.5f <= m_fCoolDown)
		{
			m_fCoolDown = 0.f;
			m_bIsCoolDown = false;
			m_tObjParam.bCanAttack = true;
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
		}
	}
}

void CBlackWolf::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	// 네비 없이
	m_pTransformCom->Add_Pos(_fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), _vDir);

	// 네비게이션 적용하면 
	//m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CBlackWolf::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CBlackWolf::Reset_BattleState()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;
	m_tObjParam.bDodge = false;
	m_tObjParam.bIsAttack = false;
	m_bIdleRandom = true;

	LOOP(10)
		m_bEventTrigger[i] = false;

	return;
}

void CBlackWolf::Play_Idle()
{
	m_eState = WOLF_ANI::Idle;

	//if (m_pMeshCom->Is_Finish_Animation())
	//{
	//	m_bIdleRandom = true;
	//}

	return;
}

void CBlackWolf::Play_Eat()
{
	if (true == m_bInRecognitionRange) //인지 함
	{
		if (WOLF_ANI::Eat == m_eState)
		{
			if(m_pMeshCom->Is_Finish_Animation())
				m_eState = WOLF_ANI::Eat_End;
			return;
		}
		if (WOLF_ANI::Eat_End == m_eState && m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eState = WOLF_ANI::Threat;
			return;
		}
	}
	else if(false == m_bInRecognitionRange) //인지 못함
	{
		if (WOLF_ANI::Eat != m_eState && WOLF_ANI::Eat_End != m_eState)
		{
			m_eState = WOLF_ANI::Eat;
			return;
		}

		else if (WOLF_ANI::Eat == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
			{
				m_eState = WOLF_ANI::Eat_End;
			}
			return;
		}
		else if (WOLF_ANI::Eat_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
			{
				m_bIdleRandom = true;
				m_eState = WOLF_ANI::Idle;
			}
			return;
		}
	}
}

void CBlackWolf::Play_Sit()
{
	if (true == m_bInRecognitionRange) //인지 함
	{
		if (WOLF_ANI::Sit == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = WOLF_ANI::Sit_End;
			return;
		}
		else if (WOLF_ANI::Sit_End == m_eState && m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eState = WOLF_ANI::Threat;
			return;
		}
	}
	else if (false == m_bInRecognitionRange) //인지 못함
	{
		if (WOLF_ANI::Sit != m_eState && WOLF_ANI::Sit_End != m_eState)
		{
			m_eState = WOLF_ANI::Sit;
			return;
		}
		else if (WOLF_ANI::Sit == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
			{
				m_eState = WOLF_ANI::Sit_End;
			}
			return;
		}
		else if (WOLF_ANI::Sit_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
			{
				m_bIdleRandom = true;
				m_eState = WOLF_ANI::Idle;
			}
			return;
		}
	}
}

void CBlackWolf::Play_RandomAtk()
{
	m_iAttackRandomNumber = CALC::Random_Num(BiteLRL, Frisbee); //27~30 4개중 1개

	switch (m_iAttackRandomNumber)
	{
	case WOLF_ANI::BiteLRL:
		Play_Bite_LRL();
		break;
	case WOLF_ANI::RDodgeAtk:
		Play_RDodgeAtk();
		break;
	case WOLF_ANI::LDodgeAtk:
		Play_LDodgeAtk();
		break;
	case WOLF_ANI::Frisbee:
		Play_Frisbee();
		break;
	}
}

void CBlackWolf::Play_Bite_LRL()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_eState = WOLF_ANI::BiteLRL;
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
			
			Reset_BattleState();

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
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (2.2f <= AniTime &&  2.8 >= AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

				Skill_RotateBody();

				return;
			}
			else if (AniTime > 2.8f && AniTime < 3.8f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true); //공격 콜라이더 작동
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (3.8f <= AniTime && 4.4f >= AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

				Skill_RotateBody();

				return;
			}
			else if (AniTime > 4.4f && AniTime < 4.9f)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (4.9f <= AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
				return;
			}
		}
	}
}

void CBlackWolf::Play_RDodgeAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_eState = WOLF_ANI::RDodgeAtk;
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
			m_tObjParam.bIsAttack = false;

			LOOP(10)
				m_bEventTrigger[i] = false;

			return;
		}
		else
		{
			if (AniTime < 1.2f) //0~1.2까지 이동 판정
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (AniTime > 1.6f && AniTime < 2.8f) //1.6~2.8까치 충돌판정
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true); //공격 콜라이더 작동
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (2.8f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
}

void CBlackWolf::Play_LDodgeAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_eState = WOLF_ANI::LDodgeAtk;
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
			m_tObjParam.bIsAttack = false;

			LOOP(10)
				m_bEventTrigger[i] = false;

			return;
		}
		else
		{
			if (AniTime < 1.2f) //0~1.2까지 이동 판정
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (AniTime > 1.6f && AniTime < 2.8f) //1.6~2.8까치 충돌판정
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true); //공격 콜라이더 작동
				}
				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (2.8f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
	//_double AniTime = m_pMeshCom->Get_TrackInfo().Position;
	//_float fTime = g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");
	//if (m_eAniPlayState != m_eState)
	//{
	//	m_tObjParam.bCanAttack = false;
	//	m_eAniPlayState = m_eState;
	//	m_bIsAttack = true;
	//}
	//else
	//{
	//	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	//	{
	//		cout << "좌 회피어택 끝" << endl;
	//		m_eState = Idle;
	//		m_eAniPlayState = m_eState;
	//		m_bIsAttack = true;
	//		LOOP(8) m_bEventTrigger[i] = false;
	//		return;
	//	}
	//	else if (AniTime < 1.2f)
	//	{
	//		if (m_bEventTrigger[1] == false)
	//		{
	//			m_bEventTrigger[1] = true;
	//			m_fSkillMoveSpeed_Cur = 4.f;
	//			m_fSkillMoveAccel_Cur = 0.f;
	//			m_fSkillMoveMultiply = 0.1f;
	//		}
	//		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
	//		Decre_Skill_Movement(m_fSkillMoveMultiply);
	//		return;
	//	}
	//	else if (AniTime > 1.6f && AniTime < 2.8f)
	//	{
	//		if (m_bEventTrigger[2] == false)
	//		{
	//			m_bEventTrigger[2] = true;
	//			m_fSkillMoveSpeed_Cur = 10.f;
	//			m_fSkillMoveAccel_Cur = 0.f;
	//			m_fSkillMoveMultiply = 0.1f;
	//		}
	//		Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
	//		Decre_Skill_Movement(m_fSkillMoveMultiply);
	//		return;
	//	}
	//}
}

void CBlackWolf::Play_Frisbee()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_eState = WOLF_ANI::Frisbee;
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
			m_tObjParam.bIsAttack = false;

			LOOP(10)
				m_bEventTrigger[i] = false;

			return;
		}
		else
		{
			if (AniTime > 1.5f && AniTime < 2.8f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true); //공격 콜라이더 작동
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
				return;
			}
			else if (2.8f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
}

void CBlackWolf::Play_Walk()
{
	//일반상태일 경우 단순하게 무작위 걷기
}

void CBlackWolf::Play_Run()
{
	m_eState = WOLF_ANI::Run;

	Skill_RotateBody();

	Skill_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Decre_Skill_Movement(0.1f);

	return;
}

void CBlackWolf::Play_Dodge()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bDodge)
	{
		m_eState = WOLF_ANI::Dodge;
		m_tObjParam.bDodge = false;
		m_bIsDodge = true;
		Reset_BattleState();
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
			m_bIsDodge = false;
			m_tObjParam.bDodge = true;
			LOOP(10)
				m_bEventTrigger[i] = false;

			return;
		}
		else if (0.8f < AniTime && 2.f> AniTime)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Skill_RotateBody();

			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));

			Decre_Skill_Movement(m_fSkillMoveMultiply);
			return;
		}
	}
}

void CBlackWolf::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		m_tObjParam.bIsHit = true;
		m_eState = WOLF_ANI::Dmg_F;
		Reset_BattleState();
	}
	else
	{
		//맞는 애니중, 95퍼 이상 재생, 행동 종료
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			++m_iDodgeCount; //200406 수정
			m_tObjParam.bIsHit = false;
			m_tObjParam.bCanHit = true;
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;

			return;
		}
		//맞는 애니중, 70퍼이상 애니 재생됬다면 또 처맞기 가능
		if (true == m_tObjParam.bIsHit && m_pMeshCom->Is_Finish_Animation(0.7f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				++m_iDodgeCount; //200406 수정
				m_tObjParam.bCanHit = true;

				return;
			}
		}
	}
}

void CBlackWolf::Play_Down_Strong()
{
	if (Down_Strong_Start == m_eState) m_pMeshCom->SetUp_Animation_Lower(Down_Strong_Start);
	if (Down_Strong_Start == m_eState && m_pMeshCom->Is_Finish_Animation_Lower(0.9f))
	{
		m_eState = Down_Strong_Loop;
		m_pMeshCom->SetUp_Animation_Lower(Down_Strong_Loop);
	}
	if (Down_Strong_Loop == m_eState && m_pMeshCom->Is_Finish_Animation_Lower(0.8f))
	{
		m_eState = Down_Strong_End;
		m_pMeshCom->SetUp_Animation_Lower(Down_Strong_End);
	}
	if (Down_Strong_End == m_eState && m_pMeshCom->Is_Finish_Animation_Lower(0.9f)) m_eState = Alert;
}

void CBlackWolf::Play_Down_Weak()
{
	if (Down_Weak_Start == m_eState) m_pMeshCom->SetUp_Animation_Lower(Down_Weak_Start);
	if (Down_Weak_Start== m_eState && m_pMeshCom->Is_Finish_Animation_Lower(0.9f))
	{
		m_eState = Down_Weak_Loop;
		m_pMeshCom->SetUp_Animation_Lower(Down_Weak_Loop);
	}
	if (Down_Weak_Loop == m_eState && m_pMeshCom->Is_Finish_Animation_Lower(0.8f))
	{
		m_eState = Down_weak_End;
		m_pMeshCom->SetUp_Animation_Lower(Down_weak_End);
	}
	if (Down_weak_End == m_eState && m_pMeshCom->Is_Finish_Animation_Lower(0.9f)) m_eState = Alert;
}

void CBlackWolf::Play_Dead()
{
	m_eState = WOLF_ANI::Dead;

	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	{
		//봐서 1.f인가 아니면 새로운 배수인가를 확인합시다
		m_bIsDead = true;
		m_dAniPlayMul = 0;
		//디졸브 작동
	}

}

void CBlackWolf::Play_Dead_Strong()
{
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

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CBlackWolf::Ready_Collider()
{
	m_vecPhysicCol.reserve(3); //3칸의 공간 할당
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

	fRadius = 0.3f;

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

	fRadius = 1.f; //크기 확인

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

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (auto& iter : m_vecPhysicCol)
	{
		Safe_Release(iter);
	}

	for (auto& iter : m_matBone)
	{
		iter = nullptr;
	}

	CGameObject::Free();
}
