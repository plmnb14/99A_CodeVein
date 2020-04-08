#include "stdafx.h"
#include "..\Headers\YachaMan.h"

CYachaMan::CYachaMan(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CYachaMan::CYachaMan(const CYachaMan & rhs)
	: CGameObject(rhs)
{
}

HRESULT CYachaMan::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CYachaMan::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_BoneMatrix();
	Ready_Collider();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE);
	m_pTargetTransform = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE));

	m_tObjParam.fHp_Max = 230.f; //4~5대 사망, 기본공격력 20+-5에서 피감소
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;

	m_eFirstCategory = MONSTER_ANITYPE::IDLE;

	m_tObjParam.bCanHit = true; //맞기 가능
	m_tObjParam.bIsHit = false;	//맞기 진행중 아님
	m_tObjParam.bCanAttack = true; //공격 가능
	m_tObjParam.bIsAttack = false; //공격 진행중 아님
	m_tObjParam.bDodge = false; //첫 생성시 회피 비활성

	return NOERROR;
}

_int CYachaMan::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	cout << "심, 야차맨 체력 " << m_tObjParam.fHp_Cur << endl;

	Check_Hit();
	Check_Dist();
	Set_AniEvent();
	Skill_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_CollisionEvent();

	return NOERROR;
}

_int CYachaMan::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CYachaMan::Render_GameObject()
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
			m_pShaderCom->Begin_Pass(0);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
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

void CYachaMan::Update_Collider()
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

void CYachaMan::Render_Collider()
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

void CYachaMan::Enter_CollisionEvent()
{
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));
}

void CYachaMan::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bIsAttack) return;

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

					//회피 중이 아니라면 충돌 체크
					iter->Set_Target_CanHit(false);
					iter->Add_Target_Hp(m_tObjParam.fDamage);

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

void CYachaMan::Check_Hit()
{
	if (DEAD == m_eFirstCategory)
		return;

	if (true == m_tObjParam.bIsHit)
	{
		if (3 <= m_iDodgeCount)
		{
			m_iDodgeCount = 0;
			m_tObjParam.bIsHit = false; //3대 맞으면 강제로 회피 작동
			m_tObjParam.bDodge = true;
			return;
		}

		return;
	}
	else
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (0 >= m_tObjParam.fHp_Cur)
			{
				m_eFirstCategory = MONSTER_ANITYPE::DEAD;
				return;
			}
			else
			{
				m_eFirstCategory = MONSTER_ANITYPE::HIT;
				Check_FBLR();
				return;
			}
		}
	}
}

void CYachaMan::Check_FBLR()
{
	//추후 피격 위치에 따른 피격 모션을 다르게 하기 위한 함수
	m_eSecondCategory_HIT = YACHAMAN_HITTYPE::HIT_HIT;
	//m_eSecondCategory_Hit = YACHAMAN_HITTYPE::HIT_HIT_F;
	//m_eSecondCategory_Hit = YACHAMAN_HITTYPE::HIT_HIT_B;
}

