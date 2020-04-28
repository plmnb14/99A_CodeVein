#include "stdafx.h"
#include "..\Headers\Urchin.h"

CUrchin::CUrchin(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CUrchin::CUrchin(const CUrchin & rhs)
	:CGameObject(rhs)
{
}

HRESULT CUrchin::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CUrchin::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
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

_int CUrchin::Update_GameObject(_double TimeDelta)
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

	return S_OK;
}

_int CUrchin::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return S_OK;
}

HRESULT CUrchin::Render_GameObject()
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

HRESULT CUrchin::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

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

void CUrchin::Update_Collider()
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

void CUrchin::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return;
}

void CUrchin::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));

	return;
}

void CUrchin::Check_CollisionPush()
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

void CUrchin::Check_CollisionEvent(list<CGameObject*> plistGameObject)
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

void CUrchin::Function_FBLR()
{
	_float angle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos()));

	if (MONSTER_STATETYPE::HIT == m_eFirstCategory)
	{
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

void CUrchin::Function_RotateBody()
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

void CUrchin::Function_CoolDown()
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

void CUrchin::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CUrchin::Function_DecreMoveMent(_float _fMutiply)
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

void CUrchin::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_tObjParam.bSuperArmor = false;

	m_bCanIdle = true;
	m_bIsIdle = false;

	m_tObjParam.bIsAttack = false;

	LOOP(20)
		m_bEventTrigger[i] = false;

	return;
}

void CUrchin::Check_PosY()
{
	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return;
}

void CUrchin::Check_Hit()
{
	if (MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	if (0 < m_tObjParam.fHp_Cur)
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (true == m_tObjParam.bIsHit)
			{
				if (true == m_tObjParam.bHitAgain)
				{
					m_eFirstCategory = MONSTER_STATETYPE::HIT;
					Function_FBLR();
					m_tObjParam.bHitAgain = false;
					m_pMeshCom->Reset_OldIndx();
				}
				else
				{
					m_eFirstCategory = MONSTER_STATETYPE::HIT;
					Function_FBLR();
				}
			}
		}
	}
	else
		m_eFirstCategory = MONSTER_STATETYPE::DEAD;

	return;
}

void CUrchin::Check_Dist()
{
	if (MONSTER_STATETYPE::HIT == m_eFirstCategory ||
		MONSTER_STATETYPE::CC == m_eFirstCategory ||
		MONSTER_STATETYPE::DEAD == m_eFirstCategory)
		return;

	if (true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsHit)
		return;

	if (nullptr == m_pTargetTransform)
	{
		Function_ResetAfterAtk();

		m_eFirstCategory = MONSTER_STATETYPE::IDLE;

		return;
	}
	else
	{
		_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

		m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
		m_fShotRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

		if (true == m_bInRecognitionRange)
		{
			if (true == m_bInAtkRange)
			{
				if (true == m_tObjParam.bCanAttack)
				{
					if (true == m_bIsCoolDown)
					{
						m_eFirstCategory = MONSTER_STATETYPE::IDLE;
						Function_RotateBody();
					}
					else
					{
						m_eFirstCategory = MONSTER_STATETYPE::ATTACK;
						Function_RotateBody();
					}
				}
				else
				{
					m_eFirstCategory = MONSTER_STATETYPE::IDLE;
					Function_RotateBody();
				}
			}
			else
			{
				m_eFirstCategory = MONSTER_STATETYPE::IDLE;
				Function_RotateBody();
			}
		}
		else
		{
			m_eFirstCategory = MONSTER_STATETYPE::IDLE;
		}
	}

	return;
}

void CUrchin::Check_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_STATETYPE::IDLE:
		Play_Idle();
		break;

	case MONSTER_STATETYPE::ATTACK:
		if (false == m_tObjParam.bIsAttack)
		{
			m_tObjParam.bCanAttack = false;
			m_tObjParam.bIsAttack = true;

			switch (CALC::Random_Num(URCHIN_ANI::Atk_Step, URCHIN_ANI::Atk_Rush))
			{
			case URCHIN_ANI::Atk_Rush:
				m_eState = Atk_Rush;
			case URCHIN_ANI::Atk_Step:
				m_eState = Atk_Step;
			}
		}
		else
		{
			switch (m_eState)
			{
			case URCHIN_ANI::Atk_Step:
				Play_Rolling();
				break;
			case URCHIN_ANI::Atk_Rush:
				Play_RollingRush();
				break;
			}
		}
		break;

	case MONSTER_STATETYPE::HIT:
		Play_Hit();
		break;

	case MONSTER_STATETYPE::CC:
		break;

	case MONSTER_STATETYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CUrchin::Check_DeadEffect(_double TimeDelta)
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

