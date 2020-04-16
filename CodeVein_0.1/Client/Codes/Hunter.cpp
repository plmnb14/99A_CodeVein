#include "stdafx.h"
#include "..\Headers\Hunter.h"
#include "..\Headers\Weapon.h"

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
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Weapon();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE);
	m_pTargetTransform = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE));

	m_eFirstCategory = MONSTER_ANITYPE::IDLE;
	m_tObjParam.fHp_Max = 280.f; //4~5대 사망, 기본공격력 20+-5에서 피감소
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fDamage = 25.f;

	m_tObjParam.bCanHit = true; //맞기 가능
	m_tObjParam.bIsHit = false;	//맞기 진행중 아님
	m_tObjParam.bCanAttack = true; //공격 가능
	m_tObjParam.bIsAttack = false; //공격 진행중 아님
	m_tObjParam.bDodge = false; //첫 생성시 회피 비활성

	m_bInRecognitionRange = false; //인지 범위 여부
	m_bInAtkRange = false; //공격 범위 여부
	m_bCanChase = false; //추격 여부
	m_bCanCoolDown = false; //쿨타임 여부
	m_bIsCoolDown = false; //쿨타임 진행중 여부

	m_bCanAtkCategoryRandom = true;
	m_bIsAtkCombo = false;
	m_bCanIdleRandom = true;

	m_fRecognitionRange = 20.f; //인지범위
	m_fAtkRange = 5.f; //공격범위
	m_fCoolDownMax = 0.f; //쿨타임 맥스값은 유동적
	m_fCoolDownCur = 0.f; //쿨타임 시간을 더함
	m_fSpeedForCollisionPush = 2.f;
	m_iRandom = 0;
	m_iDodgeCount = 0; //n회 피격시 바로 회피

	return S_OK;
}

_int CHunter::Update_GameObject(_double TimeDelta)
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

	return NO_EVENT;
}

_int CHunter::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;
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
			if (MONSTER_ANITYPE::DEAD != m_eFirstCategory)
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

			//m_pShaderCom->Begin_Pass(m_iPass);

			//if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
			//	return E_FAIL;

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

	return S_OK;
}

void CHunter::Update_Collider()
{
	_ulong matrixIdx = 0;

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
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CHunter::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));
}

void CHunter::Check_CollisionPush()
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

void CHunter::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bIsAttack)
	{
		return;
	}

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
}

void CHunter::Function_CoolDown()
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
}

void CHunter::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bDodge = false;

	m_tObjParam.bIsAttack = false;
	m_bCanAtkCategoryRandom = true;
	m_bIsAtkCombo = false;

	for (auto& vetor_iter : m_vecAttackCol)
		vetor_iter->Set_Enabled(false);
	//m_vecAttackCol[0]->Set_Enabled(false);
	//m_vecAttackCol[1]->Set_Enabled(false);
	//m_vecAttackCol[2]->Set_Enabled(false);

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Set_Target_CanAttack(false);
	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Set_Enable_Trail(false);

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CHunter::Check_Hit()
{
	if (MONSTER_ANITYPE::DEAD == m_eFirstCategory)
	{
		return;
	}

	if (false == m_tObjParam.bCanHit)
	{
		++m_iDodgeCount;
		//피격시 불값을 온하고 카운트 해주고 불값 오프
		//콤보1->콤보2 연결도중에 맞으면 콤보 바로 종료
		//지금은 바로 피격시 off해버림
		//특수기,강공격,다운기 등등이 발동시 바로 콤보 off
		//머리에 생각이 떠돌고 있으니 새벽에 가능성 있음
		if (m_iDodgeCount >= m_iDodgeCountMax)
		{
			m_iDodgeCount = 0;
			m_eFirstCategory = MONSTER_ANITYPE::MOVE;
			m_eSecondCategory_MOVE = HUNTER_MOVETYPE::MOVE_DODGE;
			m_pMeshCom->Reset_OldIndx();
			Function_RotateBody();
		}
		else
		{
			if (true == m_tObjParam.bIsHit)
			{
				if (true == m_tObjParam.bHitAgain)
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					Check_FBLR();
					m_tObjParam.bHitAgain = false;
					m_pMeshCom->Reset_OldIndx();
				}
			}
			else
			{
				if (0 >= m_tObjParam.fHp_Cur)
				{
					m_eFirstCategory = MONSTER_ANITYPE::DEAD;
				}
				else
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					Check_FBLR();
				}
			}
		}
	}

	return;
}