void CYachaMan::Check_Dist()
{
	if (HIT == m_eFirstCategory ||
		DOWN == m_eFirstCategory ||
		DEAD == m_eFirstCategory)
		return;

	if (true == m_tObjParam.bIsAttack ||
		true == m_bIsAtkCombo)
		return;

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAttackRange >= fLenth ? m_bInAttackRange = true : m_bInAttackRange = false;

	if (true == m_bInRecognitionRange)
	{
		if (YACHAMAN_ANI::Lurk == m_eState ||
			YACHAMAN_ANI::Lurk_End == m_eState ||
			YACHAMAN_ANI::Eat == m_eState ||
			YACHAMAN_ANI::Eat_End == m_eState)
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			return;
		}

		if (true == m_bInAttackRange)
		{
			if (true == m_tObjParam.bCanAttack)
			{
				if (true == m_bIsCoolDown)
				{
					//인지,범위,공격가능,쿨타임 -> 대기패턴
					m_eFirstCategory = MONSTER_ANITYPE::IDLE;
					m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_IDLE;
					Skill_RotateBody();
					return;
				}
				else
				{
					//인지,범위,공격가능,쿨타임아님->공격 패턴
					m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
					Skill_RotateBody();
					return;
				}
			}
			else
			{
				//인지, 범위, 공격불가능 -> 공격중은 아닌데? walk하면서 주위 맴돌기,경계상태에 가깝다
				m_eFirstCategory = MONSTER_ANITYPE::IDLE;
				m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_IDLE;
				return;
			}
		}
		else
		{			
			//인지o, 공격범위x->추격
			m_bCanChase = true;
			m_eFirstCategory = MONSTER_ANITYPE::MOVE;
			m_eSecondCategory_MOVE = YACHAMAN_MOVETYPE::MOVE_RUN;
			return;
		}
	}
	else
	{
		//인지x -> 일상행동
		m_bCanChase = false;
		m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		if (true == m_bCanRandomIdle)
		{
			m_bCanRandomIdle = false;

			switch (CALC::Random_Num(YACHAMAN_IDLETYPE::IDLE_IDLE, YACHAMAN_IDLETYPE::IDLE_LURK))
			{
			case YACHAMAN_IDLETYPE::IDLE_IDLE:
				m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_IDLE;
				break;
			case YACHAMAN_IDLETYPE::IDLE_EAT:
				m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_EAT;
				break;
			case YACHAMAN_IDLETYPE::IDLE_LURK:
				m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_LURK;
				break;
			}
		}
		return;
	}
}

void CYachaMan::Set_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_ANITYPE::IDLE:
		switch (m_eSecondCategory_IDLE)
		{
		case YACHAMAN_IDLETYPE::IDLE_IDLE:
			Play_Idle();
			break;
		case YACHAMAN_IDLETYPE::IDLE_EAT:
			Play_Eat();
			break;
		case YACHAMAN_IDLETYPE::IDLE_LURK:
			Play_Lurk();
			break;
		}
		break;
	case MONSTER_ANITYPE::MOVE:
		switch (m_eSecondCategory_MOVE)
		{
		case YACHAMAN_MOVETYPE::MOVE_RUN:
			Play_Run();
			break;
		case YACHAMAN_MOVETYPE::MOVE_WALK:
			Play_Walk();
			break;
		case YACHAMAN_MOVETYPE::MOVE_DODGE:
			Play_Dodge();
			break;
		}
		break;
	case MONSTER_ANITYPE::ATTACK:
		if (true == m_bCanRandomAtkCategory)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;
			m_bCanRandomAtkCategory = false;
			m_bIsAtkCombo = true;

			m_iAtkRandomType = CALC::Random_Num(YACHAMAN_ATKTYPE::ATK_NORMAL, YACHAMAN_ATKTYPE::ATK_COMBO);

			switch (m_iAtkRandomType)
			{
			case YACHAMAN_ATKTYPE::ATK_NORMAL:
				m_eSecondCategory_ATK = YACHAMAN_ATKTYPE::ATK_NORMAL;
				cout << "심, 랜덤 노말 공격" << endl;
				Play_RandomAtkNormal();
				break;
			case YACHAMAN_ATKTYPE::ATK_COMBO:
				m_eSecondCategory_ATK = YACHAMAN_ATKTYPE::ATK_COMBO;
				cout << "심, 랜덤 콤보 공격" << endl;
				Play_RandomAtkCombo();
				break;
			}

			return;
		}
		else
		{
			if (YACHAMAN_ATKTYPE::ATK_NORMAL == m_eSecondCategory_ATK)
			{
				switch (m_eState)
				{
				case YACHAMAN_ANI::Atk_Ani_R:
					Play_R();
					break;
				case YACHAMAN_ANI::Atk_Ani_L:
					Play_L();
					break;
				case YACHAMAN_ANI::Atk_Ani_Hammering:
					play_Hammering();
					break;
				case YACHAMAN_ANI::Atk_Ani_Shoulder:
					Play_Shoulder();
					break;
				case YACHAMAN_ANI::Atk_Ani_HalfClock:
					Play_HalfClock();
					break;
				case YACHAMAN_ANI::Atk_Ani_TurnTwice:
					Play_TurnTwice();
					break;
				case YACHAMAN_ANI::Atk_Ani_TargetHammering:
					Play_TargetHammering();
					break;
				case YACHAMAN_ANI::Atk_Ani_WheelWind:
					Play_WheelWind();
					break;
				case YACHAMAN_ANI::Atk_Ani_Run_Start:
				case YACHAMAN_ANI::Atk_Ani_Run_Loop:
				case YACHAMAN_ANI::Atk_Ani_Run_End:
					Play_Combo_RunHammering();
					break;
				}
			}
			if (YACHAMAN_ATKTYPE::ATK_COMBO == m_eSecondCategory_ATK)
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
					Play_Combo_Shoulder_HalfClock();
					break;
				case ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK:
					Play_Combo_Shoulder_TurnTwice();
					break;
				case ATK_COMBO_TYPE::COMBO_RUNHAMMERING:
					Play_Combo_RunHammering();
					break;
				}
			}
		}
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

	return;
}