void CUrchin::Play_Rolling()
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
		else if (2.467f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.900f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.700f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.527f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.433f <= AniTime)
		{
			if (false == m_bEventTrigger[11])
			{
				m_bEventTrigger[11] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.113f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.100f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.813f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.800f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.533f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.500f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.200f <= AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (1.900f < AniTime && 2.467f > AniTime)
		{
			if (false == m_bEventTrigger[12])
			{
				m_bEventTrigger[12] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
		else if (0.200f < AniTime && 1.700f > AniTime)
		{
			if (false == m_bEventTrigger[13])
			{
				m_bEventTrigger[13] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CUrchin::Play_RollingRush()
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
		else if (1.733f <= AniTime)
		{
			if (false == m_bEventTrigger[0])
			{
				m_bEventTrigger[0] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.503f <= AniTime)
		{
			if (false == m_bEventTrigger[1])
			{
				m_bEventTrigger[1] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.465f <= AniTime)
		{
			if (false == m_bEventTrigger[2])
			{
				m_bEventTrigger[2] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (1.107f <= AniTime)
		{
			if (false == m_bEventTrigger[3])
			{
				m_bEventTrigger[3] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (1.001f <= AniTime)
		{
			if (false == m_bEventTrigger[4])
			{
				m_bEventTrigger[4] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.703f <= AniTime)
		{
			if (false == m_bEventTrigger[5])
			{
				m_bEventTrigger[5] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.633f <= AniTime)
		{
			if (false == m_bEventTrigger[6])
			{
				m_bEventTrigger[6] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.378f <= AniTime)
		{
			if (false == m_bEventTrigger[7])
			{
				m_bEventTrigger[7] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}
		else if (0.313f <= AniTime)
		{
			if (false == m_bEventTrigger[8])
			{
				m_bEventTrigger[8] = true;
				m_vecAttackCol[0]->Set_Enabled(false);
				m_tObjParam.bSuperArmor = false;
			}
		}
		else if (0.f <= AniTime)
		{
			if (false == m_bEventTrigger[9])
			{
				m_bEventTrigger[9] = true;
				m_vecAttackCol[0]->Set_Enabled(true);
				m_tObjParam.bSuperArmor = true;
			}
		}

		if (2.467f > AniTime)
		{
			if (false == m_bEventTrigger[10])
			{
				m_bEventTrigger[10] = true;
				m_fSkillMoveSpeed_Cur = 10.f;
				m_fSkillMoveAccel_Cur = 1.f;
				m_fSkillMoveMultiply = 0.5f;
			}

			Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}
}

void CUrchin::Play_Idle()
{
	if (true == m_bInRecognitionRange)
	{
		m_bIsIdle = false;

		if (true == m_tObjParam.bCanAttack)
		{
			m_eState = URCHIN_ANI::Idle;
			Function_RotateBody();
		}
		else
		{
			m_eState = URCHIN_ANI::Idle;
			Function_RotateBody();
		}
	}
	else
	{
		m_bIsIdle = true;
		m_eState = URCHIN_ANI::Idle;
	}

	return;
}

void CUrchin::Play_Hit()
{
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;
		m_eState = URCHIN_ANI::Dmg;
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
			}
		}
	}

	return;
}

void CUrchin::Play_Dead()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (false == m_bCanPlayDead)
	{
		Function_ResetAfterAtk();
		m_bCanPlayDead = true;
		m_eState = URCHIN_ANI::Death;
	}
	else
	{
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
				Start_Dissolve(0.8f, false, true);
			}
		}
	}

	return;
}

HRESULT CUrchin::Add_Component(void * pArg)
{
	_tchar MeshName[MAX_STR] = L"";

	MONSTER_STATUS eTemp = *(MONSTER_STATUS*)pArg;

	if (nullptr == pArg)
		lstrcpy(MeshName, L"Mesh_Urchin_Black");
	else
	{
		switch (eTemp.eMonsterColor)
		{
		case MONSTER_COLORTYPE::RED:
		case MONSTER_COLORTYPE::BLUE:
		case MONSTER_COLORTYPE::YELLOW:
		case MONSTER_COLORTYPE::COLOR_NONE:
		case MONSTER_COLORTYPE::BLACK:
			lstrcpy(MeshName, L"Mesh_Urchin_Black");
			break;
		case MONSTER_COLORTYPE::WHITE:
			lstrcpy(MeshName, L"Mesh_Urchin_White");
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

HRESULT CUrchin::SetUp_ConstantTable()
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

HRESULT CUrchin::Ready_Status(void * pArg)
{
	m_tObjParam.fDamage = -100.f;
	m_tObjParam.fHp_Max = 750.f;
	m_tObjParam.fArmor_Max = 10.f;

	m_fRecognitionRange = 30.f;
	m_fShotRange = 20.f;
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

HRESULT CUrchin::Ready_Collider()
{
	m_vecAttackCol.reserve(2);
	m_vecPhysicCol.reserve(3);

	CCollider* pCollider = nullptr;
	_float fRadius;

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
	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);
	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);
	m_vecPhysicCol.push_back(pCollider);

	IF_NULL_VALUE_RETURN(pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider")), E_FAIL);
	fRadius = 0.7f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);
	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CUrchin::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = nullptr;

	IF_NULL_VALUE_RETURN(pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Hips", 0), E_FAIL);
	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CUrchin * CUrchin::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CUrchin* pInstance = new CUrchin(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CUrchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUrchin::Clone_GameObject(void * pArg)
{
	CUrchin* pInstance = new CUrchin(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CUrchin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUrchin::Free()
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

