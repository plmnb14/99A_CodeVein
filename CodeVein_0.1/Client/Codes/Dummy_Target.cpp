#include "stdafx.h"
#include "..\Headers\Dummy_Target.h"

CDummy_Target::CDummy_Target(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDummy_Target::CDummy_Target(const CDummy_Target & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_Target::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDummy_Target::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->Set_Pos(_v3(2.f, 0.f, 2.f));
	m_pTransform->Set_Scale(_v3(1.f, 1.f, 1.f));

	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanHit = true;

	m_pDynamic_Mesh->SetUp_Animation(m_eTmpAnimNum);

	return NOERROR;
}

_int CDummy_Target::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_tObjParam.bCanHit == false)
	{
		if (m_dwHitCnt > 5)
		{
			m_eActState = ACT_BREAK;
		}
		else
		{
			m_eActState = ACT_HIT;
		}
	}

	else
		m_eActState = ACT_IDLE;

	/*
	if (m_bOnGuard)
	{
		switch (m_eTmpAnimNum)
		{
		case Guard_Start:
		{
			if (m_pDynamic_Mesh->Is_Finish_Animation(0.8f))
			{
				m_eTmpAnimNum = CDummy_Target::Guard_Loop;
			}

			break;
		}

		case Guard_Loop:
		{
			if (m_pDynamic_Mesh->Is_Finish_Animation(0.95f))
			{
				m_eTmpAnimNum = CDummy_Target::Guard_End;
			}
			break;
		}

		case Guard_End:
		{
			if (m_pDynamic_Mesh->Is_Finish_Animation(0.8f))
			{
				m_eTmpAnimNum = CDummy_Target::Idle_State;
				m_bOnGuard = false;
			}
			break;
		}
		}
	}

	else if (false == m_bOnGuard)
	{
		m_fTimer += (_float)TimeDelta;
		m_eTmpAnimNum = Idle_State;

		if (m_fTimer > 5.f)
		{
			m_eTmpAnimNum = Guard_Start;

			m_bOnGuard = true;
			m_fTimer = 0.f;
		}
	}
	*/

	switch (m_eActState)
	{
	case ACT_IDLE:
	{
		if (false == m_bOnGuard)
		{
			if (m_eTmpAnimNum != Guard_Start)
				m_eTmpAnimNum = Guard_Start;

			else if (m_pDynamic_Mesh->Is_Finish_Animation(0.75f))
			{
				m_eTmpAnimNum = Guard_Loop;
				m_bOnGuard = true;
			}
		}

		else if (true == m_bOnGuard)
		{
			m_eTmpAnimNum = Guard_Loop;
		}
		break;
	}
	case ACT_HIT:
	{
		if (m_eTmpAnimNum != Guard_Hit_01)
		{
			cout << "맞는중" << endl;

			m_bOnGuard = false;
			m_dwHitCnt += 1;
			m_eTmpAnimNum = Guard_Hit_01;

			m_fSkillMoveSpeed_Cur = 4.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;
		}

		else
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamic_Mesh->Is_Finish_Animation(0.75f))
			{
				cout << "아이들로" << endl;

				m_bOnGuard = false;
				m_eActState = ACT_IDLE;
				m_tObjParam.bCanHit = true;
			}
		}

		break;
	}

	case ACT_BREAK:
	{
		if (m_eTmpAnimNum != Guard_Break)
		{
			m_bOnGuard = false;
			m_eTmpAnimNum = Guard_Break;

			m_fSkillMoveSpeed_Cur = 7.f;
			m_fSkillMoveAccel_Cur = 0.f;
			m_fSkillMoveMultiply = 0.5f;
		}

		else
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransform->Get_Axis(AXIS_Z));

			if (m_pDynamic_Mesh->Is_Finish_Animation(0.9f))
			{
				m_bOnGuard = false;
				m_tObjParam.bCanHit = true;
				m_eActState = ACT_IDLE;
			}
			break;
		}
	}
	}


	return NO_EVENT;
}

_int CDummy_Target::Late_Update_GameObject(_double TimeDelta)
{
	m_dTimeDelta = TimeDelta;

	//m_pDynamic_Mesh->SetUp_Animation(m_eTmpAnimNum);

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return _int();
}

HRESULT CDummy_Target::Render_GameObject()
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pDynamic_Mesh)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pDynamic_Mesh->Play_Animation(m_dTimeDelta);

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pDynamic_Mesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamic_Mesh->Get_NumMaterials(i);

		m_pDynamic_Mesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_pShader->Begin_Pass(0);

			if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pDynamic_Mesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			m_pShader->Commit_Changes();

			m_pDynamic_Mesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	Update_Collder();
	Draw_Collider();

	return NOERROR;
}

void CDummy_Target::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	// 네비 없이
	m_pTransform->Add_Pos(fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), tmpLook);

	// 네비게이션 적용하면 
	//m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CDummy_Target::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

HRESULT CDummy_Target::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_NormalGenji", L"Com_MeshDynamic", (CComponent**)&m_pDynamic_Mesh)))
		return E_FAIL;

	m_pDynamic_Mesh->SetUp_Animation_Lower(0);
	m_pDynamic_Mesh->SetUp_Animation_Upper(0);
	m_pDynamic_Mesh->SetUp_Animation_RightArm(0);

	return NOERROR;
}

HRESULT CDummy_Target::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

void CDummy_Target::Ready_BoneMatrix()
{
	LPCSTR tmpChar = "Head";

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pDynamic_Mesh->Get_BonInfo(tmpChar, 0);

	m_matBones[Bone_Head] = &pFamre->CombinedTransformationMatrix;

	tmpChar = "Spine";

	pFamre = (D3DXFRAME_DERIVED*)m_pDynamic_Mesh->Get_BonInfo(tmpChar, 0);

	m_matBones[Bone_Range] = &pFamre->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFamre->CombinedTransformationMatrix;
}

void CDummy_Target::Ready_Collider()
{
	//==============================================================================================================
	m_vecPhysicCol.reserve(10);
	//==============================================================================================================
	// 콜라이더 받아오고
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================
}

void CDummy_Target::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CDummy_Target::Update_Collder()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = *m_matBones[matrixIdx] * m_pTransform->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}
}

void CDummy_Target::OnCollisionEnter()
{
	Update_Collder();

	// =============================================================================================
	// 충돌
	// =============================================================================================

	//OnCollisionEvent_Physic(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));

	// =============================================================================================
}

void CDummy_Target::OnCollisionEvent_Physic(list<CGameObject*> plistGameObject)
{
	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (iter->Get_Target_CanHit())
					{
						iter->Set_Target_CanHit(false);
						iter->Add_Target_Hp(m_tObjParam.fDamage);

					}
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

CDummy_Target * CDummy_Target::Create(_Device pGraphic_Device)
{
	CDummy_Target* pInstance = new CDummy_Target(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDummy_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummy_Target::Clone_GameObject(void * pArg)
{
	CDummy_Target* pInstance = new CDummy_Target(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDummy_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Target::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pDynamic_Mesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();
}