void CHunter::Check_FBLR()
{
	//피격 방향, 데미지에 따른 히트 판정
	//m_eSecondCategory_HIT = HUNTER_HITTYPE::HIT_STRONG;
	m_eSecondCategory_HIT = HUNTER_HITTYPE::HIT_NORMAL;
	//m_eSecondCategory_HIT = HUNTER_HITTYPE::HIT_WEAK;
}

void CHunter::Check_Dist()
{
	if (MONSTER_ANITYPE::HIT == m_eFirstCategory ||
		MONSTER_ANITYPE::CC == m_eFirstCategory ||
		MONSTER_ANITYPE::DEAD == m_eFirstCategory)
		return;

	if (MONSTER_ANITYPE::MOVE == m_eFirstCategory &&
		HUNTER_MOVETYPE::MOVE_DODGE == m_eSecondCategory_MOVE)
		return;

	if (true == m_tObjParam.bIsAttack ||
		true == m_bIsAtkCombo ||
		true == m_tObjParam.bDodge ||
		true == m_tObjParam.bIsHit)
		return;

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAtkRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

	if (true == m_bInRecognitionRange)
	{
		if (HUNTER_ANI::Stand == m_eState || HUNTER_ANI::Stand_End == m_eState ||
			HUNTER_ANI::Crouch == m_eState || HUNTER_ANI::Crouch_End == m_eState ||
			HUNTER_ANI::Sit == m_eState || HUNTER_ANI::Sit_End == m_eState)
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
						//해당 플레이어 주위를 배회
						m_eFirstCategory = MONSTER_ANITYPE::IDLE;
						m_eSecondCategory_IDLE = HUNTER_IDLETYPE::IDLE_IDLE;
						Function_RotateBody();
					}
					else
					{
						//인지,범위,공격가능,쿨타임아님->공격 패턴
						m_bCanAtkCategoryRandom = true;
						m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
						Function_RotateBody();
					}
				}
				else
				{
					//인지, 범위, 공격불가능 -> 공격중은 아닌데? walk하면서 주위 맴돌기,경계상태에 가깝다
					//버프, 동료
					m_eFirstCategory = MONSTER_ANITYPE::IDLE;
					m_eSecondCategory_IDLE = HUNTER_IDLETYPE::IDLE_IDLE;
					Function_RotateBody();
				}
			}
			else
			{
				//인지 ->추적,동료,도망,버프 다양한 선택지 생성
				m_bCanChase = true;
				m_eFirstCategory = MONSTER_ANITYPE::MOVE;
				m_eSecondCategory_MOVE = HUNTER_MOVETYPE::MOVE_RUN;
			}
		}
	}
	else
	{
		m_bCanChase = false;
		m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		if (true == m_bCanIdleRandom)
		{
			switch (CALC::Random_Num(HUNTER_IDLETYPE::IDLE_IDLE, HUNTER_IDLETYPE::IDLE_SIT))
			{
			case HUNTER_IDLETYPE::IDLE_IDLE:
				m_eSecondCategory_IDLE = HUNTER_IDLETYPE::IDLE_IDLE;
				break;
			case HUNTER_IDLETYPE::IDLE_STAND:
				m_eSecondCategory_IDLE = HUNTER_IDLETYPE::IDLE_STAND;
				break;
			case HUNTER_IDLETYPE::IDLE_CROUCH:
				m_eSecondCategory_IDLE = HUNTER_IDLETYPE::IDLE_CROUCH;
				break;
			case HUNTER_IDLETYPE::IDLE_SIT:
				m_eSecondCategory_IDLE = HUNTER_IDLETYPE::IDLE_SIT;
				break;
			}
		}
	}

	return;
}

