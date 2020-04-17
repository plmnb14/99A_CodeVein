#include "stdafx.h"
#include "..\Headers\Drain_Weapon.h"

CDrain_Weapon::CDrain_Weapon(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CDrain_Weapon::CDrain_Weapon(const CDrain_Weapon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CDrain_Weapon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDrain_Weapon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	m_fAnimMultiply = 1.f;
	m_pMesh_Dynamic->SetUp_Animation(m_eAnimnum);

	Find_TailHeadBone();

	return NOERROR;
}

_int CDrain_Weapon::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	Cacl_AttachBoneTransform();

	OnCollisionEnter();

	return NO_EVENT;
}

_int CDrain_Weapon::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	if (m_bActive)
	{
		m_pMesh_Dynamic->SetUp_Animation(m_eAnimnum);

		if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
		//if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
		//	return E_FAIL;
	}

	return NO_EVENT;
}

HRESULT CDrain_Weapon::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMesh_Dynamic->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh_Dynamic->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		m_pMesh_Dynamic->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pMesh_Dynamic->Get_MaterialPass(i, j);

			m_pShader->Begin_Pass(m_iPass);

			m_pShader->Set_DynamicTexture_Auto(m_pMesh_Dynamic, i, j);

			m_pShader->Commit_Changes();

			m_pMesh_Dynamic->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	Draw_Collider();

	//g_pManagement->Gizmo_Draw_Capsule(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius());

	return NOERROR;
}

HRESULT CDrain_Weapon::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	_mat		ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransform->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_uint iNumMeshContainer = _uint(m_pMesh_Dynamic->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh_Dynamic->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		m_pMesh_Dynamic->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pMesh_Dynamic->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);

			pShader->Set_DynamicTexture_Auto(m_pMesh_Dynamic, i, j);

			pShader->Commit_Changes();

			m_pMesh_Dynamic->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return NOERROR;
}

void CDrain_Weapon::Set_Active(_bool _bActiveDrain)
{
	m_bActive = _bActiveDrain;

	if (false == m_bActive)
	{
		m_pMesh_Dynamic->SetUp_Animation(0);
		m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	}
}

void CDrain_Weapon::Set_ActiveCollider(_bool _bActiveCollider)
{
	m_pCollider->Set_Enabled(_bActiveCollider);
}

void CDrain_Weapon::Set_ResetOldAnimIdx()
{
	m_pMesh_Dynamic->Reset_OldIndx();
}

void CDrain_Weapon::Set_AnimIdx(_ulong _eAnimState)
{
	switch (_eAnimState)
	{
		// Parry
	case 375:
	{
		m_eAnimnum = Drain_Parry;
		break;
	}

	// Suck_Charge_Start
	case 376:
	{
		m_eAnimnum = Drain_Charge_Start;
		break;
	}

	// Suck_Charge_End
	case 377:
	{
		m_eAnimnum = Drain_Charge_End;
		break;
	}

	// Suck_Combo
	case 378:
	{
		m_eAnimnum = Drain_ComboSuck;
		break;
	}

	// Suck_Ground
	case 379:
	{
		m_eAnimnum = Drain_GroundSuck;
		break;
	}

	// Suck_Special_01
	case 380:
	{
		m_eAnimnum = Drain_SpecialSuck_01;
		break;
	}

	// Suck_Special_02
	case 381:
	{
		m_eAnimnum = Drain_SpecialSuck_02;
		break;
	}

	}
}

void CDrain_Weapon::Set_Enable_Record(_bool _bOnRecord)
{
	m_bRecordCollision = _bOnRecord;

	if (false == m_bRecordCollision)
	{
		Clear_CollisionRecordList();
	}
}

void CDrain_Weapon::Find_TailHeadBone()
{
	LPCSTR tmpChar = "GCTailSword";

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMesh_Dynamic->Get_BonInfo(tmpChar, 0);

	m_matTailHead = &pFamre->CombinedTransformationMatrix;
}

void CDrain_Weapon::OnCollisionEnter()
{
	if (false == m_tObjParam.bCanAttack)
		return;

	OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
}

void CDrain_Weapon::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 1 : 다 의 기술일 경우, 기록을 켜야 한대만 맞는다.
		if (m_bRecordCollision)
		{
			if (false == Update_CollisionRecord(iter))
				continue;
		}

		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

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
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}

						if (false == iter->Get_Target_IsDodge())
						{
							m_tObjParam.fDamage = 100.f;

							// 무기 공격력의 +-20%까지 랜덤범위
							// 몬스터 HP바 확인을 위해 데미지 추가해놓음 - Chae
							_uint min = (_uint)(m_tObjParam.fDamage - (m_tObjParam.fDamage * 0.2f));
							_uint max = (_uint)(m_tObjParam.fDamage + (m_tObjParam.fDamage * 0.2f));

							iter->Add_Target_Hp(-(_float)CALC::Random_Num(min, max) * 1.f);
							g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

							if (m_bRecordCollision)
							{
								m_listCollisionRecord.push_back(iter);
							}
						}
					}

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

void CDrain_Weapon::Update_Trails(_double TimeDelta)
{
}

void CDrain_Weapon::Update_Collider()
{
	_v3 tmpPos;
	memcpy(tmpPos, &m_matTailHead->_41, sizeof(_v3));

	m_pCollider->Update(tmpPos);
}

void CDrain_Weapon::Draw_Collider()
{
	g_pManagement->Gizmo_Draw_Sphere(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius().x);
}

_bool CDrain_Weapon::Update_CollisionRecord(CGameObject * pObject)
{
	for (auto& iter : m_listCollisionRecord)
	{
		// 순회하다가 같으면
		if (iter == pObject)
			return false;
	}

	return true;
}

void CDrain_Weapon::Clear_CollisionRecordList()
{
	for (auto& iter : m_listCollisionRecord)
	{
		iter = nullptr;
	}
}

void CDrain_Weapon::Change_WeaponMesh(const _tchar* _MeshName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_szName, _MeshName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_szName, _MeshName);

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_DynamicMesh");

	// 둘 다 해제
	Safe_Release(m_pMesh_Dynamic);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _MeshName));
	Safe_AddRef(iter->second);
}

HRESULT CDrain_Weapon::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Drain_LongTail", L"Com_DynamicMesh", (CComponent**)&m_pMesh_Dynamic)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	lstrcpy(m_szName, L"Mesh_Drain_LongTail");

	m_pCollider->Set_Radius(_v3{ 0.6f, 0.6f, 0.6f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CapsuleLength(1.8f);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y * 2.f , 0.f });

	return NOERROR;
}

HRESULT CDrain_Weapon::SetUp_Default()
{
	// Transform
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	return S_OK;
}

HRESULT CDrain_Weapon::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	return NOERROR;
}

void CDrain_Weapon::Cacl_AttachBoneTransform()
{
	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	memcpy(&tmpMat._41, &(*m_pmatAttach)._41, sizeof(_v3));

	m_pTransform->Calc_ParentMat(&(*m_pmatParent));
}

CDrain_Weapon * CDrain_Weapon::Create(_Device pGraphic_Device)
{
	CDrain_Weapon*	pInstance = new CDrain_Weapon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating Prototype DrainWeapon");
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CDrain_Weapon::Clone_GameObject(void * pArg)
{
	CDrain_Weapon*	pInstance = new CDrain_Weapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating Clone DrainWeapon");
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDrain_Weapon::Free()
{
	m_pmatAttach = nullptr;
	m_pmatParent = nullptr;

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh_Dynamic);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();
}
