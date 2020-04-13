#include "stdafx.h"
#include "..\Headers\Cocoon.h"

CCocoon::CCocoon(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CCocoon::CCocoon(const CCocoon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCocoon::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CCocoon::Ready_GameObject(void * pArg)
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

	m_tObjParam.fHp_Max = 70.f;
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fDamage = 20.f; //투사체가 필요한가?
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;
	m_tObjParam.bHitAgain = false;
	m_tObjParam.bCanAttack = true;
	m_tObjParam.bIsAttack = false;

	m_bInRecognitionRange = false; //인지 범위 여부
	m_bInMistAtkRange = false;
	m_bInShotAtkRange = false;

	m_bCanCoolDown = false; //쿨타임 여부
	m_bIsCoolDown = false; //쿨타임 진행중 여부

	m_fRecognitionRange = 20.f;
	m_fMistAtkRange = 5.f;
	m_fShotAtkRange = 10.f;
	m_fCoolDownMax = 0.f;
	m_fCoolDownCur = 0.f;
	m_fSpeedForCollisionPush = 2.f;
	m_iRandom = 0;

	return NOERROR;
}

_int CCocoon::Update_GameObject(_double TimeDelta)
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

_int CCocoon::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CCocoon::Render_GameObject()
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

HRESULT CCocoon::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

	return NOERROR;
}

void CCocoon::Update_Collider()
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

void CCocoon::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return;
}

void CCocoon::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));

	return;
}

void CCocoon::Check_CollisionPush()
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

void CCocoon::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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

	return;
}

void CCocoon::Check_Hit()
{
	if (MONSTER_ANITYPE::DEAD == m_eFirstCategory)
	{
		return;
	}
	else
	{
		if (false == m_tObjParam.bCanHit)
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

void CCocoon::Check_FBLR()
{	
	//피격 앞,뒤 체크
	m_eSecondCategory_HIT = Cocoon_HITTYPE::HIT_HIT_F;
	//m_eSecondCategory_HIT = WOLF_HITTYPE::HIT_HIT_B;

	return;
}

void CCocoon::Check_Dist()
{
	if (MONSTER_ANITYPE::HIT == m_eFirstCategory ||
		MONSTER_ANITYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsHit)
		return;

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fShotAtkRange >= fLenth ? m_bInShotAtkRange = true : m_bInShotAtkRange = false;
	m_fMistAtkRange >= fLenth ? m_bInMistAtkRange = true : m_bInMistAtkRange = false;


	if (true == m_bInRecognitionRange) //인지o
	{
		if (true == m_bInShotAtkRange) //원거리공격범위 o
		{
			if (true == m_bInMistAtkRange) //방사공격범위o
			{
				if (true == m_tObjParam.bCanAttack) //공격가능
				{
					if (true == m_bIsCoolDown) //쿨타임중o
					{
						m_eFirstCategory = MONSTER_ANITYPE::IDLE; //회피 대기 경계
						m_eSecondCategory_IDLE = Cocoon_IDLETYPE::IDLE_THREAT;
						Function_RotateBody();
					}
					else //쿨타임x
					{
						m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
						m_eState = COCOON_ANI::Mist;
						Function_RotateBody();
					}
				}
				else
				{
					m_eFirstCategory = MONSTER_ANITYPE::IDLE; //회피 대기 경계
					m_eSecondCategory_IDLE = Cocoon_IDLETYPE::IDLE_THREAT;
					Function_RotateBody();
				}
			}
			else //방사공격범위x
			{
				if (true == m_tObjParam.bCanAttack) //공격가능
				{
					if (true == m_bIsCoolDown) //쿨타임중o
					{
						m_eFirstCategory = MONSTER_ANITYPE::IDLE; //회피 대기 경계
						m_eSecondCategory_IDLE = Cocoon_IDLETYPE::IDLE_THREAT;
						Function_RotateBody();
					}
					else //쿨타임중x
					{
						m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
						m_eState = COCOON_ANI::Shot;
						Function_RotateBody();
					}
				}
				else //공격불가
				{
					m_eFirstCategory = MONSTER_ANITYPE::IDLE; //회피 대기 경계
					m_eSecondCategory_IDLE = Cocoon_IDLETYPE::IDLE_THREAT;
					Function_RotateBody();
				}
			}
		}
		else //공격범위x
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE; //회피 대기 경계
			m_eSecondCategory_IDLE = Cocoon_IDLETYPE::IDLE_IDLE;
			Function_RotateBody();
		}
	}
	else //인지x
	{
		m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		m_eSecondCategory_IDLE = Cocoon_IDLETYPE::IDLE_IDLE;
	}

	return;
}

void CCocoon::Set_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_ANITYPE::IDLE:
		switch (m_eSecondCategory_IDLE)
		{
		case Client::CCocoon::IDLE_IDLE:
			Play_Idle();
			break;
		case Client::CCocoon::IDLE_THREAT:
			m_eState = Threat;
			break;
		}
		break;

	case MONSTER_ANITYPE::ATTACK:
		switch (m_eState)
		{
		case Client::CCocoon::Shot:
			Play_Shot();
			break;
		case Client::CCocoon::Mist:
			Play_Mist();
			break;
		}
		break;

	case MONSTER_ANITYPE::HIT:
		switch (m_eSecondCategory_HIT)
		{
		case Client::CCocoon::HIT_HIT_F:
			Play_Hit();
			break;
		case Client::CCocoon::HIT_HIT_B:
			Play_Hit();
			break;
		}
		break;

	case MONSTER_ANITYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CCocoon::Function_RotateBody()
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

void CCocoon::Function_CoolDown()
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

void CCocoon::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CCocoon::Function_DecreMoveMent(_float _fMutiply)
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

void CCocoon::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true; //피격 가능
	m_tObjParam.bIsHit = false; //피격중 x

	m_tObjParam.bDodge = false;  //회피아님

	m_tObjParam.bIsAttack = false; //공격중x

	m_vecAttackCol[0]->Set_Enabled(false);

	LOOP(10)
		m_bEventTrigger[i] = false;

	return;
}