void CHunter::Set_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_ANITYPE::IDLE:
		if (true == m_bCanIdleRandom)
		{
			m_bCanIdleRandom = false;

			switch (m_eSecondCategory_IDLE)
			{
			case HUNTER_IDLETYPE::IDLE_IDLE:
				m_eState = HUNTER_ANI::Idle;
				break;
			case HUNTER_IDLETYPE::IDLE_STAND:
				m_eState = HUNTER_ANI::Stand;
				break;
			case HUNTER_IDLETYPE::IDLE_CROUCH:
				m_eState = HUNTER_ANI::Crouch;
				break;
			case HUNTER_IDLETYPE::IDLE_SIT:
				m_eState = HUNTER_ANI::Sit;
				break;
			}
		}
		else
		{
			switch (m_eSecondCategory_IDLE)
			{
			case HUNTER_IDLETYPE::IDLE_IDLE:
				Play_Idle();
				break;
			case HUNTER_IDLETYPE::IDLE_STAND:
				Play_Stand();
				break;
			case HUNTER_IDLETYPE::IDLE_CROUCH:
				Play_Crouch();
				break;
			case HUNTER_IDLETYPE::IDLE_SIT:
				Play_Sit();
				break;
			}
		}
		break;

	case MONSTER_ANITYPE::MOVE:
		switch (m_eSecondCategory_MOVE)
		{
		case HUNTER_MOVETYPE::MOVE_RUN:
			Play_Run();
			break;
		case HUNTER_MOVETYPE::MOVE_WALK:
			Play_Walk();
			break;
		case HUNTER_MOVETYPE::MOVE_DODGE:
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

			m_iRandom = CALC::Random_Num(HUNTER_ATKTYPE::ATK_NORMAL, HUNTER_ATKTYPE::ATK_COMBO);

			if (WEAPON_ANITYPE::HAMMER == m_eWeaponState)
				m_iRandom = 0;

			switch (m_iRandom)
			{
			case HUNTER_ATKTYPE::ATK_NORMAL:
				m_eSecondCategory_ATK = HUNTER_ATKTYPE::ATK_NORMAL;
				Play_RandomAtkNormal();
				break;
			case HUNTER_ATKTYPE::ATK_COMBO:
				m_eSecondCategory_ATK = HUNTER_ATKTYPE::ATK_COMBO;
				Play_RandomAtkCombo();
				m_bIsAtkCombo = true;
				break;
			}

			return;
		}
		else
		{
			if (HUNTER_ATKTYPE::ATK_NORMAL == m_eSecondCategory_ATK)
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
					Play_Halverd_TwoUpper();
					break;
				case HUNTER_ANI::Halberd_Atk_TwoSlash:
					Play_Halverd_SlashForth();
					break;
				case HUNTER_ANI::Halberd_Atk_Sweep:
					Play_Halverd_Sweap();
					break;
				case HUNTER_ANI::Halberd_Atk_Sp03:
					Play_Halverd_Swing_Jump();
					break;
				case HUNTER_ANI::Halberd_Atk_Sp02:
					Play_Halverd_ClockTwice();
					break;
				case HUNTER_ANI::Halberd_Atk_S01:
					Play_Halverd_DeepPierce();
					break;
				case HUNTER_ANI::Halberd_Atk_N01:
					Play_Halverd_Pierce();
					break;
				case HUNTER_ANI::Halberd_Atk_RaiseUp:
					Play_Halverd_RiseUp();
					break;
				case HUNTER_ANI::Halberd_Atk_Force:
					Play_Halverd_StepPierce();
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
			if (HUNTER_ATKTYPE::ATK_COMBO == m_eSecondCategory_ATK)
			{
				switch (m_eAtkCombo)
				{
				case ATK_COMBO_TYPE::COMBO_GUN_SHOT:
					Play_Gun_Combo_Shot();
					break;
				case ATK_COMBO_TYPE::COMBO_GUN_CQC:
					Play_Gun_Combo_CQC();
					break;
				case ATK_COMBO_TYPE::COMBO_HALVERD_THIRDATK:
					Play_Halverd_Combo_ThirdAtk();
					break;
				case ATK_COMBO_TYPE::COMBO_HALVERD_PIERCETWICE:
					Play_Halverd_Combo_PierceTwice();
					break;
				case ATK_COMBO_TYPE::COMBO_HALVERD_PIERCEWIND:
					Play_Halverd_Combo_PierceWind();
					break;
				case ATK_COMBO_TYPE::COMBO_LSWORD_NORMAL:
					Play_LSword_Combo_Normal();
					break;
				case ATK_COMBO_TYPE::COMBO_LSWORD_STRONG:
					Play_LSword_Combo_Strong();
					break;
				case ATK_COMBO_TYPE::COMBO_SSWORD_STEPPIERCE:
					Play_SSword_Combo_StepPierce();
					break;
				case ATK_COMBO_TYPE::COMBO_SSWORD_STRONG:
					Play_SSword_Combo_Strong();
					break;
				case ATK_COMBO_TYPE::COMBO_SSWORD_Diagonal_L:
					Play_SSword_Combo_Diagonal_L();
					break;
				}
			}
		}
		break;

	case MONSTER_ANITYPE::HIT:
		Play_Hit();
		break;

	case MONSTER_ANITYPE::CC:
		break;

	case MONSTER_ANITYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CHunter::Play_RandomAtkNormal()
{		
	switch (m_eWeaponState)
	{
	case WEAPON_ANITYPE::GUN:
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
	case WEAPON_ANITYPE::HALVERD:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_HALVERD_STEPPIERCE, ATK_NORMAL_TYPE::NORMAL_HALVERD_TWOUPPER))
		{
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_STEPPIERCE:
			m_eState = HUNTER_ANI::Halberd_Atk_Force;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_RISEUP:
			m_eState = HUNTER_ANI::Halberd_Atk_RaiseUp;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_PIERCE:
			m_eState = HUNTER_ANI::Halberd_Atk_N01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_DEEPPIERCE:
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_CLOCKTWICE:
			m_eState = HUNTER_ANI::Halberd_Atk_Sp02;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_SWING_JUMP:
			m_eState = HUNTER_ANI::Halberd_Atk_Sp03;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_SWEAP:
			m_eState = HUNTER_ANI::Halberd_Atk_Sweep;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_SLASHFORTH:
			m_eState = HUNTER_ANI::Halberd_Atk_TwoSlash;
			break;
		case ATK_NORMAL_TYPE::NORMAL_HALVERD_TWOUPPER:
			m_eState = HUNTER_ANI::Halberd_Atk_TwoUpper;
			break;
		}
		break;
	case WEAPON_ANITYPE::HAMMER:
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
	case WEAPON_ANITYPE::LSWORD:
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
	case WEAPON_ANITYPE::SSWORD:
		switch (CALC::Random_Num(ATK_NORMAL_TYPE::NORMAL_SSWORD_JUMP, ATK_NORMAL_TYPE::NORMAL_SSWORD_CRITICALDRAW))
		{
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_JUMP:
			m_eState = HUNTER_ANI::SSword_Atk_Jump;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_RAISEUP:
			m_eState = HUNTER_ANI::SSword_Atk_RaiseUp;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_UPPER:
			m_eState = HUNTER_ANI::SSword_Atk_N01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_Upper_L:
			m_eState = HUNTER_ANI::SSword_Atk_N04;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_WOODCHOP:
			m_eState = HUNTER_ANI::SSword_Atk_N05;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_ELBOW:
			m_eState = HUNTER_ANI::SSword_Atk_S01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_HELMETBREAK:
			m_eState = HUNTER_ANI::SSword_Atk_Sp01;
			break;
		case ATK_NORMAL_TYPE::NORMAL_SSWORD_CRITICALDRAW:
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
	case WEAPON_ANITYPE::GUN:
		if (m_fShotRange >= fLenth)
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
	case WEAPON_ANITYPE::HALVERD:
		switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_HALVERD_THIRDATK, ATK_COMBO_TYPE::COMBO_HALVERD_PIERCEWIND))
		{
		case ATK_COMBO_TYPE::COMBO_HALVERD_THIRDATK:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_HALVERD_THIRDATK;
			m_eState = HUNTER_ANI::Halberd_Atk_N01;
			break;
		case ATK_COMBO_TYPE::COMBO_HALVERD_PIERCETWICE:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_HALVERD_PIERCETWICE;
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		case ATK_COMBO_TYPE::COMBO_HALVERD_PIERCEWIND:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_HALVERD_PIERCEWIND;
			m_eState = HUNTER_ANI::Halberd_Atk_S01;
			break;
		}
		break;
	case WEAPON_ANITYPE::LSWORD:
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
	case WEAPON_ANITYPE::SSWORD:
		switch (CALC::Random_Num(ATK_COMBO_TYPE::COMBO_SSWORD_STEPPIERCE, ATK_COMBO_TYPE::COMBO_SSWORD_Diagonal_L))
		{
		case ATK_COMBO_TYPE::COMBO_SSWORD_STEPPIERCE:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SSWORD_STEPPIERCE;
			m_eState = HUNTER_ANI::SSword_Atk_Sp03_Start;
			break;
		case ATK_COMBO_TYPE::COMBO_SSWORD_STRONG:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SSWORD_STRONG;
			m_eState = HUNTER_ANI::SSword_Atk_S01;
		case ATK_COMBO_TYPE::COMBO_SSWORD_Diagonal_L:
			m_eAtkCombo = ATK_COMBO_TYPE::COMBO_SSWORD_Diagonal_L;
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
		else
		{
			if (0.267f < AniTime && 0.600f > AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.200f < AniTime && 2.133f > AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[3]->Set_Enabled(true); //발끝
				}
			}
			else if (2.133f <= AniTime)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[3]->Set_Enabled(false); //발끝
				}
			}
		}
	}
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
		else
		{
			if (1.267f < AniTime && 1.767f > AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 1.f;
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.533f < AniTime && 1.667f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_pWeapon->Set_Target_CanAttack(true);
					m_pWeapon->Set_Enable_Trail(true);
				}
			}
			else if (1.667f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_pWeapon->Set_Target_CanAttack(false);
					m_pWeapon->Set_Enable_Trail(true);
				}
				if (1.967f < AniTime)
				{
					if (m_bEventTrigger[3] == false)
					{
						m_bEventTrigger[3] = true;
						m_pWeapon->Set_Enable_Trail(false);
					}
				}
			}
		}
	}
}

