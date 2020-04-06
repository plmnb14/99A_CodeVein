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

	m_tObjParam.fHp_Max = 140.f; //4~5대 사망, 기본공격력 20+-5에서 피감소
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;

	m_tObjParam.bCanHit = true;
	m_tObjParam.bCanAttack = true;
	m_tObjParam.bDodge = false;

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

void CYachaMan::Update_Collider()
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
	if (DEAD == m_eFirstIdentify)
		return;

	if (true == m_tObjParam.bIsHit)
	{
		return;
	}
	else
	{
		if (false == m_tObjParam.bCanHit)
		{
			if (0 >= m_tObjParam.fHp_Cur)
			{
				m_eFirstIdentify = DEAD;
				return;
			}
			else
			{
				m_eFirstIdentify = HIT;
				return;
			}
		}
	}
}

void CYachaMan::Check_Dist()
{
	if (HIT == m_eFirstIdentify ||
		DOWN == m_eFirstIdentify ||
		DEAD == m_eFirstIdentify)
		return;

	if (true == m_tObjParam.bIsAttack)
	{
		return;
	}

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAttackRange >= fLenth ? m_bInAttackRange = true : m_bInAttackRange = false;

	if (true == m_bInRecognitionRange)
	{
		if (true == m_bInAttackRange)
		{
			if (true == m_tObjParam.bCanAttack)
			{
				if (true == m_bIsCoolDown)
				{
					//m_eState = Threat;
					return;
				}
				else
				{
					m_eFirstIdentify = MONSTER_ANITYPE::ATTACK;
					//m_iAttackRandomNumber = CALC::Random_Num(BiteLRL, Frisbee);
					return;
				}
			}
			else
			{
				m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
				return;
			}
		}
		else
		{
			m_bCanChase = true;
			m_eFirstIdentify = MONSTER_ANITYPE::MOVE;
			return;
		}
	}
	else
	{
		m_bCanChase = false;
		m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
		//m_iIdleRandomNumber = CALC::Random_Num(Eat, Idle);

		//if (Eat_End == m_iIdleRandomNumber)
		//	m_iIdleRandomNumber = Eat;

		return;
	}
}

void CYachaMan::Set_AniEvent()
{
	switch (m_eFirstIdentify)
	{
	case MONSTER_ANITYPE::IDLE:
		if (m_bInRecognitionRange)
		{
			switch (m_eState)
			{
			case YACHAMAN_ANI::Hammer_Idle:
				Play_Idle();
				break;
			case YACHAMAN_ANI::Eat:
			case YACHAMAN_ANI::Eat_End:
				//Play_Eat();
				break;
			}
		}
		else
		{
			//switch (m_iIdleRandomNumber)
			//{
			//case YACHAMAN_ANI::Idle:
			//	Play_Idle();
			//	break;
			//case YACHAMAN_ANI::Eat:
			//case YACHAMAN_ANI::Eat_End:
			//	//Play_Eat();
			//	break;
			//case YACHAMAN_ANI::Sit:
			//case YACHAMAN_ANI::Sit_End:
			//	//Play_Sit();
			//	break;
			//}
		}

		break;

	case MONSTER_ANITYPE::MOVE:
		if (true == m_bCanChase)
			Play_Run();
		//if(true == m_tObjParam.bDodge) //200406수정
		//	Play_Dodge();
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
	if (false == m_tObjParam.bIsAttack && false == m_tObjParam.bCanAttack)
	{
		m_fCoolDown += DELTA_60;
		m_bIsCoolDown = true;

		if (2.f <= m_fCoolDown)
		{
			m_fCoolDown = 0.f;
			m_bIsCoolDown = false;
			m_tObjParam.bCanAttack = true;
			m_eFirstIdentify = MONSTER_ANITYPE::IDLE;
		}
	}
}

void CYachaMan::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	// 네비 없이
	m_pTransformCom->Add_Pos(_fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), _vDir);

	// 네비게이션 적용하면 
	//m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
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
	m_tObjParam.bIsAttack = false;

	LOOP(10)
		m_bEventTrigger[i] = false;

	return;
}

void CYachaMan::Play_Idle()
{
}

void CYachaMan::Play_Lurk()
{
}

void CYachaMan::Play_Glance()
{
}

void CYachaMan::Play_LookAround()
{
}

void CYachaMan::Play_Eat()
{
}

void CYachaMan::Play_Walk()
{
}

void CYachaMan::Play_Run()
{
}

//void CYachaMan::Play_Dodge()
//{
//}

void CYachaMan::Play_RandomAtk()
{
}

void CYachaMan::Play_ShoulderAtk()
{
}

void CYachaMan::Play_LeftRightAtk()
{
}

void CYachaMan::Play_LeftAndHammering()
{
}

void CYachaMan::Play_ShoulderAtk_LeftAtk()
{
}

void CYachaMan::Play_ShoulderAtk_TurnTwice()
{
}

void CYachaMan::Play_TargetHammering()
{
}

void CYachaMan::Play_WheelWind()
{
}

void CYachaMan::Play_RunHammering()
{
}

void CYachaMan::Play_Hit()
{
}

void CYachaMan::Play_Down_Strong()
{
}

void CYachaMan::Play_Down_Weak()
{
}

void CYachaMan::Play_Dead()
{
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