void CYachaMan::Skill_RotateBody()
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

void CYachaMan::Skill_CoolDown()
{
	//좀더 생각해서 넣어봅시다
	if (false == m_tObjParam.bIsAttack && false == m_tObjParam.bCanAttack && false == m_bIsAtkCombo)
	{
		m_fCoolDown += DELTA_60;
		m_bIsCoolDown = true;

		if (2.f <= m_fCoolDown)
		{
			m_fCoolDown = 0.f;
			m_bIsCoolDown = false;
			m_tObjParam.bCanAttack = true;
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		}
	}
}

void CYachaMan::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	// 네비 없이
	//m_pTransformCom->Add_Pos(_fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), _vDir);

	// 네비게이션 적용하면 
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CYachaMan::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CYachaMan::Reset_BattleState()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;
	m_tObjParam.bIsAttack = false; //공격중 아님
	m_bCanRandomAtkCategory = true; //랜덤공격 가능하게끔
	m_bIsAtkCombo = false; //콤보공격중 아님

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CYachaMan::Play_Idle()
{
	if (true == m_bInRecognitionRange)
	{
		m_bCanRandomIdle = true;
		if (true == m_tObjParam.bCanAttack)
		{
			//인지, 공격 가능->대기
			m_eState = YACHAMAN_ANI::Hammer_Idle;
		}
		else
		{
			//인지, 공격 불가->경계
			Skill_RotateBody();
			m_eState = YACHAMAN_ANI::Hammer_Idle;
		}
	}
	else
	{
		m_eState = YACHAMAN_ANI::Hammer_Idle;
	}
}

void CYachaMan::Play_Lurk()
{
	if (true == m_bInRecognitionRange)
	{
		if (YACHAMAN_ANI::Lurk == m_eState)
		{
			if(m_pMeshCom->Is_Finish_Animation())
				m_eState = YACHAMAN_ANI::Lurk_End;

			return;
		}
		else if (YACHAMAN_ANI::Lurk_End == m_eState && m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_bCanRandomIdle = true;
			m_eState = YACHAMAN_ANI::Hammer_Idle;
			return;
		}
	}
	else
	{
		m_eState = YACHAMAN_ANI::Lurk;
		return;
	}
}

void CYachaMan::Play_Eat()
{
	if (true == m_bInRecognitionRange)
	{
		if (YACHAMAN_ANI::Eat == m_eState)
		{
			m_eState = YACHAMAN_ANI::Eat_End;
			return;
		}
		else if (YACHAMAN_ANI::Eat_End == m_eState && m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_bCanRandomIdle = true;
			m_eState = YACHAMAN_ANI::Hammer_Idle;
			return;
		}
	}
	else
	{
		m_eState = YACHAMAN_ANI::Eat;
		return;
	}
}

void CYachaMan::Play_Walk()
{
	m_eState = YACHAMAN_ANI::Walk;

	Skill_RotateBody();

	Skill_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Decre_Skill_Movement(0.1f);

	return;
}

void CYachaMan::Play_Run()
{
	m_eState = YACHAMAN_ANI::Run;

	Skill_RotateBody();

	Skill_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Decre_Skill_Movement(0.1f);

	return;
}