void CHunter::Play_Gun_Shoot()
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
		else
		{
			if (1.500f < AniTime && 1.800f > AniTime)
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
			else if (2.067f < AniTime && 2.500f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (2.500f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (4.773f < AniTime && 6.800f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
		}
	}
}

void CHunter::Play_Gun_Snipe()
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
		else
		{
			if (1.500f < AniTime && 1.800f > AniTime)
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
			else if (2.067f < AniTime && 2.500f > AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
			}
			else if (2.500f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (4.773f < AniTime && 6.800f > AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_fSkillMoveSpeed_Cur = 0.2f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
		}
	}
}

void CHunter::Play_Gun_Combo_Shot()
{
}

void CHunter::Play_Gun_Combo_CQC()
{
}

void CHunter::Play_Halverd_StepPierce()
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
			cout << "스텝 찌르기 끝" << endl;
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
			}
		}
		else if (3.067 <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_RiseUp()
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
			}
		}
		else if (2.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_Pierce()
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
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_DeepPierce()
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
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_ClockTwice()
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
			}
		}
		else if (3.767f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_Swing_Jump()
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
			}
		}
		else if (3.900f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.400f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_Sweap()
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
			}
		}
		else if (6.067f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (5.500f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (4.667f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (3.400f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.567f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_SlashForth()
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
			}
		}
		else if (5.267f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (4.467f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (3.700f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (2.800f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_TwoUpper()
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
			}
		}
		else if (3.733f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (2.100f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

void CHunter::Play_Halverd_Combo_ThirdAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Halberd_Atk_N01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.6f))
		{
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
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
	else if (HUNTER_ANI::Halberd_Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.6f))
		{
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
			}
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::Halberd_Atk_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.867f < AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}
		else if (1.267f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}
		else if (0.633f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Halverd_Combo_PierceTwice()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Halberd_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
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
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
	else if (HUNTER_ANI::Halberd_Atk_S03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
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
			}

		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Halverd_Combo_PierceWind()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::Halberd_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.4f))
		{
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
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
	else if (HUNTER_ANI::Halberd_Atk_S02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
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
			}
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
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
			}
		}
		else if (1.967f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
				}
			}
			else if (2.233f <= AniTime)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;
					m_pWeapon->Set_Target_CanAttack(true);
					m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (4.300f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
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
			}
		}
		else if (3.033f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.233f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[1]->Set_Enabled(true);
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
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (2.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
				}
			}
			else if (1.600f <= AniTime)
			{
				if (false == m_bEventTrigger[2])
				{
					m_bEventTrigger[2] = true;
					m_pWeapon->Set_Target_CanAttack(true);
					m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.600f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (7.467f < AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (7.167f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (7.000f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}

		if (6.900f < AniTime && 7.767f > AniTime)
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
		else if (5.800f < AniTime && 6.733f > AniTime)
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
			}
		}
		else if (1.500f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
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
			}
		}
		else if (1.667f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			cout << "점프 공격 끝" << endl;
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
			}
		}
		else if (2.633f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (2.367f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			}
		}
		else if (1.167f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			cout << "어퍼->좌 끝" << endl;
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
			}
		}
		else if (1.900f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			cout << "장작베기" << endl;
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
			}
		}
		else if (1.867f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			cout << "팔꿈치 끝" << endl;
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
			}
		}
		else if (1.333f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
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
			cout << "투구깨기 끝" << endl;
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
			}
		}
		else if (4.333f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
			cout << "발도 끝" << endl;
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
			}
		}
		else if (2.633f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_SSword_Combo_StepPierce()
{
	//sp3 st,loop, end 0.9 0.9 0.8
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::SSword_Atk_Sp03_Start == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
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
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.1f;
		}

		Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
		Function_DecreMoveMent(m_fSkillMoveMultiply);
	}
	else if (HUNTER_ANI::SSword_Atk_Sp03_End == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.8f))
		{
			cout << "스텝쓰러스트 끝" << endl;
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
			}
		}
		else if (1.533f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
			}
		}
		else if (1.421f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (1.211f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Target_CanAttack(true);
			}
		}
		else if (1.103f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (0.904f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(true);
			}
		}
		else if (0.857f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (0.665f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(true);
			}
		}
		else if (0.526f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (0.381f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(true);
			}
		}
		else if (0.233f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (0.f <= AniTime)
		{
			if (false == m_bEventTrigger[15])
			{
				m_bEventTrigger[15] = true;
				m_pWeapon->Set_Target_CanAttack(true);
			}
		}

		if (0.267f >AniTime)
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
	//s1 0.35 s2 0.65 s3 0.9
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::SSword_Atk_S01 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.35f))
		{
			m_eState = HUNTER_ANI::SSword_Atk_S02;

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
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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
	else if (HUNTER_ANI::SSword_Atk_S02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.65f))
		{
			m_eState = HUNTER_ANI::SSword_Atk_S03;

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
			}
		}
		else if (2.033f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::SSword_Atk_S03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			cout << "강콤보 끝" << endl;
			m_fCoolDownMax = 1.2f;
			m_bCanCoolDown = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (3.867f < AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_pWeapon->Set_Enable_Trail(false);
			}
		}
		else if (3.567f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}
		else if (1.267f <= AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}
		else if (0.967f <= AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
			}
		}
		else if (0.633f <= AniTime)
		{
			if (false == m_bEventTrigger[14])
			{
				m_bEventTrigger[14] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_SSword_Combo_Diagonal_L()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (HUNTER_ANI::SSword_Atk_N02 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.55f))
		{
			m_eState = HUNTER_ANI::SSword_Atk_N03;

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
		else if (1.367f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_pWeapon->Set_Target_CanAttack(false);
			}
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
	else if (HUNTER_ANI::SSword_Atk_N03 == m_eState)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			cout << "우대각->횡 끝" << endl;
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
			}
		}
		else if (1.300f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_pWeapon->Set_Target_CanAttack(true);
				m_pWeapon->Set_Enable_Trail(true);
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

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CHunter::Play_Idle()
{
	if (true == m_bInRecognitionRange)
	{
		if (true == m_tObjParam.bCanAttack)
		{
			//인지, 공격 가능->대기
			m_eState = HUNTER_ANI::Idle;
		}
		else
		{
			//인지, 공격 불가->경계
			Function_RotateBody();
			m_eState = HUNTER_ANI::Idle;
		}
	}
	else
	{
		m_bCanIdleRandom = true;
		m_eState = HUNTER_ANI::Idle;
	}

	return;
}

void CHunter::Play_Stand()
{
	if (true == m_bInRecognitionRange)
	{
		if (HUNTER_ANI::Stand == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = HUNTER_ANI::Stand_End;
		}
		else if (HUNTER_ANI::Stand_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bCanIdleRandom = true;
				m_eState = HUNTER_ANI::Idle;
			}
		}
	}
	else
	{
		m_eState = HUNTER_ANI::Stand;
	}

	return;
}

