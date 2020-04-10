#include "stdafx.h"
#include "..\Headers\YachaMan.h"
#include "..\Headers\Weapon.h"

//#include "MonsterUI.h"
//#include "DamegeNumUI.h"

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
	Ready_Weapon();

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

	m_bInRecognitionRange = false; //인지 범위 여부
	m_bInAtkRange = false; //공격 범위 여부
	m_bCanChase = false; //추격 여부
	m_bIsDodge = false; //회피 진행중 여부
	m_bCanCoolDown = false; //쿨타임 여부
	m_bIsCoolDown = false; //쿨타임 진행중 여부

	m_bCanAtkCategoryRandom = true;
	m_bIsAtkCombo = false;
	m_bCanIdleRandom = true;

	m_fRecognitionRange = 10.f; //인지범위
	m_fAtkRange = 4.f; //공격범위
	m_fCoolDownMax = 1.5f; //쿨타임 맥스값은 유동적
	m_fCoolDownCur = 0.f; //쿨타임 시간을 더함
	m_fSpeedForCollisionPush = 2.f;
	m_iRandom = 0;
	m_iDodgeCount = 0; //n회 피격시 바로 회피

	return NOERROR;
}

_int CYachaMan::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bIsDead)
		return DEAD_OBJ;

	Check_Hit();
	Check_Dist();
	Set_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_Collision();

	return NOERROR;
}

_int CYachaMan::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	m_pHammer->Late_Update_GameObject(TimeDelta);

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

	m_pHammer->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Render_Collider();

	return NOERROR;
}

HRESULT CYachaMan::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;

	//_mat matLightVP = g_pManagement->Get_LightViewProj();
	//if (FAILED(pShader->Set_Value("g_LightVP_Close", &matLightVP, sizeof(_mat))))
	//	return E_FAIL;

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

void CYachaMan::Update_Collider()
{
	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[Bone_RightArm] * m_pTransformCom->Get_WorldMat();

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

void CYachaMan::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));
}