void CYachaMan::Play_Dodge()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bDodge)
	{
		m_eState = YACHAMAN_ANI::Dodge;
		m_tObjParam.bDodge = false;
		m_bIsDodge = true;
		Reset_BattleState();
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
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

void CYachaMan::Play_RandomAtkNormal()
{
	m_iAtkRandom = CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RIGHT, ATK_NORMAL_TYPE::NORMAL_WHEELWIND);

	switch (m_iAtkRandom)
	{
	case ATK_NORMAL_TYPE::NORMAL_RIGHT:
		m_eState = YACHAMAN_ANI::Atk_Ani_R;
		break;
	case ATK_NORMAL_TYPE::NORMAL_LEFT:
		m_eState = YACHAMAN_ANI::Atk_Ani_L;
		break;
	case ATK_NORMAL_TYPE::NORMAL_HAMMERING:
		m_eState = YACHAMAN_ANI::Atk_Ani_Hammering;
		break;
	case ATK_NORMAL_TYPE::NORMAL_SHOULDER:
		m_eState = YACHAMAN_ANI::Atk_Ani_Shoulder;
		break;
	case ATK_NORMAL_TYPE::NORMAL_TURNTWICE:
		m_eState = YACHAMAN_ANI::Atk_Ani_TurnTwice;
		break;
	case ATK_NORMAL_TYPE::NORMAL_HALFCLOCK:
		m_eState = YACHAMAN_ANI::Atk_Ani_HalfClock;
		break;
	case ATK_NORMAL_TYPE::NORMAL_TARGETHAMMERING:
		m_eState = YACHAMAN_ANI::Atk_Ani_TargetHammering;
		break;
	case ATK_NORMAL_TYPE::NORMAL_WHEELWIND:
		m_eState = YACHAMAN_ANI::Atk_Ani_WheelWind;
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
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			cout << "심, 랜덤 노말->우치기 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//1.3~1.9 이동 1.8~2.0 타격
			if (1.3f < AniTime && 1.9f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.8f < AniTime && 2.0f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (2.0f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
}

void CYachaMan::Play_L()
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
			cout << "심, 랜덤 노말->좌치기 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.6~1.1 이동 1.2~1.4타격
			if (0.6f < AniTime && 1.1f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.2f < AniTime && 1.4f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if ( 1.4f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
}

void CYachaMan::play_Hammering()
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
			cout << "심, 랜덤 노말->해머링 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.9~1.8 이동 1.4~1.8 타격
			if (0.9f < AniTime && 1.8f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.4f < AniTime && 1.8f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}
			}
			if (1.8f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
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
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			cout << "심, 랜덤 노말->어깨치기 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.9~1.3 이동 0.9~1.3 타격
			if (0.9f < AniTime && 1.3f < AniTime)
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
			if (1.3f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
}

void CYachaMan::Play_TurnTwice()
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
			cout << "심, 랜덤 노말->2회전 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.8~2.0 2.9~3.3 이동 0.9~1.2 1.6~1.9 타격
			if (0.8f < AniTime && 2.0f < AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (0.9f < AniTime && 1.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (1.2f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			if (1.6f < AniTime && 1.9f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (1.9f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			if (2.9f < AniTime && 3.3f < AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}
}

void CYachaMan::Play_HalfClock()
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
			cout << "심, 랜덤 노말->반시계공격 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.8~2.0회전이동 2.9~3.3 짧게복귀 0.8~1.2타격
			if (0.8f < AniTime && 2.0f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (0.8f < AniTime && 1.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}
			}
			if (1.2f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			if (2.9f < AniTime && 3.3f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}
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
			cout << "심, 랜덤 노말->파리치기 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//1.9~3.0 이동 3.9~5.6 짧게복귀 1.9~2.6 타격
			if (1.9f < AniTime && 3.0f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.9f < AniTime && 2.6f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (2.6f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			if (3.9f<AniTime && 5.6f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}
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
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			cout << "심, 랜덤 노말->휠윈드 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			if (1.4f < AniTime && 1.8f > AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					//1.4~1.8 이동 1.4~1.8 타격
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.8f <= AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			if (1.9f < AniTime && 2.2f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					//1.9~2.2 이동
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (2.1f < AniTime && 2.4f > AniTime)
			{
				//2.1~2.4 타격
				if (false == m_bEventTrigger[3])
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (2.4f <= AniTime)
			{
				if (false == m_bEventTrigger[4])
				{
					m_bEventTrigger[4] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			if (2.3f < AniTime && 2.6f > AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					//	2.3~2.6 이동
					m_bEventTrigger[5] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (2.7f < AniTime && 2.8f > AniTime)
			{
				//2.7~2.8 타격
				if (false == m_bEventTrigger[6])
				{
					m_bEventTrigger[6] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (2.8f <= AniTime)
			{
				if (false == m_bEventTrigger[7])
				{
					m_bEventTrigger[7] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			if (2.7f < AniTime && 2.9f < AniTime)
			{
				if (m_bEventTrigger[8] == false)
				{
					//2.7~2.9 이동
					m_bEventTrigger[8] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (3.1f < AniTime && 3.3f < AniTime)
			{
				//3.1~3.3 타격
				if (false == m_bEventTrigger[9])
				{
					m_bEventTrigger[9] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (3.0f < AniTime && 3.3f < AniTime)
			{
				if (m_bEventTrigger[10] == false)
				{
					//3.0~3.1 이동
					m_bEventTrigger[10] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);

			}
			if (3.3f <= AniTime)
			{
				if (false == m_bEventTrigger[11])
				{
					m_bEventTrigger[11] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
		}
	}
}

void CYachaMan::Play_RandomAtkCombo()
{
	m_iAtkRandom = CALC::Random_Num(ATK_COMBO_TYPE::COMBO_R_L, ATK_COMBO_TYPE::COMBO_RUNHAMMERING);

	switch (m_iAtkRandom)
	{
	case ATK_COMBO_TYPE::COMBO_R_L:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_L;
		m_eState = Atk_Ani_R;
		break;
	case ATK_COMBO_TYPE::COMBO_R_HAMMERING:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_R_HAMMERING;
		m_eState = Atk_Ani_R;
		break;
	case ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_TURNTWICE;
		m_eState = Atk_Ani_Shoulder;
		break;
	case ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SHOULDER_HALFCLOCK;
		m_eState = Atk_Ani_Shoulder;
		break;
	case ATK_COMBO_TYPE::COMBO_RUNHAMMERING:
		m_eAtkCombo = ATK_COMBO_TYPE::COMBO_RUNHAMMERING;
		m_eState = Atk_Ani_Run_Start;
		break;
	}

	return;
}

void CYachaMan::Play_Combo_R_L()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (Atk_Ani_R == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.45f))
		{
			cout << "심, 랜덤 콤보->좌치기 우치기, 좌치기 종료" << endl;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Atk_Ani_L;
			return;
		}
		else
		{
			if (AniTime > 1.3f && AniTime < 1.9f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (AniTime > 1.8f && AniTime < 2.0f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (AniTime >= 2.0f)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
	else if (Atk_Ani_L == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			cout << "심, 랜덤 콤보->좌치기 우치기 종료" << endl;
			Reset_BattleState();
			return;
		}
		else
		{
			//0.6~1.1 이동 1.2~1.4타격
			if (0.6f < AniTime && 1.1f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.2f < AniTime && 1.4f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (1.4f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}

	return;
}

void CYachaMan::Play_Combo_R_Hammering()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (Atk_Ani_R == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.45f))
		{
			cout << "심, 랜덤 콤보->우치기 어깨치기, 우치기 종료" << endl;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Atk_Ani_Hammering;
			return;
		}
		else
		{
			if (AniTime > 1.3f && AniTime < 1.9f)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (AniTime > 1.8f && AniTime < 2.0f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (AniTime >= 2.0f)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
					return;
				}
			}
		}
	}
	else if (Atk_Ani_Hammering == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			cout << "심, 랜덤 콤보->우치기 어깨치기 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.9~1.8 이동 1.4~1.8 타격
			if (0.9f < AniTime && 1.8f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (1.4f < AniTime && 1.8f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}
			}
			if (1.8f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}

	return;
}

void CYachaMan::Play_Combo_Shoulder_TurnTwice()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (Atk_Ani_Shoulder == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.28f))
		{
			cout << "심, 랜덤 콤보->어깨치기 2회전, 어깨치기 종료" << endl;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Atk_Ani_TurnTwice;
			return;
		}
		else
		{
			//0.9~1.3 이동 0.9~1.3 타격
			if (0.9f < AniTime && 1.3f < AniTime)
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
			else if (1.3f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
	else if (Atk_Ani_TurnTwice == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			cout << "심, 랜덤 콤보->어깨치기 2회전 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.8~2.0 2.9~3.3 이동 0.9~1.2 1.6~1.9 타격
			if (0.8f < AniTime && 2.0f < AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (0.9f < AniTime && 1.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (1.2f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

			}
			if (1.6f < AniTime && 1.9f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (1.9f <= AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			if (2.9f < AniTime && 3.3f < AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}

	return;
}

void CYachaMan::Play_Combo_Shoulder_HalfClock()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (Atk_Ani_Shoulder == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.28f))
		{
			cout << "심, 랜덤 콤보->어깨치기 반시계, 어깨치기 종료" << endl;
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Atk_Ani_HalfClock;
			return;
		}
		else
		{
			//0.9~1.3 이동 0.9~1.3 타격
			if (0.9f < AniTime && 1.3f < AniTime)
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
			else if (1.3f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
				return;
			}
		}
	}
	else if (Atk_Ani_HalfClock == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			cout << "심, 랜덤 콤보->어깨치기 반시계 종료" << endl;
			Reset_BattleState();

			return;
		}
		else
		{
			//0.8~2.0회전이동 2.9~3.3 짧게복귀 0.8~1.2타격
			if (0.8f < AniTime && 2.0f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
			if (0.8f < AniTime && 1.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}

			}
			if (1.2f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			if (2.9f < AniTime && 3.3f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}
}

void CYachaMan::Play_Combo_RunHammering()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (Atk_Ani_Run_Start == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			cout << "심, 랜덤 콤보->달려가 내리치기,시작 종료" << endl;
			m_eState = Atk_Ani_Run_Loop;
			return;
		}
		else
		{
			if (1.1f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 10.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 4.f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}
	else if (Atk_Ani_Run_Loop == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			cout << "심, 랜덤 콤보->달려가 내리치기,루프 종료" << endl;
			m_eState = Atk_Ani_Run_End;
			return;
		}
		else
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 20.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 4.f;
			}

			Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Decre_Skill_Movement(m_fSkillMoveMultiply);
		}
	}
	else if (Atk_Ani_Run_End == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			cout << "심, 랜덤 콤보->달려가 내리치기,종료 종료" << endl;
			Reset_BattleState();
			return;
		}
		else
		{
			if (0.3f < AniTime && 0.6f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			if (2.6f < AniTime && 3.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Skill_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Decre_Skill_Movement(m_fSkillMoveMultiply);
			}
		}
	}

	return;
}

void CYachaMan::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		cout << "심, Hit" << endl;
		m_tObjParam.bIsHit = true;
		m_eState = YACHAMAN_ANI::Hit_S_FL;
		Reset_BattleState();
	}
	else
	{
		//맞는 애니중, 70퍼이상 애니 재생됬다면 또 처맞기 가능
		if (true == m_tObjParam.bIsHit && m_pMeshCom->Is_Finish_Animation(0.7f))
		{
			if (false == m_tObjParam.bCanHit)
			{
				cout << "심, 또 Hit" << endl;
				++m_iDodgeCount; //200406 수정
				m_tObjParam.bCanHit = true;

				return;
			}
		}
		//맞는 애니중, 95퍼 이상 재생, 행동 종료
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			cout << "심, Hit 종료" << endl;
			++m_iDodgeCount;
			m_tObjParam.bIsHit = false;
			m_tObjParam.bCanHit = true;
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;

			return;
		}
	}
}

void CYachaMan::Play_Down_Strong()
{
}

void CYachaMan::Play_Down_Weak()
{
}

void CYachaMan::Play_Dead()
{
	m_eState = YACHAMAN_ANI::Dead;

	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	{
		//봐서 1.f인가 아니면 새로운 배수인가를 확인합시다
		m_bIsDead = true;
		m_dAniPlayMul = 0;
		//디졸브 작동
	}

}

void CYachaMan::Play_Dead_Strong()
{
}

HRESULT CYachaMan::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_YachaMan", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CYachaMan::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);

	CManagement*		pManagement = CManagement::Get_Instance();
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

HRESULT CYachaMan::Ready_Collider()
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

HRESULT CYachaMan::Ready_BoneMatrix()
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

CYachaMan * CYachaMan::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
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
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
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