void CHunter::Play_Crouch()
{
	if (true == m_bInRecognitionRange)
	{
		if (HUNTER_ANI::Crouch == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = HUNTER_ANI::Crouch_End;
		}
		else if (HUNTER_ANI::Crouch_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bCanIdleRandom = true;
				m_eState = HUNTER_ANI::Idle;
			}
		}
	}
	else
	{
		m_eState = HUNTER_ANI::Crouch;
	}

	return;
}

void CHunter::Play_Sit()
{
	if (true == m_bInRecognitionRange)
	{
		if (HUNTER_ANI::Sit == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = HUNTER_ANI::Sit_End;
		}
		else if (HUNTER_ANI::Sit_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.95f))
			{
				m_bCanIdleRandom = true;
				m_eState = HUNTER_ANI::Idle;
			}
		}
	}
	else
	{
		m_eState = HUNTER_ANI::Sit;
	}

	return;
}

void CHunter::Play_Run()
{
	m_eState = HUNTER_ANI::Run;

	Function_RotateBody();

	Function_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CHunter::Play_Walk()
{
	m_eState = HUNTER_ANI::Walk_F;

	Function_RotateBody();

	Function_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CHunter::Play_Dodge()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_tObjParam.bDodge)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bDodge = true;
		m_eState = HUNTER_ANI::Dodge;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			m_tObjParam.bCanAttack = true;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (0.900f < AniTime && 1.300f > AniTime)
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
	}

	return;
}