void CCocoon::Play_Idle()
{
	if (true == m_bInRecognitionRange) //인지o
	{
		if (true == m_tObjParam.bCanAttack)
		{
			//인지, 공격 가능->대기
			m_eState = COCOON_ANI::Idle;
			return;
		}
	}
	else //인지x
	{
		m_eState = COCOON_ANI::Idle;
		return;
	}

	return;
}

void CCocoon::Play_Shot()
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
			//5733 생성, //6267 발사
			if (5.733f < AniTime && 6.267f > AniTime)
			{
			}
			else if (6.267f <= AniTime)
			{

			}
		}
	}

	return;
}

void CCocoon::Play_Mist()
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
			m_fCoolDownMax = 0.8f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			//현재 시간만 따둠
			//3.033 생성 3.567 방사 6.900 종료
			if (3.033f < AniTime && 3.567f > AniTime)
			{
				//입안에 머금고 있다
			}
			else if (3.567f < AniTime && 6.900f > AniTime)
			{
				//방사
			}
			else if (6.900f <= AniTime)
			{
				//종료
			}
		}
	}

	return;
}

void CCocoon::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;
		m_eState = COCOON_ANI::Dmg_F;
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

void CCocoon::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDeadAni)
	{
		Function_ResetAfterAtk();
		m_bCanPlayDeadAni = true;
		m_eState = COCOON_ANI::Dead;
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
			if (4.467f < AniTime)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;
					Start_Dissolve(0.6f, false, true);
				}
			}
		}
	}

	return;
}

HRESULT CCocoon::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Cocoon_Normal", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CCocoon::SetUp_ConstantTable()
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

HRESULT CCocoon::Ready_Collider()
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

HRESULT CCocoon::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Jaw_Tongue1", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CCocoon* CCocoon::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCocoon* pInstance = new CCocoon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CCocoon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCocoon::Clone_GameObject(void * pArg)
{
	CCocoon* pInstance = new CCocoon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CCocoon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCocoon::Free()
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