void CYachaMan::Check_CollisionPush()
{
	//플레이어
	list<CGameObject*> tmpList = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);
	//몬스터

	for (auto& iter : tmpList)
	{
		CCollider* pCollider = TARGET_TO_COL(iter);

		// 지금 속도값 임의로 넣었는데 구해서 넣어줘야함 - 완료
		if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSpeedForCollisionPush * DELTA_60))
		{
			CTransform* pTrans = TARGET_TO_TRANS(iter);
			CNavMesh*   pNav = TARGET_TO_NAV(iter);

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

void CYachaMan::Check_Hit()
{
	if (MONSTER_ANITYPE::DEAD == m_eFirstCategory)
	{
		return;
	}
	else
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
	if (MONSTER_ANITYPE::HIT == m_eFirstCategory ||
		MONSTER_ANITYPE::DOWN == m_eFirstCategory ||
		MONSTER_ANITYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_tObjParam.bIsAttack ||
		true == m_bIsAtkCombo ||
		true == m_bIsDodge ||
		true == m_tObjParam.bIsHit)
		return;

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAtkRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

	if (true == m_bInRecognitionRange)
	{
		if (YACHAMAN_ANI::Lurk == m_eState || YACHAMAN_ANI::Lurk_End == m_eState ||
			YACHAMAN_ANI::Eat == m_eState || YACHAMAN_ANI::Eat_End == m_eState)
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			return;
		}
		else
		{
			if (true == m_bInAtkRange)
			{
				if (true == m_tObjParam.bCanAttack)
				{
					if (true == m_bIsCoolDown)
					{
						//인지,범위,공격가능,쿨타임 -> 대기패턴, 경계패턴
						m_eFirstCategory = MONSTER_ANITYPE::IDLE;
						m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_IDLE;
						Function_RotateBody();
						return;
					}
					else
					{
						//인지,범위,공격가능,쿨타임아님->공격 패턴
						m_bCanAtkCategoryRandom = true;
						m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
						Function_RotateBody();
						return;
					}
				}
				else
				{
					//인지, 범위, 공격불가능 -> 공격중은 아닌데? walk하면서 주위 맴돌기,경계상태에 가깝다
					m_eFirstCategory = MONSTER_ANITYPE::IDLE;
					m_eSecondCategory_IDLE = YACHAMAN_IDLETYPE::IDLE_IDLE;
					Function_RotateBody();
					return;
				}
			}
			else
			{			
				m_bCanChase = true;
				m_eFirstCategory = MONSTER_ANITYPE::MOVE;
				m_eSecondCategory_MOVE = YACHAMAN_MOVETYPE::MOVE_RUN;
				return;
			}
		}
	}
	else 
	{
		m_bCanChase = false;
		m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		if (true == m_bCanIdleRandom)
		{
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
		if (true == m_bCanIdleRandom)
		{
			m_bCanIdleRandom = false;

			switch (m_eSecondCategory_IDLE)
			{
			case YACHAMAN_IDLETYPE::IDLE_IDLE:
				m_eState = YACHAMAN_ANI::Hammer_Idle;
				break;
			case YACHAMAN_IDLETYPE::IDLE_EAT:
				m_eState = YACHAMAN_ANI::Eat;
				break;
			case YACHAMAN_IDLETYPE::IDLE_LURK:
				m_eState = YACHAMAN_ANI::Lurk;
				break;
			}
		}
		else
		{
			switch (m_eSecondCategory_IDLE)
			{
			case Client::CYachaMan::IDLE_IDLE:
				Play_Idle();
				break;
			case Client::CYachaMan::IDLE_EAT:
				Play_Eat();
				break;
			case Client::CYachaMan::IDLE_LURK:
				Play_Lurk();
				break;
			}
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
		if (true == m_bCanAtkCategoryRandom)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;
			m_bCanAtkCategoryRandom = false;

			m_iRandom = CALC::Random_Num(YACHAMAN_ATKTYPE::ATK_NORMAL, YACHAMAN_ATKTYPE::ATK_COMBO);

			m_iRandom = 1; //이동값 수치 조절용 임시 변수값

			switch (m_iRandom)
			{
			case YACHAMAN_ATKTYPE::ATK_NORMAL:
				m_eSecondCategory_ATK = YACHAMAN_ATKTYPE::ATK_NORMAL;
				Play_RandomAtkNormal();
				break;
			case YACHAMAN_ATKTYPE::ATK_COMBO:
				m_eSecondCategory_ATK = YACHAMAN_ATKTYPE::ATK_COMBO;
				Play_RandomAtkCombo();
				m_bIsAtkCombo = true;
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
					Play_Hammering();
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

void CYachaMan::Function_RotateBody()
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

void CYachaMan::Function_CoolDown()
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
}

void CYachaMan::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	// 네비 미적용
	//m_pTransformCom->Add_Pos(_fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), _vDir);

	// 네비 적용 
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CYachaMan::Function_DecreMoveMent(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CYachaMan::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bDodge = false;
	m_bIsDodge = false;

	m_tObjParam.bIsAttack = false;
	m_bCanAtkCategoryRandom = true;
	m_bIsAtkCombo = false;

	m_vecAttackCol[0]->Set_Enabled(false);

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CYachaMan::Play_Idle()
{
	if (true == m_bInRecognitionRange)
	{
		if (true == m_tObjParam.bCanAttack)
		{
			//인지, 공격 가능->대기
			m_eState = YACHAMAN_ANI::Hammer_Idle;
			return;
		}
		else
		{
			//인지, 공격 불가->경계
			Function_RotateBody();
			m_eState = YACHAMAN_ANI::Hammer_Idle;
			return;
		}
	}
	else
	{
		m_bCanIdleRandom = true;
		m_eState = YACHAMAN_ANI::Hammer_Idle;
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
			m_bCanIdleRandom = true;
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
		else if (YACHAMAN_ANI::Lurk_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.98f))
			{
				m_bCanIdleRandom = true;
				m_eState = YACHAMAN_ANI::Hammer_Idle;
			}
			
			return;
		}
	}
	else
	{
		m_eState = YACHAMAN_ANI::Lurk;

		return;
	}
}

void CYachaMan::Play_Walk()
{
	m_eState = YACHAMAN_ANI::Walk;

	Function_RotateBody();

	Function_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CYachaMan::Play_Run()
{
	m_eState = YACHAMAN_ANI::Run;

	Function_RotateBody();

	Function_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CYachaMan::Play_Dodge()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	//회피는 변수가 좀 이상하니 일단 제외
	if (false == m_tObjParam.bDodge)
	{
		m_eState = YACHAMAN_ANI::Dodge;
		m_tObjParam.bDodge = true; //회피임
		m_bIsDodge = true; //회피중
		Function_ResetAfterAtk();
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			m_bIsDodge = false;
			m_tObjParam.bDodge = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (0.3f < AniTime && 0.8f> AniTime)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.1f;
			}

			Function_RotateBody();

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));

			Function_DecreMoveMent(m_fSkillMoveMultiply);
			return;
		}
	}
}