void CHunter::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;
		m_eState = HUNTER_ANI::Dmg01_FR;
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
			}
		}
	}

	return;
}

void CHunter::Play_CC()
{
}

void CHunter::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDeadAni)
	{
		Function_ResetAfterAtk();

		m_bCanPlayDeadAni = true;

		if (HUNTER_ANI::DmgBlow_B == m_eState)
			m_eState = HUNTER_ANI::Death_B;
		else if (HUNTER_ANI::DmgBlow_F == m_eState)
			m_eState = HUNTER_ANI::Death_F;
		else
			m_eState = HUNTER_ANI::Death;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bEnable = false;
			m_dAniPlayMul = 0;
		}
		else
		{
			if (1.30f < AniTime && 2.80f > AniTime)
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
			else if (5.233f < AniTime)
			{
				if (false == m_bEventTrigger[1])
				{
					m_bEventTrigger[1] = true;
					Start_Dissolve(0.8f, false, true);
					m_pWeapon->Start_Dissolve(0.8f, false, true);
				}
			}
		}
	}

	return;
}

HRESULT CHunter::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Hunter_Black", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

	return S_OK;
}

HRESULT CHunter::SetUp_ConstantTable()
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
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CHunter::Ready_Weapon()
{
	//m_eWeaponState 값에 따라 무기를 다양하게 받아오게끔 설정합시다
	m_pWeapon = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pWeapon->Change_WeaponData(CWeapon::WPN_SSword_Normal);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pWeapon->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pWeapon->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CHunter::Ready_Collider()
{
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(3);

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
	//공격 구 넣기

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.6f; //크기 확인

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_RightForeArm]->_41, m_matBone[Bone_RightForeArm]->_42, m_matBone[Bone_RightForeArm]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.6f; //크기 확인

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_LeftKnee]->_41, m_matBone[Bone_LeftKnee]->_42, m_matBone[Bone_LeftKnee]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.6f; //크기 확인

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
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightForeArm", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_RightForeArm] = &pFrame->CombinedTransformationMatrix; //팔꿈치

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftLeg", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_LeftKnee] = &pFrame->CombinedTransformationMatrix; //무릎

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftToeBase", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_LeftToe] = &pFrame->CombinedTransformationMatrix; // 발끝

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix; // 머리

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CHunter* CHunter::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHunter* pInstance = new CHunter(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CYachaMan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHunter::Clone_GameObject(void * pArg)
{
	CHunter* pInstance = new CHunter(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CYachaMan");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHunter::Free()
{
	//Safe_Release(m_pMonsterUI);
	Safe_Release(m_pWeapon);
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

	return;
}