void CYachaMan::Play_RandomAtkNormal()
{
	//프로토 검사용
	m_iRandom = CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RIGHT, ATK_NORMAL_TYPE::NORMAL_SHOULDER);

	//완전 랜덤 추후 특정 상황에 맞워서 적절하게
	//m_iRandom = CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_RIGHT, ATK_NORMAL_TYPE::NORMAL_WHEELWIND);

	m_iRandom = 3; //이동값 수치 조절용 임시 변수값

	switch (m_iRandom)
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
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//1.3~1.9 이동 1.8~2.0 타격 이건 폐기
			//맥스로 보정한 수치임
			if (2.833f < AniTime && 4.233f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 8.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.5f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (3.50f < AniTime && 3.93f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (3.93f <= AniTime)
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
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//0.6~1.1 이동 1.2~1.4타격
			if (0.9f < AniTime && 2.833f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.3f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.4f < AniTime && 2.7f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if ( 2.7f <= AniTime)
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

void CYachaMan::Play_Hammering()
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
			m_fCoolDownMax = 1.3f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//0.9~1.8 이동 1.4~1.8 타격
			if (2.167f < AniTime && 3.267f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (3.267f < AniTime && 3.633f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}
			}
			else if (3.633f <= AniTime)
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
			m_fCoolDownMax = 0.3f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//2.5인가 4.3인가?
			if (1.500f < AniTime && 2.10f < AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
				return;
			}
			else if (2.067f < AniTime && 2.50f < AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (2.50f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
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
			m_fCoolDownMax = 1.5f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//0.8~2.0 2.9~3.3 이동 0.9~1.2 1.6~1.9 타격
			if (1.6f < AniTime && 4.0f < AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.8f < AniTime && 4.8f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (2.4f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (3.2f < AniTime && 3.8f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (3.8f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			else if (5.8f < AniTime && 6.6f < AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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
			m_fCoolDownMax = 0.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//0.8~2.0회전이동 2.9~3.3 짧게복귀 0.8~1.2타격
			if (1.6f < AniTime && 4.0f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.6f < AniTime && 2.4f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}
			}
			else if (2.4f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			else if (5.8f < AniTime && 6.6f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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
			m_fCoolDownMax = 1.7f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//1.9~3.0 이동 3.9~5.6 짧게복귀 1.9~2.6 타격
			if (3.8f < AniTime && 6.0f > AniTime)
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
			else if (3.8f < AniTime && 5.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (5.2f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			else if (7.8f<AniTime && 11.2f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.5f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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
			m_fCoolDownMax = 1.8f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (2.8f < AniTime && 3.6f > AniTime)
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

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (3.6f <= AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (3.8f < AniTime && 4.4f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					//1.9~2.2 이동
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (4.2f < AniTime && 4.8f > AniTime)
			{
				//2.1~2.4 타격
				if (false == m_bEventTrigger[3])
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (4.8f <= AniTime)
			{
				if (false == m_bEventTrigger[4])
				{
					m_bEventTrigger[4] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (4.6f < AniTime && 5.2f > AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					//	2.3~2.6 이동
					m_bEventTrigger[5] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (5.4f < AniTime && 5.6f > AniTime)
			{
				//2.7~2.8 타격
				if (false == m_bEventTrigger[6])
				{
					m_bEventTrigger[6] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (5.6f <= AniTime)
			{
				if (false == m_bEventTrigger[7])
				{
					m_bEventTrigger[7] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (5.4f < AniTime && 5.8f < AniTime)
			{
				if (m_bEventTrigger[8] == false)
				{
					//2.7~2.9 이동
					m_bEventTrigger[8] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (6.0f < AniTime && 6.6f < AniTime)
			{
				if (m_bEventTrigger[9] == false)
				{
					//3.0~3.1 이동, 타격
					m_bEventTrigger[9] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);

			}
			else if (6.6f <= AniTime)
			{
				if (false == m_bEventTrigger[10])
				{
					m_bEventTrigger[10] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}
}

void CYachaMan::Play_RandomAtkCombo()
{
	//프로토 검사용
	m_iRandom = CALC::Random_Num(ATK_COMBO_TYPE::COMBO_R_L, ATK_COMBO_TYPE::COMBO_R_HAMMERING);

	//완점 랜덤 콤보용
	//m_iRandom = CALC::Random_Num(ATK_COMBO_TYPE::COMBO_R_L, ATK_COMBO_TYPE::COMBO_RUNHAMMERING);

	m_iRandom = ATK_COMBO_TYPE::COMBO_R_L;

	switch (m_iRandom)
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
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Atk_Ani_L;
			return;
		}
		else
		{
			if (2.833f < AniTime && 4.233f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 8.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.5f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (3.50f < AniTime && 3.93f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (3.93f <= AniTime)
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
			m_fCoolDownMax = 0.5f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();
			return;
		}
		else
		{
			//0.6~1.1 이동 1.2~1.4타격
			if (0.9f < AniTime && 2.833f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 6.f; //6
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.5f; //0.3
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.4f < AniTime && 2.7f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (2.7f <= AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
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
			m_vecAttackCol[0]->Set_Enabled(false);
			m_eState = Atk_Ani_Hammering;
			return;
		}
		else
		{
			if (1.3f < AniTime && 1.8f > AniTime)
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
			else if (1.8f <= AniTime && 2.0f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (2.0f <= AniTime)
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
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.2f;
			Function_ResetAfterAtk();
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

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.4f < AniTime && 1.8f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}
			}
			else if (1.8f <= AniTime)
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

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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
			m_fCoolDownMax = 1.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();
			return;

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

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (0.9f < AniTime && 1.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (1.2f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}

			}
			else if (1.6f < AniTime && 1.9f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (1.9f <= AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (2.9f < AniTime && 3.3f < AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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
			m_fCoolDownMax = 0.7f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

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

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (0.8f < AniTime && 1.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true); 
				}

			}
			else if (1.2f <= AniTime)
			{
				m_vecAttackCol[0]->Set_Enabled(false);
			}
			else if (2.9f < AniTime && 3.3f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
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
					m_fSkillMoveSpeed_Cur = 15.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 3.f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
		}
	}
	else if (Atk_Ani_Run_Loop == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
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
				m_fSkillMoveMultiply = 3.f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (Atk_Ani_Run_End == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_fCoolDownMax = 1.4f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();
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
			else if (2.6f < AniTime && 3.2f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 2.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
		}
	}

	return;
}

void CYachaMan::Play_Hit()
{
	
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;
		m_eState = YACHAMAN_ANI::Hit_S_FL;
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
			if (false == m_tObjParam.bCanHit)
			{
				m_tObjParam.bCanHit = true;
				Check_FBLR();

				return;
			}
		}
	}

	return;
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

	m_pHammer->Start_Dissolve();
	//이동 이벤트 처리를 해줍시다^^
	//이 친구 사극에서 죽듯이 죽습니다, 이동처리하세요
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

HRESULT CYachaMan::Ready_Weapon()
{
	m_pHammer = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pHammer->Change_WeaponData(CWeapon::WPN_Hammer_Normal);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pHammer->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pHammer->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
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

	fRadius = 0.7f;

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
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightArm]->_41, m_matBone[Bone_RightArm]->_42, m_matBone[Bone_RightArm]->_43));
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
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightArm]->_41, m_matBone[Bone_RightArm]->_42, m_matBone[Bone_RightArm]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CYachaMan::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightArm", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_RightArm] = &pFrame->CombinedTransformationMatrix;

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
	Safe_Release(m_pHammer);
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
